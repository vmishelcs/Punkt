add_subdirectory(parse_nodes)

add_library(parse_node parse_node.cc)

target_link_libraries(parse_node
    assignment_statement_node
    boolean_literal_node
    character_literal_node
    code_block_node
    declaration_statement_node
    error_node
    for_statement_node
    function_definition_node
    identifier_node
    if_statement_node
    integer_literal_node
    lambda_invocation_node
    lambda_node
    lambda_parameter_node
    main_node
    nop_node
    operator_node
    print_statement_node
    program_node
    return_statement_node
    string_literal_node
    type_node
    type
    scope
    ${llvm_libs}
)
