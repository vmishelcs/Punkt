#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <parse_node/parse_node_ir_visitor.h>

class CodeGenerationVisitor : public ParseNodeIRVisitor {
public:
    CodeGenerationVisitor(std::string module_id);

    void WriteIRToFD(int fd);

    virtual llvm::Value *GenerateCode(CodeBlockNode& node) override;
    virtual llvm::Value *GenerateCode(DeclarationStatementNode& node) override;
    virtual llvm::Value *GenerateCode(MainNode& node) override;
    virtual llvm::Value *GenerateCode(OperatorNode& node) override;
    virtual llvm::Value *GenerateCode(PrintStatementNode& node) override;
    virtual llvm::Value *GenerateCode(ProgramNode& node) override;
    
    virtual llvm::Value *GenerateCode(ErrorNode& node) override;
    virtual llvm::Value *GenerateCode(IdentifierNode& node) override;
    virtual llvm::Value *GenerateCode(BooleanLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(IntegerLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(StringLiteralNode& node) override;

private:
    void GenerateGlobalConstants();

    void GeneratePrintfDeclaration();
    void GeneratePrintfFmtStrings();
    llvm::Value *GenerateFmtString(TypeEnum type_enum, std::string fmt_str);
    llvm::Value *GetPrintfFmtString(TypeEnum type_enum);
    llvm::Value *GetPrintfFmtString(Type type);
    llvm::Value *PrintLineFeed();

    llvm::Value *CodeGenerationInternalError(std::string error_msg);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::map<std::string, llvm::Value *> global_constants_table;
};

#endif // CODE_GENERATION_VISITOR_H_
