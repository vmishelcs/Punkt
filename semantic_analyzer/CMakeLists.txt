add_library(type type.cc)

add_library(semantic_analysis_visitor semantic_analysis_visitor.cc)
target_link_libraries(semantic_analysis_visitor
    type
)

add_library(semantic_analyzer semantic_analyzer.cc)
target_link_libraries(semantic_analyzer
    semantic_analysis_visitor
)
