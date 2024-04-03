add_library(signature signature.cc)
target_link_libraries(signature
    type
    operator_code_generator
)

add_library(signatures signatures.cc)
target_link_libraries(signatures
    signature
)
