#include <utility>

#include <glog/logging.h>
#include <llvm/IR/Verifier.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/GlobalAlias.h>
#include <llvm-c/Core.h>

#include "code_generation_visitor.h"


const std::string CodeGenerationVisitor::main_function_name = "main";

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
    bool is_var_args = false;
    llvm::FunctionType *function_type = llvm::FunctionType::get(return_type, is_var_args);

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
    return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    return node.GetChild(0).GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode& node) {
    return FatalCodeGenerationError("encountered ErrorNode " + node.ToString()
            + " in CodeGenerationVisitor");
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

llvm::Value *CodeGenerationVisitor::FatalCodeGenerationError(std::string error_msg) {
    LOG(FATAL) << "Internal Error -- " << error_msg;
    return nullptr;
}
