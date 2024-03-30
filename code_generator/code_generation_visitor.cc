#include <glog/logging.h>
#include <llvm/IR/Verifier.h>
#include <llvm/TargetParser/Host.h>
#include <semantic_analyzer/type.h>

#include "code_generation_visitor.h"


static const std::string main_function_name = "main";
static const std::string printf_function_name = "printf";
static const std::string char_fmt_str = "%c";
static const std::string int_fmt_str = "%d";
static const std::string string_fmt_str = "%s";
static const char line_feed_char = 10;

CodeGenerationVisitor::CodeGenerationVisitor(std::string module_id)
    : context(std::make_unique<llvm::LLVMContext>())
    , module(std::make_unique<llvm::Module>(module_id, *context))
    , builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
}

std::string CodeGenerationVisitor::GetIRString() {
    std::string output;
    llvm::raw_string_ostream ir_ostream(output);
    module->print(ir_ostream, nullptr);
    return ir_ostream.str();
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode& node) {
    llvm::Value *ret_value = nullptr;
    for (ParseNode& child : node.GetChildren()) {
        ret_value = child.GenerateCode(*this);
        if (!ret_value) {
            return FatalCodeGenerationError("CodeBlockNode code generation error");
        }
    }
    return ret_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    llvm::Value *v = node.GetChild(1).GenerateCode(*this);
    IdentifierNode& identifier_node = dynamic_cast<IdentifierNode&>(node.GetChild(0));

    llvm::AllocaInst *alloca_instr = builder->CreateAlloca(
            v->getType(), nullptr, llvm::Twine(identifier_node.GetName()));

    auto symbol_data_opt = identifier_node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        FatalCodeGenerationError("missing entry in symbol table for "
                + identifier_node.ToString());
    }
    else {
        SymbolData &symbol_data = symbol_data_opt.value();
        symbol_data.binding = alloca_instr;
    }

    builder->CreateStore(v, alloca_instr);

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
            main_function_name,
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
    if (num_operands == 2) {
        llvm::Value *lhs = node.GetChild(0).GenerateCode(*this);
        llvm::Value *rhs = node.GetChild(1).GenerateCode(*this);
        return GenerateBinaryOperatorCode(lhs, rhs, node.GetPunctuatorEnum());
    }
    else {
        return FatalCodeGenerationError("code generation not implemented for "
                + std::to_string(num_operands) + " operands");
    }
}

llvm::Value *CodeGenerationVisitor::GenerateCode(PrintStatementNode& node) {
    llvm::Function *printf_func = module->getFunction(printf_function_name);
    if (!printf_func) {
        return FatalCodeGenerationError("unable to obtain function pointer for printf");
    }

    llvm::Value *ret_value = nullptr;

    // We call printf for each 'operand'
    for (ParseNode& child : node.GetChildren()) {
        std::vector<llvm::Value *> args;

        args.push_back(GetPrintfFmtString(child.GetType()));
        if (args.back() == nullptr) {
            return FatalCodeGenerationError("failed obtaining llvm::Value object for fmt string");
        }

        args.push_back(child.GenerateCode(*this));
        if (args.back() == nullptr) {
            return FatalCodeGenerationError("failed to generate argument for printf");
        }

        ret_value = builder->CreateCall(printf_func, args, "printf_ret");
    }

    // Print line feed once we are finished
    ret_value = PrintLineFeed();

    return ret_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    GeneratePrintfDeclaration();
    
    GenerateGlobalConstants();

    return node.GetChild(0).GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    // Look up the identifier in the symbol table
    auto symbol_data_opt = node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        FatalCodeGenerationError("unable to find " + node.ToString() + " in symbol table");
    }

    const SymbolData& symbol_data = symbol_data_opt.value();
    return builder->CreateLoad(symbol_data.binding->getAllocatedType(), symbol_data.binding,
            node.GetName());
}

//--------------------------------------------------------------------------------------//
//                            Code generation for constants                             //
//--------------------------------------------------------------------------------------//
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
//                               Binary operator helpers                                //
//--------------------------------------------------------------------------------------//
llvm::Value *CodeGenerationVisitor::GenerateBinaryOperatorCode(llvm::Value *lhs, llvm::Value *rhs,
        PunctuatorEnum punctuator)
{
    switch (punctuator) {
        case PunctuatorEnum::PLUS:
            return builder->CreateAdd(lhs, rhs, "addtmp");
        case PunctuatorEnum::MINUS:
            return builder->CreateSub(lhs, rhs, "subtmp");
        case PunctuatorEnum::MULTIPLY:
            return builder->CreateMul(lhs, rhs, "multmp");
        case PunctuatorEnum::DIVIDE:
            return builder->CreateSDiv(lhs, rhs, "divtmp");
        default:
            return FatalCodeGenerationError("unimplemented binary operator");
    }
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
            llvm::Function::ExternalLinkage, printf_function_name, *module);
    if (!printf_func) {
        FatalCodeGenerationError("could not generate declaration for printf");
    }
}

void CodeGenerationVisitor::GeneratePrintfFmtStrings() {
    llvm::Value *fmt_str = nullptr;

    if (fmt_str = GenerateFmtString(TypeEnum::CHARACTER, char_fmt_str), !fmt_str) {
        FatalCodeGenerationError("failed to generate format string for CHARACTER type");
    }
    global_constants_table[char_fmt_str] = fmt_str;

    if (fmt_str = GenerateFmtString(TypeEnum::INTEGER, int_fmt_str), !fmt_str) {
        FatalCodeGenerationError("failed to generate format string for INTEGER type");
    }
    global_constants_table[int_fmt_str] = fmt_str;

    if (fmt_str = GenerateFmtString(TypeEnum::STRING, string_fmt_str), !fmt_str) {
        FatalCodeGenerationError("failed to generate format string for STRING type");
    }
    global_constants_table[string_fmt_str] = fmt_str;
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
            key = &char_fmt_str;
            break;
        case TypeEnum::INTEGER:
            key = &int_fmt_str;
            break;
        case TypeEnum::STRING:
            key = &string_fmt_str;
            break;
        default:
            return FatalCodeGenerationError("unimplemented format string for type "
                    + Type::GetTypeEnumString(type_enum));
    }

    if (!global_constants_table.contains(*key)) {
        return FatalCodeGenerationError("unimplemented format string");
    }
    return global_constants_table.at(*key);
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtString(Type type) {
    return GetPrintfFmtString(type.GetTypeEnum());
}

llvm::Value *CodeGenerationVisitor::PrintLineFeed() {
    llvm::Function *printf_func = module->getFunction(printf_function_name);
    if (!printf_func) {
        return FatalCodeGenerationError("unable to obtain function pointer for printf");
    }

    std::vector<llvm::Value *> args;

    args.push_back(GetPrintfFmtString(TypeEnum::CHARACTER));
    if (args.back() == nullptr) {
        return FatalCodeGenerationError("failed obtaining llvm::Value object for fmt string");
    }

    args.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), line_feed_char));
    if (args.back() == nullptr) {
        return FatalCodeGenerationError("failed to generate line feed argument for printf");
    }

    return builder->CreateCall(printf_func, args, "printf_ret");
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
    return FatalCodeGenerationError("encountered ErrorNode " + node.ToString());
}

llvm::Value *CodeGenerationVisitor::FatalCodeGenerationError(std::string error_msg) {
    LOG(FATAL) << "Internal Error -- " << error_msg;
    return nullptr;
}
