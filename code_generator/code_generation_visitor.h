#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/ValueSymbolTable.h>

#include <parse_node/parse_nodes/all_nodes.h>

class CodeGenerationVisitor {
public:
    CodeGenerationVisitor(std::string module_id);

    std::string GetIRString();

    llvm::Value *GenerateCode(CodeBlockNode& node);

    llvm::Value *GenerateCode(DeclarationStatementNode& node);

    llvm::Value *GenerateCode(MainNode& node);

    llvm::Value *GenerateCode(OperatorNode& node);

    llvm::Value *GenerateCode(PrintStatementNode& node);

    llvm::Value *GenerateCode(ProgramNode& node);

    llvm::Value *GenerateCode(ErrorNode& node);
    llvm::Value *GenerateCode(IdentifierNode& node);
    llvm::Value *GenerateCode(IntegerLiteralNode& node);

private:
    llvm::Value *GenerateBinaryOperatorCode(llvm::Value *lhs, llvm::Value *rhs,
            PunctuatorEnum punctuator);

    llvm::Value *FatalCodeGenerationError(std::string error_msg);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    static const std::string main_function_name;
};

#endif // CODE_GENERATION_VISITOR_H_
