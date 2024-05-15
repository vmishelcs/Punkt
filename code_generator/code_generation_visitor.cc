#include "code_generation_visitor.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <logging/punkt_logger.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/type.h>
#include <token/punctuator_token.h>

#include <memory>
#include <string>

#include "codegen_context.h"

// Array helper constants
static const std::string kPunktArrayStructName = "struct.PunktArray";
static const std::string kAllocPunktArrayFunctionName = "function.alloc_array";
static const std::string kDeallocPunktArrayFunctionName =
    "function.dealloc_array";

// Function names
static const std::string kMainFunctionName = "main";
static const std::string kMallocFunctionName = "malloc";
static const std::string kFreeFunctionName = "free";
static const std::string kMemsetFunctionName = "memset";
static const std::string kPrintfFunctionName = "printf";
static const std::string kExitFunctionName = "exit";

// Internal constants
static const std::string kCharFmtString = "%c";
static const std::string kIntFmtString = "%d";
static const std::string kStrFmtString = "%s";
static const char kLineFeedChar = '\n';
static const char kOpenBracketChar = '[';
static const char kCommaChar = ',';
static const char kSpaceChar = ' ';
static const char kCloseBracketChar = ']';

using codegen_function_type = llvm::Value *(*)(CodeGenerationVisitor &,
                                               OperatorNode &);

/// @brief Log fatal internal code generation error.
/// @param message String to be output before terminating the compilation
/// process.
/// @return Always `nullptr`.
[[noreturn]] static llvm::Value *CodeGenerationInternalError(
    const std::string &);

CodeGenerationVisitor::CodeGenerationVisitor(std::string module_id)
    : codegen_context{nullptr} {
  CodegenContext::Initialize(module_id);
  codegen_context = CodegenContext::Get();
}

void CodeGenerationVisitor::WriteIRToFD(int fd) {
  llvm::raw_fd_ostream ir_ostream(fd, /*shouldClose=*/false);
  llvm::Module *module = codegen_context->GetModule();
  module->print(ir_ostream, nullptr);
}

// TODO: Stop using ParseNode::GetLLVMType and use Type::GetLLVMType instead.

llvm::Value *CodeGenerationVisitor::GenerateCode(ArrayTypeNode &node) {
  // TODO
  return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CallStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  return node.GetLambdaInvocationNode()->GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode &node) {
  for (auto child : node.GetChildren()) {
    child->GenerateCode(*this);
  }

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  // GenerateCode(CodeBlockNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(
    ExpressionStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  node.GetExpressionNode()->GenerateCode(*this);

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  // GenerateCode(ExpressionStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(
    DeclarationStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  IdentifierNode *identifier_node = node.GetIdentifierNode();

  // Generate code for initializer value.
  llvm::Value *initializer_value = node.GetInitializer()->GenerateCode(*this);

  SymbolTableEntry *sym_table_entry = identifier_node->GetSymbolTableEntry();
  if (!sym_table_entry) {
    CodeGenerationInternalError(
        "no symbol table entry found for identifier node.");
  }

  auto parent_function = builder->GetInsertBlock()->getParent();
  // Allocate stack memory for variables in the entry block of the function.
  llvm::AllocaInst *alloca_inst =
      CreateEntryBlockAlloca(parent_function, identifier_node->GetName(),
                             initializer_value->getType());

  // Store alloca instruction in the symbol table to be used for loads.
  sym_table_entry->alloca = alloca_inst;

  builder->CreateStore(initializer_value, alloca_inst);

  return initializer_value;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ForStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Emit code for the loop initializer.
  node.GetLoopInitializerNode()->GenerateCode(*this);

  auto parent_function = builder->GetInsertBlock()->getParent();

  // Create a basic block to check if the loop condition is true/false and
  // attach to the parent function.
  auto condition_block =
      llvm::BasicBlock::Create(*llvm_context, "condcheck", parent_function);

  // Create a basic block for the loop body.
  auto loop_block = llvm::BasicBlock::Create(*llvm_context, "loop");

  // Create a basic block for the loop exit.
  auto afterloop_block = llvm::BasicBlock::Create(*llvm_context, "afterloop");

  // Insert an explicit fall-through from the current block (before the loop)
  // to the loop condition block.
  builder->CreateBr(condition_block);

  // Now we are inserting instructions into the loop condition block.
  builder->SetInsertPoint(condition_block);

  // Check if the condition to enter the loop is satisfied.
  llvm::Value *end_condition = node.GetEndConditionNode()->GenerateCode(*this);
  if (!end_condition) {
    return CodeGenerationInternalError(
        "failed generating end condition for for-statement");
  }
  end_condition = builder->CreateTrunc(
      end_condition, llvm::Type::getInt1Ty(*llvm_context), "trunctmp");
  builder->CreateCondBr(end_condition, loop_block, afterloop_block);

  // Append the 'loop' block after the 'condition' block.
  parent_function->insert(parent_function->end(), loop_block);

  // Now we are inserting instructions into the loop block.
  builder->SetInsertPoint(loop_block);

  // Emit loop body.
  node.GetLoopBodyNode()->GenerateCode(*this);

  // Get an updated pointer to the loop_block.
  loop_block = builder->GetInsertBlock();

  // Check if the loop block ends in a block terminator.
  llvm::Instruction *loop_block_last_inst = loop_block->getTerminator();
  if (!loop_block_last_inst || !loop_block_last_inst->isTerminator()) {
    // Emit code for the step value, appending it to the end of the loop
    // body.
    node.GetStepValueNode()->GenerateCode(*this);

    // Create an unconditional branch to the start of the loop, where we
    // check if the condition to continue looping is true/false.
    builder->CreateBr(condition_block);
  }

  // Append the 'afterloop' block after the 'loop' block. We jump to this block
  // if the loop condition fails.
  parent_function->insert(parent_function->end(), afterloop_block);

  // Any new code will be inserted in the 'afterloop' block.
  builder->SetInsertPoint(afterloop_block);

  // GenerateCode(ForStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(FunctionDefinitionNode &node) {
  // We merely call on the lambda node to generate its code. We store the
  // function's address at the identifier's symbol table entry in the
  // `GenerateCode(LambdaNode&)` method.
  node.GetLambdaNode()->GenerateCode(*this);

  // GenerateCode(FunctionDefinitionNode&) return value is not used.
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IfStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *condition = node.GetIfConditionNode()->GenerateCode(*this);
  if (!condition) {
    return CodeGenerationInternalError(
        "failed generating condition for if-statement");
  }
  // Truncate condition to make sure it has LLVM type `i1`.
  condition = builder->CreateTrunc(
      condition, llvm::Type::getInt1Ty(*llvm_context), "trunctmp");

  auto parent_function = builder->GetInsertBlock()->getParent();

  // Create blocks for 'then' and 'else' cases, and the 'merge' block where the
  // control merges. Note that we do not require the programmer to provide an
  // 'else' clause, hence we leave it as null for now to be initialized later.
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(*llvm_context, "then", parent_function);
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *merge_block =
      llvm::BasicBlock::Create(*llvm_context, "ifcont");

  if (node.HasElseBlock()) {
    // This is where we initialize 'else' if it was provided.
    else_block = llvm::BasicBlock::Create(*llvm_context, "else");
    builder->CreateCondBr(condition, then_block, else_block);
  } else {
    builder->CreateCondBr(condition, then_block, merge_block);
  }

  // Emit 'then' block.
  builder->SetInsertPoint(then_block);
  node.GetIfBlockNode()->GenerateCode(*this);

  // Get an updated pointer to 'then' block.
  then_block = builder->GetInsertBlock();

  // Check if 'then' block ends in a block terminator.
  llvm::Instruction *then_block_last_inst = then_block->getTerminator();
  if (!then_block_last_inst || !then_block_last_inst->isTerminator()) {
    // Insert a break statement to merge control flow if the 'then' block has
    // no block terminator.
    builder->CreateBr(merge_block);
  }

  if (node.HasElseBlock()) {
    // Emit 'else' block.
    parent_function->insert(parent_function->end(), else_block);
    builder->SetInsertPoint(else_block);
    node.GetElseBlockNode()->GenerateCode(*this);

    // Get an updated pointer to 'else' block.
    else_block = builder->GetInsertBlock();

    // Check if 'else' block ends in a block terminator.
    llvm::Instruction *else_block_last_inst = else_block->getTerminator();
    if (!else_block_last_inst || !else_block_last_inst->isTerminator()) {
      // Insert a break statement to merge control flow if the 'else' block
      // has no block terminator.
      builder->CreateBr(merge_block);
    }
  }

  // Emit 'merge' block.
  parent_function->insert(parent_function->end(), merge_block);
  builder->SetInsertPoint(merge_block);

  // GenerateCode(IfStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaInvocationNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  std::vector<llvm::Value *> arg_values;
  std::vector<llvm::Type *> arg_types;
  for (const auto &arg : node.GetArgumentNodes()) {
    arg_values.push_back(arg->GenerateCode(*this));
    arg_types.push_back(arg->GetType()->GetLLVMType(*llvm_context));
  }

  Type *return_type = node.GetType();

  // Check if invocation returns void.
  bool returns_void = false;
  auto return_base_type = dynamic_cast<BaseType *>(return_type);
  if (return_base_type) {
    returns_void = return_base_type->IsEquivalentTo(BaseTypeEnum::VOID);
  }

  llvm::Value *callee_value = node.GetCalleeNode()->GenerateCode(*this);

  // If we are provided an llvm::Function object directly, just call it.
  if (auto function = llvm::dyn_cast<llvm::Function>(callee_value)) {
    if (returns_void) {
      // Void instructions cannot have a name.
      builder->CreateCall(function, arg_values);
      // Return null value for void functions.
      return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
    }

    return builder->CreateCall(function, arg_values, "calltmp");
  }

  // In order to call a function using its pointer, we need to construct the
  // function type and pass it to IRBuilder::CreateCall.
  auto function_type =
      llvm::FunctionType::get(node.GetType()->GetLLVMType(*llvm_context),
                              arg_types, /*isVarArg=*/false);

  if (returns_void) {
    // Void instructions cannot have a name.
    builder->CreateCall(function_type, callee_value, arg_values);
    // Return null value for void functions.
    return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
  }

  return builder->CreateCall(function_type, callee_value, arg_values,
                             "calltmp");
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  std::vector<llvm::Type *> param_types;
  std::vector<LambdaParameterNode *> param_nodes = node.GetParameterNodes();
  param_types.reserve(param_nodes.size());

  // Create a vector holding llvm::Type pointers describing the parameter
  // types.
  for (const auto &param_node : param_nodes) {
    param_types.push_back(param_node->GetType()->GetLLVMType(*llvm_context));
  }

  // Create the function type.
  auto function_type = llvm::FunctionType::get(
      node.GetReturnTypeNode()->GetType()->GetLLVMType(*llvm_context),
      param_types,
      /*isVarArg=*/false);

  // Functions have external linkage, lambdas have private linkage.
  llvm::GlobalValue::LinkageTypes linkage_type =
      node.IsFunction() ? llvm::GlobalValue::LinkageTypes::ExternalLinkage
                        : llvm::GlobalValue::LinkageTypes::PrivateLinkage;

  // Create the function.
  auto function = llvm::Function::Create(function_type, linkage_type,
                                         "function.user_function", module);

  // Set names for arguments.
  unsigned i = 0;
  for (auto &arg : function->args()) {
    arg.setName(param_nodes[i]->GetIdentifierNode()->GetName());
    ++i;
  }

  // Check if this lambda is anonymous.
  if (!node.IsAnonymous()) {
    // Locate the identifier node.
    ParseNode *parent = node.GetParent();
    auto identifier_node = static_cast<IdentifierNode *>(parent->GetChild(0));

    // Store llvm::Function pointer in the symbol table entry.
    SymbolTableEntry *sym_table_entry = identifier_node->GetSymbolTableEntry();
    if (dynamic_cast<FunctionDefinitionNode *>(parent)) {
      sym_table_entry->function = function;
    }
  }

  // In case we are generating lambda variable code, save the current insert
  // block.
  llvm::BasicBlock *builder_insert_point_save = builder->GetInsertBlock();

  // Set up builder to generate function body.
  auto function_entry_block =
      llvm::BasicBlock::Create(*llvm_context, "entry", function);
  builder->SetInsertPoint(function_entry_block);

  // Allocate memory for arguments.
  i = 0;
  for (auto &arg : function->args()) {
    IdentifierNode *arg_identifier_node = param_nodes[i]->GetIdentifierNode();

    // Create an alloca for this parameter.
    llvm::AllocaInst *alloca = CreateEntryBlockAlloca(
        function, arg_identifier_node->GetName(), arg.getType());

    // Store alloca instruction in the symbol table.
    arg_identifier_node->SetLLVMAlloca(alloca);

    // Store initial value into the alloca.
    builder->CreateStore(&arg, alloca);

    ++i;
  }

  // Generate code for the function body.
  node.GetLambdaBodyNode()->GenerateCode(*this);

  // Get the final basic block of the function.
  llvm::BasicBlock *last_bb = builder->GetInsertBlock();

  // Check if the last instruction of the block is a terminator.
  llvm::Instruction *last_instruction = last_bb->getTerminator();
  if (!last_instruction || !last_instruction->isTerminator()) {
    // If there is no last instruction, or if the last instruction is not a
    // block terminator, we need to insert a terminator here.
    BaseType *return_base_type =
        dynamic_cast<BaseType *>(node.GetReturnTypeNode()->GetType());
    if (return_base_type &&
        return_base_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
      // For void lambdas, place an implicit return statement at the end of
      // the lambda body.
      builder->CreateRetVoid();
    } else {
      // Otherwise, issue a runtime error.
      GenerateRuntimeErrorWithMessage("non-void lambda with no return value");
    }
  }

  // Validate generated code.
  if (llvm::verifyFunction(*function, &llvm::errs())) {
    // CodeGenerationInternalError("generated IR is invalid.");
  }

  // Restore builder insert point after generating lambda.
  builder->SetInsertPoint(builder_insert_point_save);

  return function;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaParameterNode &node) {
  // GenerateCode(LambdaParameterNode&) is not used.
  return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaTypeNode &node) {
  // GenerateCode(LambdaTypeNode&) is not used.
  return nullptr;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(MainNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Main always returns int (always returns 0).
  llvm::Type *return_type = llvm::Type::getInt32Ty(*llvm_context);

  // Main does not take any arguments.
  llvm::FunctionType *function_type =
      llvm::FunctionType::get(return_type, /*isVarArgs=*/false);

  // Create an function object for main.
  llvm::Function *main_func =
      llvm::Function::Create(function_type, llvm::Function::ExternalLinkage,
                             kMainFunctionName, *module);

  llvm::BasicBlock *entry_block =
      llvm::BasicBlock::Create(*llvm_context, "entry", main_func);
  builder->SetInsertPoint(entry_block);

  // Generate main code block.
  node.GetChild(0)->GenerateCode(*this);

  llvm::BasicBlock *last_main_bb = builder->GetInsertBlock();
  // Check if the last instruction of the block is a terminator.
  llvm::Instruction *last_instruction = last_main_bb->getTerminator();
  if (!last_instruction || !last_instruction->isTerminator()) {
    // Main always returns 0.
    builder->CreateRet(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0));
  }

  if (llvm::verifyFunction(*main_func, &llvm::errs())) {
    CodeGenerationInternalError("generated IR is invalid.");
  }

  // GenerateCode(MainNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(OperatorNode &node) {
  codegen_function_type codegen_function = node.GetCodegenFunction();
  if (!codegen_function) {
    CodeGenerationInternalError("operator codegen function not set");
  }

  return codegen_function(*this, node);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(PrintStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::LLVMContext *context = codegen_context->GetLLVMContext();

  // We call printf for each 'operand'.
  for (auto child : node.GetChildren()) {
    PrintValue(child->GetType(), child->GenerateCode(*this));
  }

  if (node.IsPrintln()) {
    // Print line feed if we are dealing with a `println` statement.
    PrintLineFeed();
  }

  // GenerateCode(PrintStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode &node) {
  // Generate some helpful C standard library function declarations.
  GenerateMallocFunctionDeclaration();
  GenerateFreeFunctionDeclaration();
  GenerateMemsetFunctionDeclaration();
  GeneratePrintfFunctionDeclaration();
  GenerateExitFunctionDeclaration();

  // Define Punkt array struct type.
  GeneratePunktArrayType();

  GenerateAllocPunktArrayFunction();
  GenerateDeallocPunktArrayFunction();

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Generate code for each function of the program.
  for (const auto &child : node.GetChildren()) {
    child->GenerateCode(*this);
  }

  if (llvm::verifyModule(*module, &llvm::errs())) {
    // CodeGenerationInternalError("generated IR is invalid.");
  }

  // GenerateCode(ProgramNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ReturnStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  ParseNode *enclosing_function = node.GetEnclosingFunctionNode();

  if (enclosing_function->GetParseNodeType() == ParseNodeType::MAIN_NODE) {
    // We always return 0 from main.
    builder->CreateRet(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0));
  } else if (!node.GetReturnValueNode()) {
    // Return void if no return value is specified.
    builder->CreateRetVoid();
  } else {
    llvm::Value *return_value = node.GetReturnValueNode()->GenerateCode(*this);
    builder->CreateRet(return_value);
  }

  // GenerateCode(ReturnStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(WhileStatementNode &node) {
  if (IsPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();

  // Create a basic block to check if the while condition is true/false and
  // attach it to the parent function.
  auto condition_block =
      llvm::BasicBlock::Create(*llvm_context, "condcheck", parent_function);

  // Create a basic block for the while loop body.
  auto loop_block = llvm::BasicBlock::Create(*llvm_context, "loop");

  // Create a basic block for the loop exit.
  auto afterloop_block = llvm::BasicBlock::Create(*llvm_context, "afterloop");

  // Insert an explicit fall-through from the current block (before the
  // loop) to the loop condition block.
  builder->CreateBr(condition_block);

  // Now we are inserting instructions into the loop condition block.
  builder->SetInsertPoint(condition_block);

  // Generate code for the while loop condition.
  llvm::Value *condition = node.GetConditionNode()->GenerateCode(*this);
  if (!condition) {
    CodeGenerationInternalError(
        "failed generating condition for while-statement");
  }
  // Truncate condition to make sure it has type `i1`.
  condition = builder->CreateTrunc(
      condition, llvm::Type::getInt1Ty(*llvm_context), "trunctmp");
  // Create a conditional jump based on the while loop condition.
  builder->CreateCondBr(condition, loop_block, afterloop_block);

  // Append the loop block after the condition block.
  parent_function->insert(parent_function->end(), loop_block);

  // Now we are inserting instructions into the loop block.
  builder->SetInsertPoint(loop_block);

  // Emit loop body.
  node.GetLoopBodyNode()->GenerateCode(*this);

  // Get an updated pointer to the loop_block.
  loop_block = builder->GetInsertBlock();

  // Check if the loop block ends in a block terminator.
  llvm::Instruction *loop_block_last_inst = loop_block->getTerminator();
  if (!loop_block_last_inst || !loop_block_last_inst->isTerminator()) {
    // Create an unconditional branch to the start of the loop, where we
    // check the while condition.
    builder->CreateBr(condition_block);
  }

  // Append the afterloop block. We jump to this block if the while
  // condition is false.
  parent_function->insert(parent_function->end(), afterloop_block);

  // Any subsequent code is inserted in the afterloop block.
  builder->SetInsertPoint(afterloop_block);

  // GenerateCode(WhileStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*llvm_context));
}

/******************************************************************************
 *                      Code generation for identifiers *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode &node) {
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Look up the symbol table entry for this identifier.
  SymbolTableEntry *sym_table_entry = node.GetSymbolTableEntry();
  if (!sym_table_entry) {
    CodeGenerationInternalError("missing entry in symbol table for " +
                                node.ToString());
  }

  if (sym_table_entry->symbol_type == SymbolType::FUNCTION) {
    return sym_table_entry->function;
  }

  llvm::AllocaInst *alloca_inst = sym_table_entry->alloca;
  if (!alloca_inst) {
    CodeGenerationInternalError("unable to find alloca for " + node.ToString());
  }

  // Return llvm::AllocaInst pointer if we are targetting this identifier.
  if (node.IsAssignmentTarget()) {
    return alloca_inst;
  }

  // Return identifier value otherwise.
  return builder->CreateLoad(alloca_inst->getAllocatedType(), alloca_inst,
                             node.GetName());
}

/******************************************************************************
 *                        Code generation for literals *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(BooleanLiteralNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();

  return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context),
                                (int)node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CharacterLiteralNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();

  return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context),
                                node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();

  return llvm::ConstantInt::getSigned(llvm::Type::getInt64Ty(*llvm_context),
                                      node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(StringLiteralNode &node) {
  return GetOrCreateString(node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(BaseTypeNode &node) {
  // GenerateCode(BaseTypeNode&) is not used.
  return nullptr;
}

/******************************************************************************
 *                            NOP code generation *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(NopNode &node) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  return builder->CreateAdd(
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), 0),
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), 0), "nop");
}

/******************************************************************************
 *              Helpful C standard library function declarations *
 ******************************************************************************/
void CodeGenerationVisitor::GenerateMallocFunctionDeclaration() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Create a function type returning a pointer type, taking an integer
  // argument.
  std::vector<llvm::Type *> parameters = {
      llvm::Type::getInt64Ty(*llvm_context)};
  llvm::FunctionType *malloc_func_type =
      llvm::FunctionType::get(llvm::PointerType::getUnqual(*llvm_context),
                              parameters, /* IsVarArg=*/false);

  // Create the function declaration
  llvm::Function *malloc_f =
      llvm::Function::Create(malloc_func_type, llvm::Function::ExternalLinkage,
                             kMallocFunctionName, *module);
  if (!malloc_f) {
    CodeGenerationInternalError("could not generate declaration for malloc");
  }
}

void CodeGenerationVisitor::GenerateFreeFunctionDeclaration() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Create a void function taking a pointer argument.
  std::vector<llvm::Type *> parameters = {
      llvm::PointerType::getUnqual(*llvm_context)};
  llvm::FunctionType *free_func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(*llvm_context), parameters, /* IsVarArg=*/false);

  // Create the function declaration
  llvm::Function *free_f =
      llvm::Function::Create(free_func_type, llvm::Function::ExternalLinkage,
                             kFreeFunctionName, *module);
  if (!free_f) {
    CodeGenerationInternalError("could not generate declaration for free");
  }
}

void CodeGenerationVisitor::GenerateMemsetFunctionDeclaration() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Create a void function taking a pointer, a char, and a 64-bit integer
  // as arguments.
  std::vector<llvm::Type *> parameters = {
      llvm::PointerType::getUnqual(*llvm_context),
      llvm::Type::getInt8Ty(*llvm_context),
      llvm::Type::getInt64Ty(*llvm_context)};
  llvm::FunctionType *memset_func_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(*llvm_context), parameters, /* IsVarArg=*/false);

  // Create the function declaration
  llvm::Function *memset_f =
      llvm::Function::Create(memset_func_type, llvm::Function::ExternalLinkage,
                             kMemsetFunctionName, *module);
  if (!memset_f) {
    CodeGenerationInternalError("could not generate declaration for memset");
  }
}

void CodeGenerationVisitor::GeneratePrintfFunctionDeclaration() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Create a function type returning a 32-bit int, taking 1 parameter and a
  // variable number of arguments.
  std::vector<llvm::Type *> parameters = {
      llvm::PointerType::getUnqual(*llvm_context)};
  llvm::FunctionType *printf_func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(*llvm_context), parameters, /*isVarArg=*/true);

  // Create the function declaration
  llvm::Function *printf_f =
      llvm::Function::Create(printf_func_type, llvm::Function::ExternalLinkage,
                             kPrintfFunctionName, *module);
  if (!printf_f) {
    CodeGenerationInternalError("could not generate declaration for printf");
  }
}

void CodeGenerationVisitor::GenerateExitFunctionDeclaration() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();

  // Create a void function taking an integer argument.
  std::vector<llvm::Type *> parameters = {
      llvm::Type::getInt32Ty(*llvm_context)};
  llvm::FunctionType *exit_f_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(*llvm_context), parameters, /*isVarArg=*/false);

  // Create the function declaration
  llvm::Function *exit_f = llvm::Function::Create(
      exit_f_type, llvm::Function::ExternalLinkage, kExitFunctionName, *module);
  if (!exit_f) {
    CodeGenerationInternalError("could not generate declaration for printf");
  }
}

/******************************************************************************
 *                              Printing helpers *
 ******************************************************************************/
void CodeGenerationVisitor::PrintValue(Type *type, llvm::Value *value) {
  if (auto base_type = dynamic_cast<BaseType *>(type)) {
    PrintBaseTypeValue(base_type, value);
  } else if (auto array_type = dynamic_cast<ArrayType *>(type)) {
    PrintArrayTypeValue(array_type, value);
  } else if (auto lambda_type = dynamic_cast<LambdaType *>(type)) {
    PrintLambdaTypeValue(lambda_type);
  } else {
    CodeGenerationInternalError("unimplemented type in print statement");
  }
}

void CodeGenerationVisitor::PrintBaseTypeValue(BaseType *base_type,
                                               llvm::Value *value) {
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Function *printf_func = module->getFunction(kPrintfFunctionName);
  if (!printf_func) {
    CodeGenerationInternalError(
        "unable to obtain llvm::Function pointer for printf");
  }

  // Vector for holding arguments for `printf`.
  std::vector<llvm::Value *> printf_args;

  printf_args.push_back(GetPrintfFormatStringForBaseType(base_type));
  if (printf_args.back() == nullptr) {
    CodeGenerationInternalError(
        "failed obtaining llvm::Value object for printf format string");
  }

  llvm::Value *print_value = value;
  if (base_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
    // When printing booleans, we first truncate to 1-bit such that only the
    // least significant bit remains. Then we extend to 32 bits and print
    // the boolean as an integer.
    auto truncated_value = builder->CreateTrunc(
        print_value, llvm::Type::getInt1Ty(*context), "trunctmp");
    print_value = builder->CreateZExt(
        truncated_value, llvm::Type::getInt32Ty(*context), "zexttmp");
  } else if (base_type->IsEquivalentTo(BaseTypeEnum::CHARACTER)) {
    // When printing characters, we sign-extend to 32 bits.
    print_value = builder->CreateSExt(
        print_value, llvm::Type::getInt32Ty(*context), "sexttmp");
  }

  printf_args.push_back(print_value);
  if (printf_args.back() == nullptr) {
    CodeGenerationInternalError("failed to generate argument for printf");
  }

  builder->CreateCall(printf_func, printf_args, "printf_ret");
}

void CodeGenerationVisitor::PrintArrayTypeValue(ArrayType *array_type,
                                                llvm::Value *PunktArray_ptr) {
  // The array printing algorithm is described by the following pseudo-IR.
  //
  // array_printloop_before:
  //   ...
  //   ; Print '[' character
  //   %PunktArray_size_ptr = gep PunktArray_ptr, 0
  //   %PunktArray_size = load %PunktArray_size_ptr
  //   %cmptmp = cmp eq %PunktArray_size, 0
  //   br %cmptmp, label %array_printloop_end, label %array_printloop_start
  //
  // array_printloop_start:
  //   %PunktArray_loop_size = sub %PunktArray_size, 1
  //   br label %array_printloop_cond
  //
  // array_printloop_cond:
  //   %arr_idx = phi [0, %array_printloop_start],
  //                  [%next_arr_idx, %array_printloop_main]
  //   ; Load Punkt array data to be available for %array_printloop_main and
  //   ; %array_printloop_last.
  //   %cmptmp = cmp ult %arr_idx, %PunktArray_loop_size
  //   br %cmptmp, label %array_printloop_main, label %array_printloop_last
  //
  // array_printloop_main:
  //   ; Print value at index %arr_idx
  //   ; Print ',' character
  //   ; Print ' ' character
  //   %next_arr_idx = add %arr_idx, 1
  //   br label %array_printloop_cond
  //
  // array_printloop_last:
  //  ; Print last stored value of the array
  //  br label %array_printloop_end
  //
  // array_printloop_end:
  //  ; Print ']' character
  //  ...

  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();
  llvm::StructType *PunktArray_struct =
      llvm::StructType::getTypeByName(*llvm_context, kPunktArrayStructName);

  // Create basic required basic blocks.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *printloop_start =
      llvm::BasicBlock::Create(*llvm_context, "array_printloop_start");
  llvm::BasicBlock *printloop_cond =
      llvm::BasicBlock::Create(*llvm_context, "array_printloop_cond");
  llvm::BasicBlock *printloop_main =
      llvm::BasicBlock::Create(*llvm_context, "array_printloop_main");
  llvm::BasicBlock *printloop_last =
      llvm::BasicBlock::Create(*llvm_context, "array_printloop_last");
  llvm::BasicBlock *printloop_end =
      llvm::BasicBlock::Create(*llvm_context, "array_printloop_end");

  // array_printloop_before:
  // Print '['.
  auto char_base_type_tmp = BaseType::Create(BaseTypeEnum::CHARACTER);
  llvm::Value *open_bracket_char_val = llvm::ConstantInt::get(
      llvm::Type::getInt8Ty(*llvm_context), kOpenBracketChar);
  PrintBaseTypeValue(char_base_type_tmp.get(), open_bracket_char_val);

  // Get array size.
  llvm::Value *PunktArray_size = builder->CreateLoad(
      llvm::Type::getInt64Ty(*llvm_context), PunktArray_ptr, "PunktArray_size");

  // Compare `PunktArray_size == 0`.
  llvm::Value *size_check = builder->CreateICmpEQ(
      PunktArray_size,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0),
      "cmptmp");

  // If `PunktArray_size == 0`, go to `printloop_end` block. Otherwise, go
  // to `array_printloop_start`.
  builder->CreateCondBr(size_check, printloop_end, printloop_start);

  // array_printloop_start:
  // Attach `array_printloop_start` to the end of the parent function.
  parent_function->insert(parent_function->end(), printloop_start);

  // Begin inserting in `array_printloop_start`.
  builder->SetInsertPoint(printloop_start);

  // Here, `PunktArray_loop_size` denotes the number of iterations of
  // `array_printloop_main` where we print the array element followed by a
  // comma and space.
  llvm::Value *PunktArray_loop_size = builder->CreateSub(
      PunktArray_size,
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 1),
      "PunktArray_loop_size");

  // Create an unconditional branch to `array_printloop_cond`.
  builder->CreateBr(printloop_cond);

  // array_printloop_cond:
  // Attach `array_printloop_cond` to the end of the parent function.
  parent_function->insert(parent_function->end(), printloop_cond);

  // Begin inserting in `printloop_cond`.
  builder->SetInsertPoint(printloop_cond);

  // Create PHI node with an entry for `array_printloop_start`.
  llvm::PHINode *arr_idx =
      builder->CreatePHI(llvm::Type::getInt64Ty(*llvm_context), 2, "arr_idx");
  arr_idx->addIncoming(
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0),
      printloop_start);

  // Load Punkt array data to be available for both `array_printloop_main`
  // and `array_printloop_last`.
  llvm::Value *PunktArray_data_ptr = builder->CreateGEP(
      PunktArray_struct, PunktArray_ptr,
      {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1)},
      "PunktArray_data_ptr");
  llvm::Value *PunktArray_data =
      builder->CreateLoad(llvm::PointerType::getUnqual(*llvm_context),
                          PunktArray_data_ptr, "PunktArray_data");

  // Compute the `array_printloop_main` end condition.
  llvm::Value *end_cond =
      builder->CreateICmpULT(arr_idx, PunktArray_loop_size, "cmptmp");
  builder->CreateCondBr(end_cond, printloop_main, printloop_last);

  // array_printloop_main:
  // Attach the `array_printloop_main` basic block to the end of the parent
  // function.
  parent_function->insert(parent_function->end(), printloop_main);

  // Begin inserting in `printloop`.
  builder->SetInsertPoint(printloop_main);

  // Print arr[arr_idx] value.
  Type *subtype = array_type->GetSubtype();
  llvm::Type *llvm_subtype = subtype->GetLLVMType(*llvm_context);
  llvm::Value *elem_addr =
      builder->CreateGEP(llvm_subtype, PunktArray_data, {arr_idx}, "elem_addr");
  llvm::Value *elem = builder->CreateLoad(llvm_subtype, elem_addr, "elem");
  PrintValue(subtype, elem);

  // Print ','.
  llvm::Value *comma_char_val =
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), kCommaChar);
  PrintBaseTypeValue(char_base_type_tmp.get(), comma_char_val);

  // Print ' '.
  llvm::Value *space_char_val =
      llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), kSpaceChar);
  PrintBaseTypeValue(char_base_type_tmp.get(), space_char_val);

  // Increment the array index.
  llvm::Value *next_arr_idx = builder->CreateAdd(
      arr_idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 1),
      "next_arr_idx");

  // Add new entry to the PHI node for `next_arr_idx`.
  printloop_main = builder->GetInsertBlock();
  arr_idx->addIncoming(next_arr_idx, printloop_main);

  // Create an unconditional branch to go back to `printloop_cond`.
  builder->CreateBr(printloop_cond);

  // array_printloop_last:
  // Attach `array_printloop_last` to the end of the parent function.
  parent_function->insert(parent_function->end(), printloop_last);

  // Begin inserting in `array_printloop_last`. This basic block is to print
  // the last stored value of the array.
  builder->SetInsertPoint(printloop_last);

  // Print last element of the array.
  elem_addr =
      builder->CreateGEP(llvm_subtype, PunktArray_data, {arr_idx}, "elem_addr");
  elem = builder->CreateLoad(llvm_subtype, elem_addr, "elem");
  PrintValue(subtype, elem);

  // Create an unconditional branch to `printloop_end`.
  builder->CreateBr(printloop_end);

  // printloop_end:
  // Append `printloop_end` basic block to the end of the parent function.
  parent_function->insert(parent_function->end(), printloop_end);

  // Any new code will be inserted after `printloop`.
  builder->SetInsertPoint(printloop_end);

  llvm::Value *close_bracket_char_val = llvm::ConstantInt::get(
      llvm::Type::getInt8Ty(*llvm_context), kCloseBracketChar);
  PrintBaseTypeValue(char_base_type_tmp.get(), close_bracket_char_val);
}

void CodeGenerationVisitor::PrintLambdaTypeValue(LambdaType *lambda_type) {
  std::unique_ptr<BaseType> basetype_tmp =
      BaseType::Create(BaseTypeEnum::STRING);
  llvm::Value *lambda_type_string_value =
      GetOrCreateString(lambda_type->ToString());

  PrintBaseTypeValue(basetype_tmp.get(), lambda_type_string_value);
}

void CodeGenerationVisitor::PrintLineFeed() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();

  std::unique_ptr<BaseType> basetype_tmp =
      BaseType::Create(BaseTypeEnum::CHARACTER);
  llvm::Value *line_feed_char_value = llvm::ConstantInt::get(
      llvm::Type::getInt8Ty(*llvm_context), kLineFeedChar);

  PrintBaseTypeValue(basetype_tmp.get(), line_feed_char_value);
}

llvm::Value *CodeGenerationVisitor::GetPrintfFormatStringForBaseType(
    BaseType *base_type) {
  BaseTypeEnum base_type_enum = base_type->GetBaseTypeEnum();
  switch (base_type_enum) {
    case BaseTypeEnum::BOOLEAN:
      return GetOrCreateString("%d");
    case BaseTypeEnum::CHARACTER:
      return GetOrCreateString("%c");
    case BaseTypeEnum::INTEGER:
      return GetOrCreateString("%lld");
    case BaseTypeEnum::STRING:
      return GetOrCreateString("%s");

    case BaseTypeEnum::VOID:
    case BaseTypeEnum::ERROR:
    default:
      CodeGenerationInternalError(
          "invalid BaseTypeEnum in "
          "CodeGenerationVisitor::GetPrintfFormatStringForBaseType");
  }
}

/******************************************************************************
 *                           Miscellaneous helpers *
 ******************************************************************************/
void CodeGenerationVisitor::GenerateRuntimeErrorWithMessage(
    const std::string &message) {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Print error message.
  auto string_base_type_tmp = BaseType::CreateStringType();
  PrintBaseTypeValue(string_base_type_tmp.get(),
                     GetOrCreateString("runtime error: " + message + "\n"));

  // Call exit with argument -1.
  llvm::Function *exit_f = module->getFunction(kExitFunctionName);
  builder->CreateCall(exit_f, {llvm::ConstantInt::get(
                                  llvm::Type::getInt32Ty(*llvm_context), -1)});
  builder->CreateUnreachable();
}

llvm::AllocaInst *CodeGenerationVisitor::CreateEntryBlockAlloca(
    llvm::Function *function, const std::string &identifier,
    llvm::Type *llvm_type) {
  llvm::IRBuilder<> alloca_builder(&function->getEntryBlock(),
                                   function->getEntryBlock().begin());
  return alloca_builder.CreateAlloca(llvm_type, nullptr, identifier);
}

llvm::Value *CodeGenerationVisitor::GetOrCreateString(const std::string &str) {
  static uint64_t string_counter = 1;
  if (string_map.contains(str)) {
    return string_map[str];
  }

  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();
  llvm::Value *str_value = builder->CreateGlobalString(
      str, "string_" + std::to_string(string_counter++));
  string_map[str] = str_value;
  return str_value;
}

bool CodeGenerationVisitor::IsPreviousInstructionBlockTerminator() {
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Instruction *last_instruction =
      builder->GetInsertBlock()->getTerminator();
  if (last_instruction && last_instruction->isTerminator()) {
    return true;
  }
  return false;
}

/******************************************************************************
 *                         Punkt array helper methods *
 ******************************************************************************/
void CodeGenerationVisitor::GeneratePunktArrayType() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();

  // Initialize PunktArray struct type for storing arrays.
  llvm::StructType *punkt_array_type =
      llvm::StructType::create(*llvm_context, kPunktArrayStructName);
  std::vector<llvm::Type *> struct_fields = {
      llvm::Type::getInt64Ty(*llvm_context) /* array size */,
      llvm::PointerType::getUnqual(*llvm_context) /* array data */};
  punkt_array_type->setBody(struct_fields);
}

void CodeGenerationVisitor::GenerateAllocPunktArrayFunction() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Create a function that returns a pointer and takes 2 integer arguments.
  std::vector<llvm::Type *> parameters = {
      llvm::Type::getInt64Ty(*llvm_context),
      llvm::Type::getInt64Ty(*llvm_context)};
  llvm::FunctionType *f_type =
      llvm::FunctionType::get(llvm::PointerType::getUnqual(*llvm_context),
                              parameters, /*isVarArg=*/false);
  auto function_alloc_array =
      llvm::Function::Create(f_type, llvm::Function::PrivateLinkage,
                             kAllocPunktArrayFunctionName, *module);

  // Set argument names.
  function_alloc_array->arg_begin()->setName("elem_size");
  (function_alloc_array->arg_begin() + 1)->setName("arr_size");

  // Get a reference to the `malloc` and `memset` functions.
  llvm::Function *malloc_func = module->getFunction(kMallocFunctionName);
  llvm::Function *memset_func = module->getFunction(kMemsetFunctionName);

  llvm::BasicBlock *entry_block =
      llvm::BasicBlock::Create(*llvm_context, "entry", function_alloc_array);
  builder->SetInsertPoint(entry_block);

  // Allocate memory for the PunktArray data type.
  llvm::Value *PunktArray_ptr = builder->CreateCall(
      malloc_func,
      {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 16)},
      "PunktArray_ptr");

  // Store array size in the size field.
  builder->CreateStore(function_alloc_array->getArg(1), PunktArray_ptr);

  // Calculate the number of bytes required for the data field.
  llvm::Value *data_size_in_bytes =
      builder->CreateMul(function_alloc_array->getArg(0),
                         function_alloc_array->getArg(1), "data_size_in_bytes");

  // Allocate memory for the array.
  llvm::Value *data_memory_block = builder->CreateCall(
      malloc_func, {data_size_in_bytes}, "data_memory_block");

  // Initialize all memory within the data block to 0.
  builder->CreateCall(
      memset_func,
      {data_memory_block,
       llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), 0),
       data_size_in_bytes});

  auto PunktArray_struct =
      llvm::StructType::getTypeByName(*llvm_context, kPunktArrayStructName);

  // Get the pointer to the data field of the PunktArray object.
  llvm::Value *PunktArray_data_ptr = builder->CreateGEP(
      PunktArray_struct, PunktArray_ptr,
      {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1)},
      "PunktArray_data_ptr");

  // Store the data memory block in the PunktArray object.
  builder->CreateStore(data_memory_block, PunktArray_data_ptr);

  builder->CreateRet(PunktArray_ptr);
}

void CodeGenerationVisitor::GenerateDeallocPunktArrayFunction() {
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Create a void function that takes a pointer argument.
  std::vector<llvm::Type *> parameters = {
      llvm::PointerType::getUnqual(*llvm_context)};
  llvm::FunctionType *f_type = llvm::FunctionType::get(
      llvm::Type::getVoidTy(*llvm_context), parameters, /*isVarArg=*/false);
  auto function_dealloc_array =
      llvm::Function::Create(f_type, llvm::Function::PrivateLinkage,
                             kDeallocPunktArrayFunctionName, *module);

  // Set argument array.
  function_dealloc_array->arg_begin()->setName("arr");

  // Get a reference to the `free` function.
  llvm::Function *free_func = module->getFunction(kFreeFunctionName);

  llvm::BasicBlock *entry_block =
      llvm::BasicBlock::Create(*llvm_context, "entry", function_dealloc_array);
  builder->SetInsertPoint(entry_block);

  auto PunktArray_struct =
      llvm::StructType::getTypeByName(*llvm_context, kPunktArrayStructName);
  llvm::Value *PunktArray_ptr = function_dealloc_array->getArg(0);

  // First, free PunktArray data field.
  llvm::Value *PunktArray_data_ptr = builder->CreateGEP(
      PunktArray_struct, PunktArray_ptr,
      {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1)},
      "PunktArray_data_ptr");
  llvm::Value *PunktArray_data =
      builder->CreateLoad(llvm::PointerType::getUnqual(*llvm_context),
                          PunktArray_data_ptr, "PunktArray_data");
  builder->CreateCall(free_func, {PunktArray_data});

  // Now free PunktArray object pointer.
  builder->CreateCall(free_func, {PunktArray_ptr});

  builder->CreateRetVoid();
}

const std::string &CodeGenerationVisitor::GetPunktArrayStructName() const {
  return kPunktArrayStructName;
}

const std::string &CodeGenerationVisitor::GetAllocPunktArrayFunctionName()
    const {
  return kAllocPunktArrayFunctionName;
}

const std::string &CodeGenerationVisitor::GetDeallocPunktArrayFunctionName()
    const {
  return kDeallocPunktArrayFunctionName;
}

/******************************************************************************
 *                               Error handling *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode &node) {
  CodeGenerationInternalError("encountered ErrorNode " + node.ToString());
}

[[noreturn]] llvm::Value *CodeGenerationInternalError(
    const std::string &message) {
  PunktLogger::LogFatalInternalError(message);
}
