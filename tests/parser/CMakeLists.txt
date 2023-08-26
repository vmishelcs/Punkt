add_executable(parser_test parser_test.cc)
target_link_libraries(parser_test scanner parser GTest::gtest)
gtest_discover_tests(parser_test)

target_compile_definitions(
	parser_test
	PRIVATE
	PARSER_TEST_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/tests/parser/test_files/"
)
