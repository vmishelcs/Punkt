add_library(alloc_expression_node alloc_expression_node.cc)
add_library(array_type_node array_type_node.cc)
add_library(base_type_node base_type_node.cc)
add_library(boolean_literal_node boolean_literal_node.cc)
add_library(character_literal_node character_literal_node.cc)
add_library(code_block_node code_block_node.cc)
add_library(dealloc_statement_node dealloc_statement_node.cc)
add_library(declaration_statement_node declaration_statement_node.cc)
add_library(error_node error_node.cc)
add_library(expression_statement_node expression_statement_node.cc)
add_library(float_literal_node float_literal_node.cc)
add_library(for_statement_node for_statement_node.cc)
add_library(function_definition_node function_definition_node.cc)
add_library(identifier_node identifier_node.cc)
add_library(if_statement_node if_statement_node.cc)
add_library(integer_literal_node integer_literal_node.cc)
add_library(lambda_invocation_node lambda_invocation_node.cc)
add_library(lambda_node lambda_node.cc)
add_library(lambda_parameter_node lambda_parameter_node.cc)
add_library(lambda_type_node lambda_type_node.cc)
add_library(main_node main_node.cc)
add_library(nop_node nop_node.cc)
add_library(operator_node operator_node.cc)
add_library(populated_array_expression_node populated_array_expression_node.cc)
add_library(print_statement_node print_statement_node.cc)
add_library(program_node program_node.cc)
add_library(return_statement_node return_statement_node.cc)
add_library(string_literal_node string_literal_node.cc)
add_library(while_statement_node while_statement_node.cc)
target_link_libraries(base_type_node type)
target_link_libraries(lambda_type_node type)
