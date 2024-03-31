add_library(code_generation_visitor code_generation_visitor.cc)
target_link_libraries(code_generation_visitor
    ${llvm_libs}
    parse_node
    type
)

add_library(code_generator code_generator.cc)
target_link_libraries(code_generator code_generation_visitor)
