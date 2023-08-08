#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include <input_handler/input_handler.h>

class InputHandlerTest : public ::testing::Test {
protected:
	std::string test_file_directory = INPUT_HANDLER_TEST_FILE_DIRECTORY;
};

TEST_F(InputHandlerTest, Init) {
	InputHandler input_handler(test_file_directory + "Init.punkt");
}

TEST_F(InputHandlerTest, TestNext) {
	InputHandler input_handler(test_file_directory + "TestNext.punkt");
	std::string test_string = "Hello World!";
	for (char c : test_string) {
		LocatedChar lc = input_handler.Next();
		EXPECT_EQ(lc.character, c);
	}
}

TEST_F(InputHandlerTest, TestNextManyLines) {
	InputHandler input_handler(test_file_directory + "TestNextManyLines.punkt");
	std::vector<std::string> lines = {
		"Hello World!",
		"This is a file with many lines...",
		" ",
		"It also has lines with just a single whitespace character?",
		"	",
		"	Indented lines too!!!",
		"Good-bye cruel world!"
	};

	for (auto line : lines) {
		for (char c : line) {
			EXPECT_EQ(input_handler.Next().character, c);
		}
	}
}
