#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include <input_handler/input_handler.h>

class InputHandlerTest : public ::testing::Test {
protected:
	void ReadFileIntoBuffer(std::string file_name, std::string& buffer) {
		std::ifstream file(file_name);
		char c = 0;
		while (file >> std::noskipws >> c) {
			buffer.push_back(c);
		}
		file.close();
	}

	std::string test_file_directory = INPUT_HANDLER_TEST_FILE_DIRECTORY;
};

TEST_F(InputHandlerTest, TestInit) {
	InputHandler input_handler(test_file_directory + "TestInit.punkt");
}

TEST_F(InputHandlerTest, TestNext) {
	std::string test_file_name = test_file_directory + "TestNext.punkt";
	std::string test_string;
	ReadFileIntoBuffer(test_file_name, test_string);

	InputHandler input_handler(test_file_name);
	for (char c : test_string) {
		LocatedChar lc = input_handler.Next();
		ASSERT_EQ(lc.character, c);
	}
}

TEST_F(InputHandlerTest, TestNextManyLines) {
	std::string test_file_name = test_file_directory + "TestNextManyLines.punkt";
	std::string test_string;
	ReadFileIntoBuffer(test_file_name, test_string);

	InputHandler input_handler(test_file_name);
	for (char c : test_string) {
		ASSERT_EQ(input_handler.Next().character, c);
	}
}

TEST_F(InputHandlerTest, TestNextEmptyLines) {
	std::string test_file_name = test_file_directory + "TestNextEmptyLines.punkt";
	std::string test_string;
	ReadFileIntoBuffer(test_file_name, test_string);

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

TEST_F(InputHandlerTest, TestPutBack) {
	std::string test_file_name = test_file_directory + "TestPutBack.punkt";
	std::string test_string;
	ReadFileIntoBuffer(test_file_name, test_string);

	InputHandler input_handler(test_file_name);
	int n = test_string.length();
	for (int i = 0; i < n; ++i) {
		if (i % 3 == 0) {
			auto lc = input_handler.Next();
			ASSERT_EQ(lc.character, test_string[i]);
			input_handler.PutBack(lc);
			lc = input_handler.Next();
			ASSERT_EQ(lc.character, test_string[i]);
		} else {
			ASSERT_EQ(input_handler.Next().character, test_string[i]);
		}
	}
}

TEST_F(InputHandlerTest, TestLargeInput) {
	std::string test_file_name = test_file_directory + "TestLargeInput.punkt";
	std::string test_string;
	ReadFileIntoBuffer(test_file_name, test_string);

	srand(1024);
	InputHandler input_handler(test_file_name);
	int n = test_string.length();
	for (int i = 0; i < n; ++i) {
		if (rand() % 2 == 0) {
			auto lc = input_handler.Next();
			ASSERT_EQ(lc.character, test_string[i]);
			input_handler.PutBack(lc);
			lc = input_handler.Next();
			ASSERT_EQ(lc.character, test_string[i]);
		} else {
			ASSERT_EQ(input_handler.Next().character, test_string[i]);
		}
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
