#include <gtest/gtest.h>
#include <scanner/scanner.h>
#include <scanner/keyword.h>
#include <token/integer_literal_token.h>
#include <token/punctuator_token.h>

class ScannerTest : public ::testing::Test {
protected:
	void ReadWordsIntoVector(std::string& file_name, std::vector<std::string>& strings) {
		std::ifstream file(file_name);
		ASSERT_FALSE(file.fail());
		std::string word;
		while (file >> word) {
			strings.push_back(word);
		}
		file.close();
	}

	std::string test_file_directory = SCANNER_TEST_FILE_DIRECTORY;
};

TEST_F(ScannerTest, TestInit) {
	Scanner scanner(test_file_directory + "TestInit.punkt");
}

TEST_F(ScannerTest, TestScanIdentifiers) {
	std::string file_name = test_file_directory + "TestScanIdentifiers.punkt";
	std::vector<std::string> strings;
	ReadWordsIntoVector(file_name, strings);

	Scanner scanner(file_name);
	for (auto str : strings) {
		auto token = scanner.Next();
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
		auto token = scanner.Next();
		ASSERT_EQ(token->GetLexeme(), strings[i]);
		ASSERT_EQ(token->GetTokenType(), TokenType::INTEGER_LITERAL);
		auto integer_literal_token = std::dynamic_pointer_cast<IntegerLiteralToken>(token);
		ASSERT_EQ(integer_literal_token->GetValue(), values[i]);
	}
}

TEST_F(ScannerTest, TestScanKeywords) {
	std::string file_name = test_file_directory + "TestScanKeywords.punkt";
	std::vector<std::string> strings;
	ReadWordsIntoVector(file_name, strings);

	Scanner scanner(file_name);
	int n = strings.size();
	for (int i = 0; i < n; ++i) {
		auto token = scanner.Next();
		ASSERT_EQ(token->GetLexeme(), strings[i]);
		ASSERT_EQ(token->GetTokenType(), TokenType::KEYWORD);
	}
}

TEST_F(ScannerTest, TestScanKeywordsAndIdentifiers) {
	std::string file_name = test_file_directory + "TestScanKeywordsAndIdentifiers.punkt";
	std::vector<std::string> strings;
	ReadWordsIntoVector(file_name, strings);

	Scanner scanner(file_name);
	int n = strings.size();
	for (int i = 0; i < n; ++i) {
		auto token = scanner.Next();
		ASSERT_EQ(token->GetLexeme(), strings[i]);
		if (Keyword::IsKeyword(strings[i])) {
			ASSERT_EQ(token->GetTokenType(), TokenType::KEYWORD);
		}
		else {
			ASSERT_EQ(token->GetTokenType(), TokenType::IDENTIFIER);
		}
	}
}

TEST_F(ScannerTest, TestScanPunctuatorsSimple) {
	std::string file_name = test_file_directory + "TestScanPunctuatorsSimple.punkt";
	std::vector<std::string> strings;
	ReadWordsIntoVector(file_name, strings);
	ASSERT_EQ(strings.size(), 11);

	Scanner scanner(file_name);
	std::shared_ptr<Token> token = nullptr;
	std::shared_ptr<PunctuatorToken> punctuator_token = nullptr;

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[0]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::OPEN_BRACE);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[1]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::CLOSE_BRACE);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[2]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::OPEN_PARENTHESIS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[3]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::CLOSE_PARENTHESIS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[4]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::TERMINATOR);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[5]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::EQUAL);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[6]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::PLUS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[7]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::MINUS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[8]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::MULTIPLY);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), strings[9]);
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::DIVIDE);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][0]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::OPEN_BRACE);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][1]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::CLOSE_BRACE);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][2]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::OPEN_PARENTHESIS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][3]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::CLOSE_PARENTHESIS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][4]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::TERMINATOR);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][5]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::EQUAL);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][6]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::PLUS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][7]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::MINUS);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][8]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::MULTIPLY);

	token = scanner.Next();
	ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][9]));
	ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
	punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
	ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), PunctuatorEnum::DIVIDE);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
