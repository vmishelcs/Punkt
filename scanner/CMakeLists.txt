add_library(scanner scanner.cc)
add_library(keyword keyword.cc)
add_library(punctuator punctuator.cc punctuator_scanner.cc)
target_link_libraries(scanner
    located_char_stream
    token
    keyword
    punctuator
    logger
)
