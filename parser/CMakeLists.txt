add_library(parser parser.cc)
target_link_libraries(parser
    scanner
    parse_node
)
