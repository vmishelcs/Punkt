add_library(symbol_table symbol_table.cc)
target_link_libraries(symbol_table
    logger
)
add_library(scope scope.cc)
target_link_libraries(scope
    symbol_table
)
