add_executable(input_handler_test input_handler_test.cc)
target_link_libraries(input_handler_test input_handler GTest::gtest)
include(GoogleTest)
gtest_discover_tests(input_handler_test)

target_compile_definitions(
	input_handler_test
	PRIVATE
	INPUT_HANDLER_TEST_FILE_DIRECTORY="${PROJECT_SOURCE_DIR}/tests/input_handler/test_files/"
)
