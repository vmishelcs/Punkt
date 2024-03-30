#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <map>

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
    llvm::Value *GenerateCode(StringLiteralNode& node);

private:
    void GenerateGlobalConstants();

    llvm::Value *GenerateBinaryOperatorCode(llvm::Value *lhs, llvm::Value *rhs,
            PunctuatorEnum punctuator);

    void GeneratePrintfDeclaration();
    void GeneratePrintfFmtStrings();
    llvm::Value *GenerateFmtString(TypeEnum type_enum, std::string fmt_str);
    llvm::Value *GetPrintfFmtString(TypeEnum type_enum);
    llvm::Value *GetPrintfFmtString(Type type);
    llvm::Value *PrintLineFeed();

    llvm::Value *FatalCodeGenerationError(std::string error_msg);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::map<std::string, llvm::Value *> global_constants_table;
};

#endif // CODE_GENERATION_VISITOR_H_
