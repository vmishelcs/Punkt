add_executable(token_printer token_printer.cc)

target_compile_definitions(
	token_printer
	PRIVATE
	INPUT_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/input_files/"
)
