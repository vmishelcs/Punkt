#include <variant>

#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/type.h>

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
        auto alloca_inst = identifier->FindLLVMAlloca();

        // Generate code for new value.
        auto new_value = node.GetChild(1)->GenerateCode(*this);

        builder->CreateStore(new_value, alloca_inst);
        return new_value;
    }

    return CodeGenerationInternalError("non-targettable expression in assingment statement");
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode& node) {
    for (auto child : node.GetChildren()) {
        child->GenerateCode(*this);
    }

    // GenerateCode(CodeBlockNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    auto identifier_node = dynamic_cast<IdentifierNode *>(node.GetChild(0));

    // Generate code for initializer value.
    llvm::Value *initializer_value = node.GetChild(1)->GenerateCode(*this);

    // Allocate stack memory for variables in the entry block of the function.
    auto parent_function = builder->GetInsertBlock()->getParent();
    llvm::AllocaInst *alloca_inst = CreateEntryBlockAlloca(parent_function,
            identifier_node->GetName(), initializer_value->getType());

    // Store alloca instruction in the symbol table to be used for loads.
    identifier_node->SetLLVMAlloca(alloca_inst);

    builder->CreateStore(initializer_value, alloca_inst);

    return initializer_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ForStatementNode& node) {
    // Emit code for the loop initializer.
    node.GetChild(0)->GenerateCode(*this);

    auto parent_function = builder->GetInsertBlock()->getParent();

    // Create a basic block to check if the loop condition is true/false.
    auto condition_block = llvm::BasicBlock::Create(*context, "condcheck", parent_function);

    // Create a basic block for the loop and add it to the parent function.
    auto loop_block = llvm::BasicBlock::Create(*context, "loop");

    // Create a basic block for the loop exit.
    auto afterloop_block = llvm::BasicBlock::Create(*context, "afterloop");

    // Insert an explicit fall-through from the current block (before the loop) to the loop
    // condition block.
    builder->CreateBr(condition_block);

    // Now we are inserting instructions into the loop condition block.
    builder->SetInsertPoint(condition_block);

    // Check if the condition to enter the loop is satisfied.
    auto end_condition = node.GetChild(1)->GenerateCode(*this);
    end_condition = builder->CreateTrunc(end_condition, llvm::Type::getInt1Ty(*context),
            "trunctmp");
    builder->CreateCondBr(end_condition, loop_block, afterloop_block);

    // Append the 'loop' block after the 'condition' block.
    parent_function->insert(parent_function->end(), loop_block);

    // Now we are inserting instructions into the loop block.
    builder->SetInsertPoint(loop_block);

    // Emit loop body.
    node.GetChild(3)->GenerateCode(*this);

    // Emit code for the step value, appending it to the end of the loop body.
    node.GetChild(2)->GenerateCode(*this);

    // Create an unconditional branch to the start of the loop, where we check if the condition to
    // continue looping is true/false.
    builder->CreateBr(condition_block);
    
    // Append the 'afterloop' block after the 'loop' block. We jump to this block if the loop
    // condition fails.
    parent_function->insert(parent_function->end(), afterloop_block);

    // Any new code will be inserted in the 'afterloop' block.
    builder->SetInsertPoint(afterloop_block);

    // GenerateCode(ForStatementNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(FunctionDefinitionNode& node) {
    auto identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        return CodeGenerationInternalError("unable to find function definition identifier");
    }

    auto lambda_node = node.GetLambdaNode();
    if (!lambda_node) {
        return CodeGenerationInternalError("unable to find function definition lambda node");
    }

    auto function = static_cast<llvm::Function *>(lambda_node->GenerateCode(*this));
    identifier_node->SetLLVMFunction(function);

    // GenerateCode(FunctionDefinitionNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IfStatementNode& node) {
    llvm::Value *condition = node.GetChild(0)->GenerateCode(*this);
    if (!condition) {
        return CodeGenerationInternalError("failed generating condition for if-statement");
    }
    // Truncate condition to make sure it has LLVM type `i8`.
    condition = builder->CreateTrunc(condition, llvm::Type::getInt1Ty(*context), "trunctmp");

    auto parent_function = builder->GetInsertBlock()->getParent();

    // Create blocks for 'then' and 'else' cases, and the 'merge' block where the control merges.
    // Note that we do not require the programmer to provide an 'else' clause, hence we leave it as
    // null for now to be initialized later.
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

    // GenerateCode(IfStatementNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaInvocationNode& node) {
    // TODO: Add check that node.GetIdentifierNode()->GenerateCode(*this) returns llvm::Function *.
    // This is risky... but it's what we must do for anonymous lambdas.
    llvm::Function *callee_f = static_cast<llvm::Function *>
            (node.GetIdentifierNode()->GenerateCode(*this));

    std::vector<llvm::Value *> arg_values;
    for (const auto& arg : node.GetArgumentNodes()) {
        arg_values.push_back(arg->GenerateCode(*this));
    }

    return builder->CreateCall(callee_f, arg_values, "calltmp");
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaNode& node) {
    std::vector<llvm::Type *> param_types;
    std::vector<LambdaParameterNode *> param_nodes = node.GetParameterNodes();
    param_types.reserve(param_nodes.size());

    // Create a vector holding llvm::Type pointers describing the parameter types.
    for (const auto& param_node : param_nodes) {
        param_types.push_back(param_node->GetTypeNode()->GetLLVMType(*context));
    }

    // Create the function type.
    auto function_type = llvm::FunctionType::get(
            node.GetReturnTypeNode()->GetLLVMType(*context), param_types, /*isVarArg=*/false);

    // Create the function.
    auto function = llvm::Function::Create(
            function_type, llvm::Function::ExternalLinkage, "usrfunc", module.get());

    // Set names for arguments.
    unsigned i = 0;
    for (auto &arg : function->args()) {
        arg.setName(param_nodes[i]->GetIdentifierNode()->GetName());
        ++i;
    }

    // Check if this lambda is anonymous.
    if (!node.IsAnonymous()) {
        // If it is not anonymous, store the function signature in the identifier's symbol table
        // entry.
        auto func_def_node = static_cast<FunctionDefinitionNode *>(node.GetParent());
        auto identifier_node = func_def_node->GetIdentifierNode();
        identifier_node->SetLLVMFunction(function);
    }

    // Set up builder to generate function body.
    auto function_entry_block = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(function_entry_block);

    // Allocate memory for arguments.
    i = 0;
    for (auto &arg : function->args()) {
        IdentifierNode *arg_identifier_node = param_nodes[i]->GetIdentifierNode();

        // Create an alloca for this parameter.
        llvm::AllocaInst *alloca = CreateEntryBlockAlloca(function, arg_identifier_node->GetName(),
                arg.getType());

        // Store alloca instruction in the symbol table.
        arg_identifier_node->SetLLVMAlloca(alloca);

        // Store initial value into the alloca.
        builder->CreateStore(&arg, alloca);

        ++i;
    }

    // Generate code for the function body.
    node.GetLambdaBodyNode()->GenerateCode(*this);

    // Validate generated code.
    llvm::verifyFunction(*function);

    return function;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaParameterNode& node) {
    // GenerateCode(LambdaParameterNode&) is not used.
    return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(MainNode& node) {
    // Main always returns void
    llvm::Type *return_type = llvm::Type::getVoidTy(*context);

    // Main does not take any arguments
    llvm::FunctionType *function_type = llvm::FunctionType::get(return_type, /*isVarArgs=*/false);

    // Create an LLVM::Function object
    llvm::Function *main_func = llvm::Function::Create(function_type,
            llvm::Function::ExternalLinkage,
            kMainFunctionName,
            *module);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(*context, "entry", main_func);
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
    // We call printf for each 'operand'
    for (auto child : node.GetChildren()) {
        PrintValue(child->GenerateCode(*this), child->GetType());
    }

    // Print line feed once we are finished.
    PrintLineFeed();

    // GenerateCode(ProgramNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    GeneratePrintfDeclaration();
    
    GenerateGlobalConstants();

    // Generate code for each function of the program.
    for (const auto& child : node.GetChildren()) {
        child->GenerateCode(*this);
    }

    llvm::verifyModule(*module);

    // GenerateCode(ProgramNode&) return value is not used.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ReturnStatementNode& node) {
    llvm::Value *ret_value = node.GetChild(0)->GenerateCode(*this);
    return builder->CreateRet(ret_value);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    // Look up the symbol table entry for this identifier.
    auto symbol_table_entry_opt = node.FindSymbolTableEntry();
    if (!symbol_table_entry_opt.has_value()) {
         CodeGenerationInternalError("missing entry in symbol table for "
                + node.ToString());
    }
    
    auto symbol_table_entry = symbol_table_entry_opt.value().get();

    llvm::AllocaInst *alloca_inst = nullptr;
    llvm::Function *function = nullptr;
    switch (symbol_table_entry.symbol_type) {
        case SymbolType::VARIABLE:
            alloca_inst = node.FindLLVMAlloca();
            if (!alloca_inst) {
                CodeGenerationInternalError("unable to find alloca for " + node.ToString());
            }
            return builder->CreateLoad(alloca_inst->getAllocatedType(), alloca_inst,
                    node.GetName());

        case SymbolType::FUNCTION:
            function = node.FindLLVMFunction();
            if (!function) {
                CodeGenerationInternalError("function signature is not set for " + node.ToString());
            }
            return function;

        default:
            return (llvm::Value *)
                    PunktLogger::LogFatalInternalError("unimplemented symbol type in "
                    "CodeGenerationVisitor::GenerateCode(IdentifierNode&).");
    }
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

llvm::Value *CodeGenerationVisitor::GenerateCode(TypeNode& node) {
    // GenerateCode(TypeNode&) is not used.
    return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(NopNode& node) {
    return builder->CreateAdd(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0),
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0),
        "nop"
    );
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

void CodeGenerationVisitor::GeneratePrintfFmtStringsForBaseTypes() {
    llvm::Value *fmt_str = nullptr;

    if (fmt_str = GenerateFmtStringForBaseType(BaseTypeEnum::CHARACTER, kCharFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for CHARACTER type");
    }
    global_constants_table[kCharFmtString] = fmt_str;

    if (fmt_str = GenerateFmtStringForBaseType(BaseTypeEnum::INTEGER, kIntFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for INTEGER type");
    }
    global_constants_table[kIntFmtString] = fmt_str;

    if (fmt_str = GenerateFmtStringForBaseType(BaseTypeEnum::STRING, kStrFmtString), !fmt_str) {
        CodeGenerationInternalError("failed to generate format string for STRING type");
    }
    global_constants_table[kStrFmtString] = fmt_str;
}

llvm::Value *CodeGenerationVisitor::GenerateFmtStringForBaseType(BaseTypeEnum base_type_enum,
        std::string fmt_str)
{
    return builder->CreateGlobalString(fmt_str, ".fmt_" + BaseType::GetEnumString(base_type_enum), 0,
            module.get());
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtString(Type *type) {
    if (type->GetTypeEnum() == TypeEnum::BASE_TYPE) {
        BaseType *base_type = dynamic_cast<BaseType *>(type);
        if (!base_type) {
            return CodeGenerationInternalError("failed to cast Type to BaseType");
        }

        return GetPrintfFmtStringForBaseType(base_type->GetBaseTypeEnum());
    }
    return CodeGenerationInternalError(
            "CodeGenerationVisitor::GetPrintfFmtString not implemented for non-base types");
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtStringForBaseType(BaseTypeEnum base_type_enum) {
    const std::string *key = nullptr;
    switch (base_type_enum) {
        case BaseTypeEnum::CHARACTER:
            key = &kCharFmtString;
            break;
        case BaseTypeEnum::BOOLEAN:
        case BaseTypeEnum::INTEGER:
            key = &kIntFmtString;
            break;
        case BaseTypeEnum::STRING:
            key = &kStrFmtString;
            break;
        default:
            return CodeGenerationInternalError("unimplemented format string for type "
                    + BaseType::GetEnumString(base_type_enum));
    }

    if (!global_constants_table.contains(*key)) {
        return CodeGenerationInternalError("unimplemented format string");
    }
    return global_constants_table.at(*key);
}

llvm::Value *CodeGenerationVisitor::PrintValue(llvm::Value *value, Type *type) {
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
    BaseType *base_type = dynamic_cast<BaseType *>(type);
    if (base_type && base_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
        auto truncated_value = builder->CreateTrunc(print_value, llvm::Type::getInt1Ty(*context),
                "trunctmp");
        print_value = builder->CreateZExt(truncated_value, llvm::Type::getInt32Ty(*context),
                "zexttmp");
    }
    // When printing characters, we sign-extend to 32 bits.
    else if (base_type && base_type->IsEquivalentTo(BaseTypeEnum::CHARACTER)) {
        print_value = builder->CreateSExt(print_value, llvm::Type::getInt32Ty(*context), "sexttmp");
    }

    printf_args.push_back(print_value);
    if (printf_args.back() == nullptr) {
        return CodeGenerationInternalError("failed to generate argument for printf");
    }

    return builder->CreateCall(printf_func, printf_args, "printf_ret");
}

llvm::Value *CodeGenerationVisitor::PrintLineFeed() {
    auto temp_char_base_type = BaseType::Create(BaseTypeEnum::CHARACTER);
    return PrintValue(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), kLineFeedChar),
            temp_char_base_type.get());
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void CodeGenerationVisitor::GenerateGlobalConstants() {
    GeneratePrintfFmtStringsForBaseTypes();
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
