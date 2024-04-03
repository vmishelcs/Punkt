add_library(operator_code_generator operator_code_generator.cc)
target_link_libraries(operator_code_generator
    ${llvm_libs}
)

add_library(code_generation_visitor code_generation_visitor.cc)
target_link_libraries(code_generation_visitor
    parse_node
    type
    ${llvm_libs}
)

add_library(code_generator code_generator.cc)
target_link_libraries(code_generator code_generation_visitor)
