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

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode& node) {
    llvm::Value *ret_value = nullptr;
    for (ParseNode& child : node.GetChildren()) {
        ret_value = child.GenerateCode(*this);
        if (!ret_value) {
            return CodeGenerationInternalError("CodeBlockNode code generation error");
        }
    }
    return ret_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    IdentifierNode& identifier_node = dynamic_cast<IdentifierNode&>(node.GetChild(0));

    llvm::Value *v = node.GetChild(1).GenerateCode(*this);
    // Store boolean values as 8-bit integers.
    if (node.GetChild(1).GetType() == TypeEnum::BOOLEAN) {
        v = builder->CreateZExt(v, llvm::Type::getInt8Ty(*context), "zexttmp");
    }

    llvm::AllocaInst *store_value = builder->CreateAlloca(v->getType(), nullptr,
            llvm::Twine(identifier_node.GetName()));

    auto symbol_data_opt = identifier_node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        CodeGenerationInternalError("missing entry in symbol table for "
                + identifier_node.ToString());
    }
    else {
        SymbolData &symbol_data = symbol_data_opt.value();
        symbol_data.binding = store_value;
    }

    builder->CreateStore(v, store_value);

    return v;
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
    node.GetChild(0).GenerateCode(*this);

    // Main always returns void
    builder->CreateRetVoid();

    llvm::verifyFunction(*main_func);

    return main_func;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(OperatorNode& node) {
    unsigned num_operands = node.GetChildren().size();
    if (num_operands == 1) {
        llvm::Value *operand = node.GetChild(0).GenerateCode(*this);

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
        llvm::Value *lhs = node.GetChild(0).GenerateCode(*this);
        llvm::Value *rhs = node.GetChild(1).GenerateCode(*this);

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
    llvm::Function *printf_func = module->getFunction(kPrintfFunctionName);
    if (!printf_func) {
        return CodeGenerationInternalError("unable to obtain function pointer for printf");
    }

    llvm::Value *ret_value = nullptr;

    // We call printf for each 'operand'
    for (ParseNode& child : node.GetChildren()) {
        ret_value = PrintValue(child.GenerateCode(*this), child.GetType());
    }

    // Print line feed once we are finished
    ret_value = PrintLineFeed();

    return ret_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    GeneratePrintfDeclaration();
    
    GenerateGlobalConstants();

    auto result = node.GetChild(0).GenerateCode(*this);

    llvm::verifyModule(*module);

    return result;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    // Look up the identifier in the symbol table
    auto symbol_data_opt = node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        CodeGenerationInternalError("unable to find " + node.ToString() + " in symbol table");
    }

    const SymbolData& symbol_data = symbol_data_opt.value();
    return builder->CreateLoad(symbol_data.binding->getAllocatedType(), symbol_data.binding,
            node.GetName());
}

//--------------------------------------------------------------------------------------//
//                            Code generation for constants                             //
//--------------------------------------------------------------------------------------//
llvm::Value *CodeGenerationVisitor::GenerateCode(BooleanLiteralNode& node) {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), (int)node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(StringLiteralNode& node) {
    std::string str = node.GetValue();
    auto char_type = llvm::Type::getInt8Ty(*context);

    // Initialize a vector of chars
    std::vector<llvm::Constant *> char_vec(str.length());
    for (unsigned i = 0, end = str.length(); i < end; ++i) {
        char_vec[i] = llvm::ConstantInt::get(char_type, str[i]);
    }
    char_vec.push_back(llvm::ConstantInt::get(char_type, 0)); // Null terminator

    // Create a string type based on array of chars
    auto string_type = llvm::ArrayType::get(char_type, char_vec.size());

    // Declare private global variable
    llvm::GlobalVariable *string_var =
            (llvm::GlobalVariable *)module->getOrInsertGlobal("", string_type);
    string_var->setInitializer(llvm::ConstantArray::get(string_type, char_vec));
    string_var->setConstant(true);
    string_var->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
    string_var->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    return string_var;
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
    auto char_type = llvm::Type::getInt8Ty(*context);
    std::vector<llvm::Constant *> char_vec(fmt_str.size());
    for (unsigned i = 0, end = fmt_str.length(); i < end; ++i) {
        char_vec[i] = llvm::ConstantInt::get(char_type, fmt_str[i]);
    }
    char_vec.push_back(llvm::ConstantInt::get(char_type, 0)); // Null terminator

    // Create a string type based on array of chars
    auto fmt_string_type = llvm::ArrayType::get(char_type, char_vec.size());

    // Declare private global variable
    llvm::GlobalVariable *fmt_string_var =
            (llvm::GlobalVariable *)module->getOrInsertGlobal(
            ".fmt_" + Type::GetTypeEnumString(type_enum), fmt_string_type);
    fmt_string_var->setInitializer(llvm::ConstantArray::get(fmt_string_type, char_vec));
    fmt_string_var->setConstant(true);
    fmt_string_var->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
    fmt_string_var->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    return fmt_string_var;
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
    llvm::Function *printf_func = module->getFunction(kPrintfFunctionName);
    if (!printf_func) {
        return CodeGenerationInternalError("unable to obtain function pointer for printf");
    }

    std::vector<llvm::Value *> args;

    args.push_back(GetPrintfFmtString(TypeEnum::CHARACTER));
    if (args.back() == nullptr) {
        return CodeGenerationInternalError("failed obtaining llvm::Value object for fmt string");
    }

    args.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), kLineFeedChar));
    if (args.back() == nullptr) {
        return CodeGenerationInternalError("failed to generate line feed argument for printf");
    }

    return builder->CreateCall(printf_func, args, "printf_ret");
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

//--------------------------------------------------------------------------------------//
//                                    Error handling                                    //
//--------------------------------------------------------------------------------------//
llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode& node) {
    return (llvm::Value *)PunktLogger::LogFatalInternalError(
            "encountered ErrorNode " + node.ToString());
}

llvm::Value *CodeGenerationVisitor::CodeGenerationInternalError(std::string message) {
    return (llvm::Value *)PunktLogger::LogFatalInternalError(message);
}
