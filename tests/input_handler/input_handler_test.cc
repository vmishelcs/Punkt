#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include <input_handler/input_handler.h>

class InputHandlerTest : public ::testing::Test {
protected:
	std::string test_file_directory = INPUT_HANDLER_TEST_FILE_DIRECTORY;
};

TEST_F(InputHandlerTest, Init) {
	InputHandler input_handler(test_file_directory + "Init.punkt");
}
