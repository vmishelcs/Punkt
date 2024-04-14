add_subdirectory(signatures)
add_subdirectory(types)

add_library(function_declaration_visitor function_declaration_visitor.cc)
target_link_libraries(function_declaration_visitor
    parse_node
    type
    scope
)

add_library(semantic_analysis_visitor semantic_analysis_visitor.cc)
target_link_libraries(semantic_analysis_visitor
    function_declaration_visitor
    parse_node
    type
    scope
    signatures
)

add_library(semantic_analyzer semantic_analyzer.cc)
target_link_libraries(semantic_analyzer
    semantic_analysis_visitor
)
