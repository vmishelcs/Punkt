#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <map>

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <semantic_analyzer/types/base_type.h>

class CodeGenerationVisitor : public ParseNodeIRVisitor {
public:
    CodeGenerationVisitor(std::string module_id);

    void WriteIRToFD(int fd);

    virtual llvm::Value *GenerateCode(AssignmentStatementNode& node) override;
    virtual llvm::Value *GenerateCode(CallStatementNode& node) override;
    virtual llvm::Value *GenerateCode(CodeBlockNode& node) override;
    virtual llvm::Value *GenerateCode(DeclarationStatementNode& node) override;
    virtual llvm::Value *GenerateCode(ForStatementNode& node) override;
    virtual llvm::Value *GenerateCode(FunctionDefinitionNode& node) override;
    virtual llvm::Value *GenerateCode(IfStatementNode& node) override;
    virtual llvm::Value *GenerateCode(LambdaInvocationNode& node) override;
    virtual llvm::Value *GenerateCode(LambdaNode& node) override;
    virtual llvm::Value *GenerateCode(LambdaParameterNode& node) override;
    virtual llvm::Value *GenerateCode(LambdaTypeNode& node) override;
    virtual llvm::Value *GenerateCode(MainNode& node) override;
    virtual llvm::Value *GenerateCode(OperatorNode& node) override;
    virtual llvm::Value *GenerateCode(PrintStatementNode& node) override;
    virtual llvm::Value *GenerateCode(ProgramNode& node) override;
    virtual llvm::Value *GenerateCode(ReturnStatementNode& node) override;
    
    virtual llvm::Value *GenerateCode(ErrorNode& node) override;
    virtual llvm::Value *GenerateCode(NopNode& node) override;
    virtual llvm::Value *GenerateCode(IdentifierNode& node) override;
    virtual llvm::Value *GenerateCode(BooleanLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(CharacterLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(IntegerLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(StringLiteralNode& node) override;
    virtual llvm::Value *GenerateCode(BaseTypeNode& node) override;

private:
    void GenerateGlobalConstants();

    bool WasPreviousInstructionBlockTerminator() {
        llvm::Instruction *last_instruction = builder->GetInsertBlock()->getTerminator();
        if (last_instruction && last_instruction->isTerminator()) {
            return true;
        }
        return false;
    }

    llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *function,
            const std::string& identifier_name, llvm::Type *llvm_type);

    void GeneratePrintfDeclaration();
    void GeneratePrintfFmtStringsForBaseTypes();
    llvm::Value *GenerateFmtStringForBaseType(BaseTypeEnum base_type_enum, std::string fmt_str);

    llvm::Value *GetPrintfFmtString(Type *type);
    llvm::Value *GetPrintfFmtStringForBaseType(BaseTypeEnum base_type_enum);

    llvm::Value *PrintValue(llvm::Value *value, Type *type);
    llvm::Value *PrintLineFeed();

    llvm::Value *CodeGenerationInternalError(std::string error_msg);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::map<std::string, llvm::Value *> global_constants_table;
};

#endif // CODE_GENERATION_VISITOR_H_
