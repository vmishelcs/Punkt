#include <utility>

#include <glog/logging.h>
#include "llvm/IR/Verifier.h"
#include <llvm/TargetParser/Host.h>

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
    // Currently supports only one statement per code block
    return node.GetChild(0).GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    // Generate a binding for the value we want assigned to a variable
    llvm::Value *binding = node.GetChild(1).GenerateCode(*this);

    IdentifierNode& identifier_node = dynamic_cast<IdentifierNode&>(node.GetChild(0));
    auto symbol_data_opt = identifier_node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        LOG(FATAL) << "Internal error - missing entry in symbol table for "
                << identifier_node.ToString();
    }
    else {
        SymbolData &symbol_data = symbol_data_opt.value();
        symbol_data.binding = binding;
    }

    llvm::AllocaInst *alloca_instr = builder->CreateAlloca(
            binding->getType(), nullptr, llvm::Twine(identifier_node.GetName()));
    builder->CreateStore(binding, alloca_instr);

    return binding;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(MainNode& node) {
    // Main always returns void
    llvm::Type *return_type = llvm::Type::getVoidTy(*context);

    // Main does not take any arguments
    bool variable_number_of_args = false;
    llvm::FunctionType *function_type = llvm::FunctionType::get(return_type, variable_number_of_args);

    // Create an LLVM::Function object
    llvm::Function *main_func = llvm::Function::Create(function_type,
            llvm::Function::ExternalLinkage,
            main_function_name,
            *module);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(*context, "entry", main_func);
    builder->SetInsertPoint(entry_block);

    // Generate code for children of the MainNode
    for (ParseNode& child : node.GetChildren()) {
        child.GenerateCode(*this);
    }

    // Main always returns void
    builder->CreateRetVoid();

    llvm::verifyFunction(*main_func);

    return main_func;
}

void CodeGenerationVisitor::VisitEnter(OperatorNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(OperatorNode& node) {
    
}

void CodeGenerationVisitor::VisitEnter(PrintStatementNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(PrintStatementNode& node) {
    
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode& node) {
    return node.GetChild(0).GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode& node) {
    LOG(FATAL) << "encountered ErrorNode " << node.ToString() << " in CodeGenerationVisitor";
    return nullptr;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    // Look up the identifier in the symbol table
    auto symbol_data_opt = node.FindIdentifierSymbolData();
    if (!symbol_data_opt.has_value()) {
        LOG(FATAL) << "unable to find " << node.ToString() << " in symbol table";
    }

    const SymbolData& symbol_data = symbol_data_opt.value();
    return symbol_data.binding;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}
