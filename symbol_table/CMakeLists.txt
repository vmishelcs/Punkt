add_library(symbol_table symbol_table.cc)
add_library(scope scope.cc)
target_link_libraries(scope
    parse_node
    symbol_table
)
