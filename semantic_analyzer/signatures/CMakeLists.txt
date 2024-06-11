add_library(signature signature.cc)
target_link_libraries(signature
    type
)

add_library(signatures signatures.cc)
target_link_libraries(signatures
    signature
    operator_codegen
)
