#include <variant>

#include <llvm/IR/Verifier.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>
#include <semantic_analyzer/type.h>

#include "code_generation_visitor.h"

static const std::string kMainFunctionName = "main";
static const std::string kPrintfFunctionName = "printf";
static const std::string kCharFmtString = "%c";
static const std::string kIntFmtString = "%d";
static const std::string kStrFmtString = "%s";
static const char kLineFeedChar = 10;

using code_gen_func_type_1_operand = llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *);
using code_gen_func_type_2_operand = llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *, llvm::Value *);

CodeGenerationVisitor::CodeGenerationVisitor(std::string module_id)
    : context(std::make_unique<llvm::LLVMContext>())
    , module(std::make_unique<llvm::Module>(module_id, *context))
    , builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
}

void CodeGenerationVisitor::WriteIRToFD(int fd) {
    llvm::raw_fd_ostream ir_ostream(fd, /* shouldClose = */ false);
    module->print(ir_ostream, nullptr);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(AssignmentStatementNode& node) {
    auto target = node.GetChild(0);
    if (target->GetParseNodeType() == ParseNodeType::IDENTIFIER_NODE) {
        IdentifierNode *identifier = static_cast<IdentifierNode *>(target);
        auto alloca_inst = identifier->FindAlloca();

        // Generate code for new value.
        auto new_value = node.GetChild(1)->GenerateCode(*this);

        // Store boolean values as 8-bit integers.
        if (node.GetChild(1)->GetType()->EquivalentTo(TypeEnum::BOOLEAN)) {
            new_value = builder->CreateZExt(new_value, llvm::Type::getInt8Ty(*context),
                    "zexttmp");
        }

        builder->CreateStore(new_value, alloca_inst);
        return new_value;
    }
    else {
        return CodeGenerationInternalError("non-targettable expression in assingment statement");
    }
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode& node) {
    for (auto child : node.GetChildren()) {
        child->GenerateCode(*this);
    }
    return nullptr; // This method's return value is not used anywhere.
}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    auto identifier_node = dynamic_cast<IdentifierNode *>(node.GetChild(0));

    // Generate code for initializer value.
    llvm::Value *initializer_value = node.GetChild(1)->GenerateCode(*this);
    
    // Store boolean values as 8-bit integers.
    if (node.GetChild(1)->GetType()->EquivalentTo(TypeEnum::BOOLEAN)) {
        initializer_value = builder->CreateZExt(initializer_value, llvm::Type::getInt8Ty(*context),
                "zexttmp");
    }

    // Allocate stack memory for variables in the entry block of the function.
    auto parent_function = builder->GetInsertBlock()->getParent();
    llvm::AllocaInst *alloca_inst = CreateEntryBlockAlloca(parent_function,
            identifier_node->GetName(), initializer_value->getType());

    auto symbol_table_entry_opt = identifier_node->FindSymbolTableEntry();
    if (!symbol_table_entry_opt.has_value()) {
        CodeGenerationInternalError("missing entry in symbol table for "
                + identifier_node->ToString());
    }
    else {
        SymbolTableEntry &symbol_table_entry = symbol_table_entry_opt.value();
        symbol_table_entry.alloca = alloca_inst;
    }

    builder->CreateStore(initializer_value, alloca_inst);

    return initializer_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IfStatementNode& node) {
    llvm::Value *condition = node.GetChild(0)->GenerateCode(*this);
    if (!condition) {
        return CodeGenerationInternalError("failed generating condition for if-statement");
    }
    // Truncate condition to make sure it has LLVM type `i8`.
    condition = builder->CreateTrunc(condition, llvm::Type::getInt1Ty(*context), "trunctmp");

    auto parent_function = builder->GetInsertBlock()->getParent();

    // Create blocks for 'then' and 'else' cases, and the block where the control merges. Note that
    // we do not require the programmer to provide an 'else' block, hence we leave it as null for
    // now to be initialized later.
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(*context, "then", parent_function);
    llvm::BasicBlock *else_block = nullptr;
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(*context, "ifcont");

    if (node.HasElseBlock()) {
        // This is where we initialize 'else' if it was provided.
        else_block = llvm::BasicBlock::Create(*context, "else");
        builder->CreateCondBr(condition, then_block, else_block);
    }
    else {
        builder->CreateCondBr(condition, then_block, merge_block);
    }

    // Emit 'then' block.
    builder->SetInsertPoint(then_block);
    node.GetChild(1)->GenerateCode(*this);
    // Create a break statement to merge control flow.
    builder->CreateBr(merge_block);
    then_block = builder->GetInsertBlock();

    if (node.HasElseBlock()) {
        // Emit 'else' block.
        parent_function->insert(parent_function->end(), else_block);
        builder->SetInsertPoint(else_block);
        node.GetChild(2)->GenerateCode(*this);
        // Create a break statement to merge control flow.
        builder->CreateBr(merge_block);
        else_block = builder->GetInsertBlock();
    }

    // Emit 'merge' block.
    parent_function->insert(parent_function->end(), merge_block);
    builder->SetInsertPoint(merge_block);

    return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(MainNode& node) {
    // Main always returns void
    llvm::Type *return_type = llvm::Type::getVoidTy(*context);

    // Main does not take any arguments
    llvm::FunctionType *function_type = llvm::FunctionType::get(return_type,
            /* IsVarArgs = */ false);

    // Create an LLVM::Function object
    llvm::Function *main_func = llvm::Function::Create(function_type,
            llvm::Function::ExternalLinkage,
            kMainFunctionName,
            *module);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(*context, "", main_func);
    builder->SetInsertPoint(entry_block);

    // Generate for 'main' code block
    node.GetChild(0)->GenerateCode(*this);

    // Main always returns void
    builder->CreateRetVoid();

    llvm::verifyFunction(*main_func);

    return main_func;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(OperatorNode& node) {
    unsigned num_operands = node.GetChildren().size();
    if (num_operands == 1) {
        llvm::Value *operand = node.GetChild(0)->GenerateCode(*this);

        // Obtain codegen function pointer for 1 operand from variant.
        try {
            auto fp = std::get<code_gen_func_type_1_operand>(node.GetCodeGenFunc());
            return fp(context.get(), builder.get(), operand);
        }
        catch (std::bad_variant_access const& ex) {
            return CodeGenerationInternalError(
                    "bad variant access when generating code for 1 operand");
        }

    }
    if (num_operands == 2) {
        llvm::Value *lhs = node.GetChild(0)->GenerateCode(*this);
        llvm::Value *rhs = node.GetChild(1)->GenerateCode(*this);

        // Obtain codegen function pointer for 2 operands from variant.
        try {
            auto fp = std::get<code_gen_func_type_2_operand>(node.GetCodeGenFunc());
            return fp(context.get(), builder.get(), lhs, rhs);
        }
        catch (std::bad_variant_access const& ex) {
            return CodeGenerationInternalError(
                    "bad variant access when generating code for 2 operands");
        }
    }
    else {
        return CodeGenerationInternalError("code generation not implemented for "
                + std::to_string(num_operands) + " operands");
    }
}

llvm::Value *CodeGenerationVisitor::GenerateCode(PrintStatementNode& node) {
    llvm::Value *ret_value = nullptr;

    // We call printf for each 'operand'
    for (auto child : node.GetChildren()) {
        ret_value = PrintValue(child->GenerateCode(*this), *child->GetType());
    }

    // Print line feed once we are finished
    ret_value = PrintLineFeed();

    return ret_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    GeneratePrintfDeclaration();
    
    GenerateGlobalConstants();

    auto result = node.GetChild(0)->GenerateCode(*this);

    llvm::verifyModule(*module);

    return result;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    // Search for an `alloca` in the symbol table.
    auto alloca_instr = node.FindAlloca();
    if (!alloca_instr) {
        CodeGenerationInternalError("unable to find alloca instruction for "
                + node.ToString() + " in symbol table");
    }

    return builder->CreateLoad(alloca_instr->getAllocatedType(), alloca_instr, node.GetName());
}

//--------------------------------------------------------------------------------------//
//                            Code generation for constants                             //
//--------------------------------------------------------------------------------------//
llvm::Value *CodeGenerationVisitor::GenerateCode(BooleanLiteralNode& node) {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), (int)node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CharacterLiteralNode& node) {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(StringLiteralNode& node) {
    return builder->CreateGlobalString(node.GetValue(), "", 0, module.get());
}

//--------------------------------------------------------------------------------------//
//                               Print statement helpers                                //
//--------------------------------------------------------------------------------------//
void CodeGenerationVisitor::GeneratePrintfDeclaration() {
    // Create a vector for parameters
    std::vector<llvm::Type *> parameters(1, llvm::PointerType::getUnqual(*context));
    // Create a function type returning a 32-bit int, taking 1 parameter and a variable number of
    // arguments
	llvm::FunctionType *printf_func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context),
            parameters, /* IsVarArg = */ true);

    // Create the function declaration
	llvm::Function *printf_func = llvm::Function::Create(printf_func_type,
            llvm::Function::ExternalLinkage, kPrintfFunctionName, *module);
    if (!printf_func) {
        CodeGenerationInternalError("could not generate declaration for printf");
    }
}

void CodeGenerationVisitor::GeneratePrintfFmtStrings() {
    llvm::Value *fmt_str = nullptr;

    if (fmt_str = GenerateFmtString(TypeEnum::CHARACTER, kCharFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for CHARACTER type");
    }
    global_constants_table[kCharFmtString] = fmt_str;

    if (fmt_str = GenerateFmtString(TypeEnum::INTEGER, kIntFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for INTEGER type");
    }
    global_constants_table[kIntFmtString] = fmt_str;

    if (fmt_str = GenerateFmtString(TypeEnum::STRING, kStrFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for STRING type");
    }
    global_constants_table[kStrFmtString] = fmt_str;
}

llvm::Value *CodeGenerationVisitor::GenerateFmtString(TypeEnum type_enum, std::string fmt_str) {
    return builder->CreateGlobalString(fmt_str, ".fmt_" + Type::GetTypeEnumString(type_enum), 0,
            module.get());
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtString(TypeEnum type_enum) {
    const std::string *key = nullptr;
    switch (type_enum) {
        case TypeEnum::CHARACTER:
            key = &kCharFmtString;
            break;
        case TypeEnum::BOOLEAN:
        case TypeEnum::INTEGER:
            key = &kIntFmtString;
            break;
        case TypeEnum::STRING:
            key = &kStrFmtString;
            break;
        default:
            return CodeGenerationInternalError("unimplemented format string for type "
                    + Type::GetTypeEnumString(type_enum));
    }

    if (!global_constants_table.contains(*key)) {
        return CodeGenerationInternalError("unimplemented format string");
    }
    return global_constants_table.at(*key);
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtString(const Type& type) {
    return GetPrintfFmtString(type.GetTypeEnum());
}

llvm::Value *CodeGenerationVisitor::PrintLineFeed() {
    return PrintValue(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), kLineFeedChar),
            Type(TypeEnum::CHARACTER));
}

llvm::Value *CodeGenerationVisitor::PrintValue(llvm::Value *value, const Type& type) {
    llvm::Function *printf_func = module->getFunction(kPrintfFunctionName);
    if (!printf_func) {
        return CodeGenerationInternalError("unable to obtain llvm::Function pointer for printf");
    }

    std::vector<llvm::Value *> printf_args;

    printf_args.push_back(GetPrintfFmtString(type));
    if (printf_args.back() == nullptr) {
        return CodeGenerationInternalError("failed obtaining llvm::Value object for fmt string");
    }

    auto print_value = value;

    // When printing booleans, we first truncate to 1-bit such that only the least significant bit
    // remains. Then we extend to 32 bits and print the boolean as an integer.
    if (type == TypeEnum::BOOLEAN) {
        auto truncated_value = builder->CreateTrunc(print_value, llvm::Type::getInt1Ty(*context),
                "trunctmp");
        print_value = builder->CreateZExt(truncated_value, llvm::Type::getInt32Ty(*context),
                "zexttmp");
    }
    // When printing characters, we sign-extend to 32 bits.
    else if (type == TypeEnum::CHARACTER) {
        print_value = builder->CreateSExt(print_value, llvm::Type::getInt32Ty(*context), "sexttmp");
    }

    printf_args.push_back(print_value);
    if (printf_args.back() == nullptr) {
        return CodeGenerationInternalError("failed to generate argument for printf");
    }

    return builder->CreateCall(printf_func, printf_args, "printf_ret");
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void CodeGenerationVisitor::GenerateGlobalConstants() {
    GeneratePrintfFmtStrings();
}

llvm::AllocaInst *CodeGenerationVisitor::CreateEntryBlockAlloca(llvm::Function *function,
        const std::string& identifier_name, llvm::Type *llvm_type)
{
    llvm::IRBuilder<> tmp_builder(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmp_builder.CreateAlloca(llvm_type, nullptr, identifier_name);
}

//--------------------------------------------------------------------------------------//
//                                    Error handling                                    //
//--------------------------------------------------------------------------------------//
llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode& node) {
    return CodeGenerationInternalError("encountered ErrorNode " + node.ToString());
}

llvm::Value *CodeGenerationVisitor::CodeGenerationInternalError(std::string message) {
    return (llvm::Value *)PunktLogger::LogFatalInternalError(message);
}
