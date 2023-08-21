#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include <input_handler/input_handler.h>

class InputHandlerTest : public ::testing::Test {
protected:
	std::string test_file_directory = INPUT_HANDLER_TEST_FILE_DIRECTORY;
};

TEST_F(InputHandlerTest, TestInit) {
	InputHandler input_handler(test_file_directory + "TestInit.punkt");
}

TEST_F(InputHandlerTest, TestNext) {
	std::string test_file_name = test_file_directory + "TestNext.punkt";
	std::ifstream test_file(test_file_name);
	std::string test_string;
	char c = 0;
    while (test_file >> std::noskipws >> c) {
        test_string.push_back(c);
    }
	test_file.close();

	InputHandler input_handler(test_file_name);
	for (char c : test_string) {
		LocatedChar lc = input_handler.Next();
		ASSERT_EQ(lc.character, c);
	}
}

TEST_F(InputHandlerTest, TestNextManyLines) {
	std::string test_file_name = test_file_directory + "TestNextManyLines.punkt";
	std::ifstream test_file(test_file_name);
	std::string test_string;
	char c = 0;
    while (test_file >> std::noskipws >> c) {
        test_string.push_back(c);
    }
	test_file.close();

	InputHandler input_handler(test_file_name);
	for (char c : test_string) {
		ASSERT_EQ(input_handler.Next().character, c);
	}
}

TEST_F(InputHandlerTest, TestNextEmptyLines) {
	std::string test_file_name = test_file_directory + "TestNextEmptyLines.punkt";
	std::ifstream test_file(test_file_name);
	std::string test_string;
	char c = 0;
    while (test_file >> std::noskipws >> c) {
        test_string.push_back(c);
    }
	test_file.close();

	InputHandler input_handler(test_file_name);
	for (char c : test_string) {
		ASSERT_EQ(input_handler.Next().character, c);
	}
	for (int i = 0; i < 8; ++i) {
		ASSERT_EQ(input_handler.Next(), FLAG_END_OF_INPUT);
	}
}

TEST_F(InputHandlerTest, TestEmptyFile) {
	InputHandler input_handler(test_file_directory + "TestEmptyFile.punkt");
	for (int i = 0; i < 16; ++i) {
		ASSERT_EQ(input_handler.Next(), FLAG_END_OF_INPUT);
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
