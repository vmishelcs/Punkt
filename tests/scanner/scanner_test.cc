#include <iomanip>
#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <scanner/scanner.h>

#include <token/integer_literal_token.h>

class ScannerTest : public ::testing::Test {
protected:
	std::string test_file_directory = SCANNER_TEST_FILE_DIRECTORY;
};

TEST_F(ScannerTest, TestInit) {
	Scanner scanner(test_file_directory + "TestInit.punkt");
}

TEST_F(ScannerTest, TestScanIdentifiers) {
	std::string file_name = test_file_directory + "TestScanIdentifiers.punkt";
	std::ifstream file(file_name);
	std::string word;
	std::vector<std::string> strings;
	while (file >> word) {
		strings.push_back(word);
	}
	file.close();

	Scanner scanner(file_name);
	for (auto str : strings) {
		auto token = scanner.GetNextToken();
		ASSERT_EQ(token->GetLexeme(), str);
		ASSERT_EQ(token->GetTokenType(), TokenType::IDENTIFIER);
	}
}

TEST_F(ScannerTest, TestScanIntegerLiterals) {
	std::string file_name = test_file_directory + "TestScanIntegerLiterals.punkt";
	std::ifstream file(file_name);
	std::string int_literal;
	std::string int_value;
	std::vector<std::string> strings;
	std::vector<int> values;
	while (file >> int_literal) {
		strings.push_back(int_literal);
		values.push_back(std::stoi(int_literal));
	}
	file.close();

	Scanner scanner(file_name);
	int n = strings.size();
	for (int i = 0; i < n; ++i) {
		auto token = scanner.GetNextToken();
		ASSERT_EQ(token->GetLexeme(), strings[i]);
		ASSERT_EQ(token->GetTokenType(), TokenType::INTEGER_LITERAL);
		auto integer_literal_token = std::dynamic_pointer_cast<IntegerLiteralToken>(token);
		ASSERT_EQ(integer_literal_token->GetValue(), values[i]);
	}
}

int main(int argc, char **argv) {
	::google::InitGoogleLogging(argv[0]);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
