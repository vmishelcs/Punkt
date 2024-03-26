#include <utility>

#include <glog/logging.h>
#include "llvm/IR/Verifier.h"
#include <llvm/TargetParser/Host.h>

#include "code_generation_visitor.h"


const std::string CodeGenerationVisitor::main_function_name = "main";

CodeGenerationVisitor::CodeGenerationVisitor()
    : context(std::make_unique<llvm::LLVMContext>())
    /* TODO: ModuleID should probably be the input source code file name */
    , module(std::make_unique<llvm::Module>("program_module", *context))
    , builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
}

std::string CodeGenerationVisitor::DumpLLVMIR() {
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
                << identifier_node.AsString();
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

    // Generate each statement within the function
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
    return nullptr;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    return nullptr;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}
