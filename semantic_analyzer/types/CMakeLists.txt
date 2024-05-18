add_library(type type.h)

add_library(base_type base_type.cc)
target_link_libraries(base_type ${LLVM_LINKER_FLAGS})

add_library(array_type array_type.cc)
target_link_libraries(array_type ${LLVM_LINKER_FLAGS})

add_library(lambda_type lambda_type.cc)
target_link_libraries(lambda_type ${LLVM_LINKER_FLAGS})

add_library(arbitrary_type arbitrary_type.cc)

target_link_libraries(type
    base_type
    array_type
    lambda_type
    arbitrary_type
)
