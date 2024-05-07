#include "code_generation_visitor.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Casting.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/type.h>
#include <token/punctuator_token.h>

#include <memory>
#include <string>
#include <variant>

static const std::string kMainFunctionName = "main";
static const std::string kPrintfFunctionName = "printf";
static const std::string kCharFmtString = "%c";
static const std::string kIntFmtString = "%d";
static const std::string kStrFmtString = "%s";
static const char kLineFeedChar = 10;

using code_gen_func_type_1_operand =
    llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *,
                     llvm::Value *);
using code_gen_func_type_2_operands =
    llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *,
                     llvm::Value *, llvm::Value *);

CodeGenerationVisitor::CodeGenerationVisitor(std::string module_id)
    : context(std::make_unique<llvm::LLVMContext>()),
      module(std::make_unique<llvm::Module>(module_id, *context)),
      builder(std::make_unique<llvm::IRBuilder<> >(*context)) {
  std::string target_triple = llvm::sys::getDefaultTargetTriple();
  module->setTargetTriple(target_triple);
}

void CodeGenerationVisitor::WriteIRToFD(int fd) {
  llvm::raw_fd_ostream ir_ostream(fd, /*shouldClose=*/false);
  module->print(ir_ostream, nullptr);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CallStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  return node.GetLambdaInvocationNode()->GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CodeBlockNode &node) {
  for (auto child : node.GetChildren()) {
    llvm::Value *gen = child->GenerateCode(*this);
    if (!gen) break;
  }

  // GenerateCode(CodeBlockNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(
    ExpressionStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  node.GetExpressionNode()->GenerateCode(*this);

  // GenerateCode(ExpressionStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(
    DeclarationStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  IdentifierNode *identifier_node = node.GetIdentifierNode();
  if (!identifier_node) {
    return CodeGenerationInternalError(
        "GenerateCode(DeclarationStatementNode&): "
        "incorrectly constructed declaration node.");
  }

  // Generate code for initializer value.
  llvm::Value *initializer_value = node.GetInitializer()->GenerateCode(*this);
  if (dynamic_cast<LambdaNode *>(node.GetInitializer())) {
    // If the initializer value is a lambda node, all declaration codegen has
    // already been done in the `GenerateCode(LambdaNode&)` method.
    return initializer_value;
  }

  SymbolTableEntry *sym_table_entry = identifier_node->GetSymbolTableEntry();
  if (!sym_table_entry) {
    return CodeGenerationInternalError(
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
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  // Emit code for the loop initializer.
  node.GetLoopInitializerNode()->GenerateCode(*this);

  auto parent_function = builder->GetInsertBlock()->getParent();

  // Create a basic block to check if the loop condition is true/false and
  // attach to the parent function.
  auto condition_block =
      llvm::BasicBlock::Create(*context, "condcheck", parent_function);

  // Create a basic block for the loop body.
  auto loop_block = llvm::BasicBlock::Create(*context, "loop");

  // Create a basic block for the loop exit.
  auto afterloop_block = llvm::BasicBlock::Create(*context, "afterloop");

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
      end_condition, llvm::Type::getInt1Ty(*context), "trunctmp");
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
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(FunctionDefinitionNode &node) {
  // We merely call on the lambda node to generate its code. We store the
  // function's address at the identifier's symbol table entry in the
  // `GenerateCode(LambdaNode&)` method.
  return node.GetLambdaNode()->GenerateCode(*this);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IfStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  llvm::Value *condition = node.GetIfConditionNode()->GenerateCode(*this);
  if (!condition) {
    return CodeGenerationInternalError(
        "failed generating condition for if-statement");
  }
  // Truncate condition to make sure it has LLVM type `i1`.
  condition = builder->CreateTrunc(condition, llvm::Type::getInt1Ty(*context),
                                   "trunctmp");

  auto parent_function = builder->GetInsertBlock()->getParent();

  // Create blocks for 'then' and 'else' cases, and the 'merge' block where the
  // control merges. Note that we do not require the programmer to provide an
  // 'else' clause, hence we leave it as null for now to be initialized later.
  llvm::BasicBlock *then_block =
      llvm::BasicBlock::Create(*context, "then", parent_function);
  llvm::BasicBlock *else_block = nullptr;
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(*context, "ifcont");

  if (node.HasElseBlock()) {
    // This is where we initialize 'else' if it was provided.
    else_block = llvm::BasicBlock::Create(*context, "else");
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
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaInvocationNode &node) {
  llvm::Value *callee_value = node.GetCalleeNode()->GenerateCode(*this);

  std::vector<llvm::Value *> arg_values;
  std::vector<llvm::Type *> arg_types;
  for (const auto &arg : node.GetArgumentNodes()) {
    arg_values.push_back(arg->GenerateCode(*this));
    arg_types.push_back(arg->GetLLVMType(*context));
  }

  Type *return_type = node.GetType();

  // Check if invocation returns void.
  bool returns_void = false;
  auto return_base_type = dynamic_cast<BaseType *>(return_type);
  if (return_base_type &&
      return_base_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
    returns_void = true;
  }

  if (auto function = llvm::dyn_cast<llvm::Function>(callee_value)) {
    // Void instructions cannot have a name.
    return builder->CreateCall(function, arg_values,
                               returns_void ? "" : "calltmp");
  }

  // In order to call a function using its pointer, we need to construct the
  // function type and pass it to IRBuilder::CreateCall.
  auto function_type = llvm::FunctionType::get(node.GetLLVMType(*context),
                                               arg_types, /*isVarArg=*/false);

  // Void instructions cannot have a name.
  return builder->CreateCall(function_type, callee_value, arg_values,
                             returns_void ? "" : "calltmp");
}

llvm::Value *CodeGenerationVisitor::GenerateCode(LambdaNode &node) {
  std::vector<llvm::Type *> param_types;
  std::vector<LambdaParameterNode *> param_nodes = node.GetParameterNodes();
  param_types.reserve(param_nodes.size());

  // Create a vector holding llvm::Type pointers describing the parameter
  // types.
  for (const auto &param_node : param_nodes) {
    param_types.push_back(param_node->GetTypeNode()->GetLLVMType(*context));
  }

  // Create the function type.
  auto function_type = llvm::FunctionType::get(
      node.GetReturnTypeNode()->GetLLVMType(*context), param_types,
      /*isVarArg=*/false);

  // Functions have external linkage, lambdas have private linkage.
  llvm::GlobalValue::LinkageTypes linkage_type =
      node.IsFunction() ? llvm::GlobalValue::LinkageTypes::ExternalLinkage
                        : llvm::GlobalValue::LinkageTypes::InternalLinkage;

  // Create the function.
  auto function = llvm::Function::Create(function_type, linkage_type, "usrfunc",
                                         module.get());

  // Set names for arguments.
  unsigned i = 0;
  for (auto &arg : function->args()) {
    arg.setName(param_nodes[i]->GetIdentifierNode()->GetName());
    ++i;
  }

  // Check if this lambda is anonymous.
  if (!node.IsAnonymous()) {
    // If it is not anonymous, store the function signature in the
    // identifier's symbol table entry.
    IdentifierNode *identifier_node = nullptr;
    if (auto func_def_node =
            dynamic_cast<FunctionDefinitionNode *>(node.GetParent())) {
      identifier_node =
          static_cast<IdentifierNode *>(func_def_node->GetIdentifierNode());
    } else /* parent is a DeclarationStatementNode */
    {
      auto decl_node =
          dynamic_cast<DeclarationStatementNode *>(node.GetParent());
      identifier_node = decl_node->GetIdentifierNode();
    }

    SymbolTableEntry *sym_table_entry = identifier_node->GetSymbolTableEntry();
    sym_table_entry->function = function;
  }

  // In case we are generating lambda variable code, save the current insert
  // block.
  llvm::BasicBlock *builder_insert_point_save = builder->GetInsertBlock();

  // Set up builder to generate function body.
  auto function_entry_block =
      llvm::BasicBlock::Create(*context, "entry", function);
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
      // Otherwise, create a "trap" intrinsic.
      // Create a call to a "trap" intrinsic in case no return statement is
      // provided.
      llvm::Function *trap_intrinsic =
          llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::trap);
      builder->CreateCall(trap_intrinsic);
      builder->CreateUnreachable();
    }
  }

  // Validate generated code.
  if (llvm::verifyFunction(*function, &llvm::errs())) {
    return CodeGenerationInternalError("generated IR is invalid.");
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
  // Main always returns int (always returns 0).
  llvm::Type *return_type = llvm::Type::getInt32Ty(*context);

  // Main does not take any arguments.
  llvm::FunctionType *function_type =
      llvm::FunctionType::get(return_type, /*isVarArgs=*/false);

  // Create an function object for main.
  llvm::Function *main_func =
      llvm::Function::Create(function_type, llvm::Function::ExternalLinkage,
                             kMainFunctionName, *module);

  llvm::BasicBlock *entry_block =
      llvm::BasicBlock::Create(*context, "entry", main_func);
  builder->SetInsertPoint(entry_block);

  // Generate main code block.
  node.GetChild(0)->GenerateCode(*this);

  llvm::BasicBlock *last_main_bb = builder->GetInsertBlock();
  // Check if the last instruction of the block is a terminator.
  llvm::Instruction *last_instruction = last_main_bb->getTerminator();
  if (!last_instruction || !last_instruction->isTerminator()) {
    // Main always returns 0.
    builder->CreateRet(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));
  }

  if (llvm::verifyFunction(*main_func, &llvm::errs())) {
    return CodeGenerationInternalError("generated IR is invalid.");
  }

  return main_func;
}

llvm::Value *CodeGenerationVisitor::GenerateCode(OperatorNode &node) {
  unsigned num_operands = node.GetChildren().size();
  if (num_operands == 1) {
    llvm::Value *operand = node.GetChild(0)->GenerateCode(*this);

    // Obtain codegen function pointer for 1 operand from variant.
    try {
      auto fp = std::get<code_gen_func_type_1_operand>(node.GetCodeGenFunc());
      return fp(context.get(), builder.get(), operand);
    } catch (std::bad_variant_access const &ex) {
      return CodeGenerationInternalError(
          "bad variant access when generating code for 1 operand");
    }
  }
  if (num_operands == 2) {
    llvm::Value *lhs = node.GetChild(0)->GenerateCode(*this);
    llvm::Value *rhs = node.GetChild(1)->GenerateCode(*this);

    // Obtain codegen function pointer for 2 operands from variant.
    try {
      auto fp = std::get<code_gen_func_type_2_operands>(node.GetCodeGenFunc());
      return fp(context.get(), builder.get(), lhs, rhs);
    } catch (std::bad_variant_access const &ex) {
      return CodeGenerationInternalError(
          "bad variant access when generating code for 2 operands");
    }
  } else {
    return CodeGenerationInternalError("code generation not implemented for " +
                                       std::to_string(num_operands) +
                                       " operands");
  }
}

llvm::Value *CodeGenerationVisitor::GenerateCode(PrintStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  // We call printf for each 'operand'
  for (auto child : node.GetChildren()) {
    PrintValue(child->GenerateCode(*this), child->GetType());
  }

  // Print line feed once we are finished.
  PrintLineFeed();

  // GenerateCode(ProgramNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ProgramNode &node) {
  GeneratePrintfDeclaration();

  GenerateGlobalConstants();

  // Generate code for each function of the program.
  for (const auto &child : node.GetChildren()) {
    child->GenerateCode(*this);
  }

  if (llvm::verifyModule(*module, &llvm::errs())) {
    return CodeGenerationInternalError("generated IR is invalid.");
  }

  // GenerateCode(ProgramNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

llvm::Value *CodeGenerationVisitor::GenerateCode(ReturnStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  ParseNode *enclosing_function = node.GetEnclosingFunctionNode();

  if (enclosing_function->GetParseNodeType() == ParseNodeType::MAIN_NODE) {
    // We always return 0 from main.
    return builder->CreateRet(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));
  }

  if (!node.GetReturnValueNode()) {
    // Return void if no return value is specified.
    return builder->CreateRetVoid();
  }

  llvm::Value *return_value = node.GetReturnValueNode()->GenerateCode(*this);
  return builder->CreateRet(return_value);
}

llvm::Value *CodeGenerationVisitor::GenerateCode(WhileStatementNode &node) {
  if (WasPreviousInstructionBlockTerminator()) {
    // No more instructions in this basic block.
    return nullptr;
  }

  auto parent_function = builder->GetInsertBlock()->getParent();

  // Create a basic block to check if the while condition is true/false and
  // attach it to the parent function.
  auto condition_block =
      llvm::BasicBlock::Create(*context, "condcheck", parent_function);

  // Create a basic block for the while loop body.
  auto loop_block = llvm::BasicBlock::Create(*context, "loop");

  // Create a basic block for the loop exit.
  auto afterloop_block = llvm::BasicBlock::Create(*context, "afterloop");

  // Insert an explicit fall-through from the current block (before the loop) to
  // the loop condition block.
  builder->CreateBr(condition_block);

  // Now we are inserting instructions into the loop condition block.
  builder->SetInsertPoint(condition_block);

  // Generate code for the while loop condition.
  llvm::Value *condition = node.GetConditionNode()->GenerateCode(*this);
  if (!condition) {
    return CodeGenerationInternalError(
        "failed generating condition for while-statement");
  }
  // Truncate condition to make sure it has type `i1`.
  condition = builder->CreateTrunc(condition, llvm::Type::getInt1Ty(*context),
                                   "trunctmp");
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
    // Create an unconditional branch to the start of the loop, where we check
    // the while condition.
    builder->CreateBr(condition_block);
  }

  // Append the afterloop block. We jump to this block if the while condition is
  // false.
  parent_function->insert(parent_function->end(), afterloop_block);

  // Any subsequent code is inserted in the afterloop block.
  builder->SetInsertPoint(afterloop_block);

  // GenerateCode(WhileStatementNode&) return value is not used.
  return llvm::Constant::getNullValue(llvm::Type::getVoidTy(*context));
}

/******************************************************************************
 *                      Code generation for identifiers                       *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode &node) {
  // Look up the symbol table entry for this identifier.
  auto sym_table_entry = node.GetSymbolTableEntry();
  if (!sym_table_entry) {
    CodeGenerationInternalError("missing entry in symbol table for " +
                                node.ToString());
  }

  if (sym_table_entry->symbol_type == SymbolType::LAMBDA) {
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
  return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context),
                                (int)node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(CharacterLiteralNode &node) {
  return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context),
                                node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode &node) {
  return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context),
                                      node.GetValue());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(StringLiteralNode &node) {
  return builder->CreateGlobalString(node.GetValue(), "", 0, module.get());
}

llvm::Value *CodeGenerationVisitor::GenerateCode(BaseTypeNode &node) {
  // GenerateCode(BaseTypeNode&) is not used.
  return nullptr;
}

/******************************************************************************
 *                            NOP code generation                             *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(NopNode &node) {
  return builder->CreateAdd(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0),
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0), "nop");
}

/******************************************************************************
 *                              Printing helpers                              *
 ******************************************************************************/
void CodeGenerationVisitor::GeneratePrintfDeclaration() {
  // Create a vector for parameters
  std::vector<llvm::Type *> parameters(1,
                                       llvm::PointerType::getUnqual(*context));
  // Create a function type returning a 32-bit int, taking 1 parameter and a
  // variable number of arguments
  llvm::FunctionType *printf_func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(*context), parameters, /* IsVarArg = */ true);

  // Create the function declaration
  llvm::Function *printf_func =
      llvm::Function::Create(printf_func_type, llvm::Function::ExternalLinkage,
                             kPrintfFunctionName, *module);
  if (!printf_func) {
    CodeGenerationInternalError("could not generate declaration for printf");
  }
}

void CodeGenerationVisitor::GeneratePrintfFmtStringsForBaseTypes() {
  llvm::Value *fmt_str = nullptr;

  if (fmt_str =
          GenerateFmtStringForBaseType(BaseTypeEnum::CHARACTER, kCharFmtString),
      !fmt_str) {
    CodeGenerationInternalError(
        "failed to generate format string for CHARACTER type");
  }
  global_constants_table[kCharFmtString] = fmt_str;

  if (fmt_str =
          GenerateFmtStringForBaseType(BaseTypeEnum::INTEGER, kIntFmtString),
      !fmt_str) {
    CodeGenerationInternalError(
        "failed to generate format string for INTEGER type");
  }
  global_constants_table[kIntFmtString] = fmt_str;

  if (fmt_str =
          GenerateFmtStringForBaseType(BaseTypeEnum::STRING, kStrFmtString),
      !fmt_str) {
    CodeGenerationInternalError(
        "failed to generate format string for STRING type");
  }
  global_constants_table[kStrFmtString] = fmt_str;
}

llvm::Value *CodeGenerationVisitor::GenerateFmtStringForBaseType(
    BaseTypeEnum base_type_enum, std::string fmt_str) {
  return builder->CreateGlobalString(
      fmt_str, ".fmt_" + BaseType::GetEnumString(base_type_enum), 0,
      module.get());
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtString(Type *type) {
  if (type->GetTypeEnum() == TypeEnum::BASE_TYPE) {
    BaseType *base_type = dynamic_cast<BaseType *>(type);
    if (!base_type) {
      return CodeGenerationInternalError("failed to cast Type to BaseType");
    }

    return GetPrintfFmtStringForBaseType(base_type->GetBaseTypeEnum());
  }
  return CodeGenerationInternalError(
      "CodeGenerationVisitor::GetPrintfFmtString not implemented for "
      "non-base "
      "types");
}

llvm::Value *CodeGenerationVisitor::GetPrintfFmtStringForBaseType(
    BaseTypeEnum base_type_enum) {
  const std::string *key = nullptr;
  switch (base_type_enum) {
    case BaseTypeEnum::CHARACTER:
      key = &kCharFmtString;
      break;
    case BaseTypeEnum::BOOLEAN:
    case BaseTypeEnum::INTEGER:
      key = &kIntFmtString;
      break;
    case BaseTypeEnum::STRING:
      key = &kStrFmtString;
      break;
    default:
      return CodeGenerationInternalError(
          "unimplemented format string for type " +
          BaseType::GetEnumString(base_type_enum));
  }

  if (!global_constants_table.contains(*key)) {
    return CodeGenerationInternalError("unimplemented format string");
  }
  return global_constants_table.at(*key);
}

llvm::Value *CodeGenerationVisitor::PrintValue(llvm::Value *value, Type *type) {
  llvm::Function *printf_func = module->getFunction(kPrintfFunctionName);
  if (!printf_func) {
    return CodeGenerationInternalError(
        "unable to obtain llvm::Function pointer for printf");
  }

  std::vector<llvm::Value *> printf_args;

  printf_args.push_back(GetPrintfFmtString(type));
  if (printf_args.back() == nullptr) {
    return CodeGenerationInternalError(
        "failed obtaining llvm::Value object for fmt string");
  }

  auto print_value = value;

  // When printing booleans, we first truncate to 1-bit such that only the
  // least significant bit remains. Then we extend to 32 bits and print the
  // boolean as an integer.
  BaseType *base_type = dynamic_cast<BaseType *>(type);
  if (base_type && base_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
    auto truncated_value = builder->CreateTrunc(
        print_value, llvm::Type::getInt1Ty(*context), "trunctmp");
    print_value = builder->CreateZExt(
        truncated_value, llvm::Type::getInt32Ty(*context), "zexttmp");
  }
  // When printing characters, we sign-extend to 32 bits.
  else if (base_type && base_type->IsEquivalentTo(BaseTypeEnum::CHARACTER)) {
    print_value = builder->CreateSExt(
        print_value, llvm::Type::getInt32Ty(*context), "sexttmp");
  }

  printf_args.push_back(print_value);
  if (printf_args.back() == nullptr) {
    return CodeGenerationInternalError(
        "failed to generate argument for printf");
  }

  return builder->CreateCall(printf_func, printf_args, "printf_ret");
}

llvm::Value *CodeGenerationVisitor::PrintLineFeed() {
  auto temp_char_base_type = BaseType::Create(BaseTypeEnum::CHARACTER);
  return PrintValue(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), kLineFeedChar),
      temp_char_base_type.get());
}

/******************************************************************************
 *                           Miscellaneous helpers                            *
 ******************************************************************************/
void CodeGenerationVisitor::GenerateGlobalConstants() {
  GeneratePrintfFmtStringsForBaseTypes();
}

llvm::AllocaInst *CodeGenerationVisitor::CreateEntryBlockAlloca(
    llvm::Function *function, const std::string &identifier_name,
    llvm::Type *llvm_type) {
  llvm::IRBuilder<> tmp_builder(&function->getEntryBlock(),
                                function->getEntryBlock().begin());
  return tmp_builder.CreateAlloca(llvm_type, nullptr, identifier_name);
}

/******************************************************************************
 *                               Error handling                               *
 ******************************************************************************/
llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode &node) {
  return CodeGenerationInternalError("encountered ErrorNode " +
                                     node.ToString());
}

llvm::Value *CodeGenerationVisitor::CodeGenerationInternalError(
    std::string message) {
  return (llvm::Value *)PunktLogger::LogFatalInternalError(message);
}
