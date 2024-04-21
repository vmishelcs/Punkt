add_library(type type.h)
add_library(base_type base_type.cc)
add_library(lambda_type lambda_type.cc)

target_link_libraries(type
    base_type
    lambda_type
)
