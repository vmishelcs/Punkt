add_library(scanner scanner.cc)
add_library(keyword keyword.cc)
add_library(punctuator punctuator.cc)
target_link_libraries(keyword logger)
target_link_libraries(punctuator logger)
target_link_libraries(scanner
    located_char_stream
    token
    keyword
    punctuator
    logger
)
