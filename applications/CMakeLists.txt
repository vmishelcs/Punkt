add_executable(token_printer token_printer.cc)
target_link_libraries(token_printer scanner)

add_executable(ast_printer ast_printer.cc)
target_link_libraries(ast_printer parser)

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
