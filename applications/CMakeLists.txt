add_executable(token_printer token_printer.cc)
target_link_libraries(token_printer scanner)

add_executable(ast_printer ast_printer.cc)
target_link_libraries(ast_printer parser xml_generator_visitor)

add_executable(ast_xml_printer ast_xml_printer.cc)
target_link_libraries(ast_xml_printer parser xml_generator_visitor)

add_executable(decorated_ast_printer decorated_ast_printer.cc)
target_link_libraries(decorated_ast_printer parser semantic_analyzer xml_generator_visitor)

add_executable(code_printer code_printer.cc)
target_link_libraries(code_printer parser semantic_analyzer code_generator)

add_executable(punkt_compiler punkt_compiler.cc)
target_link_libraries(punkt_compiler parser semantic_analyzer code_generator xml_generator_visitor)

target_compile_definitions(
	token_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/applications_input_files/"
)

target_compile_definitions(
	ast_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/applications_input_files/"
)

target_compile_definitions(
	decorated_ast_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/applications_input_files/"
)

target_compile_definitions(
	code_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/applications_input_files/"
)
