add_library(codegen_context codegen_context.cc)
target_link_libraries(codegen_context
    ${llvm_libs}
)

add_library(operator_codegen operator_codegen.cc)
target_link_libraries(operator_codegen
    codegen_context
    ${llvm_libs}
)

add_library(code_generation_visitor code_generation_visitor.cc)
target_link_libraries(code_generation_visitor
    codegen_context
    parse_node
    type
    ${llvm_libs}
)

add_library(code_generator code_generator.cc)
target_link_libraries(code_generator code_generation_visitor)
