add_executable(token_printer token_printer.cc)
target_link_libraries(token_printer scanner)

add_executable(ast_printer ast_printer.cc)
target_link_libraries(ast_printer parser)

add_executable(decorated_ast_printer decorated_ast_printer.cc)
target_link_libraries(decorated_ast_printer parser semantic_analyzer)

target_compile_definitions(
	token_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/input_files/"
)

target_compile_definitions(
	ast_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/input_files/"
)

target_compile_definitions(
	decorated_ast_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/input_files/"
)
