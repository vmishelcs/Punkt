add_library(xml_generator_visitor xml_generator_visitor.cc)
target_link_libraries(xml_generator_visitor
    parse_node
    type
    scope
)
