#include <gtest/gtest.h>
#include <scanner/scanner.h>
#include <parser/parser.h>

class ParserTest : public ::testing::Test {
protected:
	std::string test_file_directory = PARSER_TEST_FILE_DIRECTORY;
};

TEST_F(ParserTest, TestInit) {
	std::string file_name = test_file_directory + "TestInit.punkt";
	Scanner scanner(file_name);
	auto tree = Parser::Parse(scanner);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
