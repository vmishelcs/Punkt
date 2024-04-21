add_library(signature signature.cc)
target_link_libraries(signature
    type
    operator_codegen
)

add_library(signatures signatures.cc)
target_link_libraries(signatures
    signature
)
