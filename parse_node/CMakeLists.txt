add_subdirectory(parse_nodes)

add_library(parse_node parse_node.cc)

target_link_libraries(parse_node
    boolean_literal_node
    character_literal_node
    code_block_node
    declaration_statement_node
    error_node
    identifier_node
    integer_literal_node
    main_node
    operator_node
    print_statement_node
    program_node
    string_literal_node
    type
    scope
    ${llvm_libs}
)
