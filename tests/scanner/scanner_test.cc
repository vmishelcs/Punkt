#include <gtest/gtest.h>
#include <scanner/keyword.h>
#include <scanner/scanner.h>
#include <token/integer_literal_token.h>
#include <token/punctuator_token.h>

class ScannerTest : public ::testing::Test {
 protected:
  void ReadWordsIntoVector(fs::path &file_path,
                           std::vector<std::string> &strings) {
    std::ifstream file(file_path);
    ASSERT_FALSE(file.fail());
    std::string word;
    while (file >> word) {
      strings.push_back(word);
    }
    file.close();
  }

  fs::path test_file_directory = fs::path(SCANNER_TEST_FILE_DIRECTORY);
};

TEST_F(ScannerTest, TestInit) {
  Scanner scanner(test_file_directory / "TestInit.punkt");
}

TEST_F(ScannerTest, TestScanIdentifiers) {
  fs::path file_path = test_file_directory / "TestScanIdentifiers.punkt";
  std::vector<std::string> strings;
  ReadWordsIntoVector(file_path, strings);

  Scanner scanner(file_path);
  for (auto str : strings) {
    auto token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), str);
    ASSERT_EQ(token->GetTokenType(), TokenType::IDENTIFIER);
  }
}

TEST_F(ScannerTest, TestScanIntegerLiterals) {
  fs::path file_path = test_file_directory / "TestScanIntegerLiterals.punkt";
  std::ifstream file(file_path);
  std::string int_literal;
  std::string int_value;
  std::vector<std::string> strings;
  std::vector<int> values;
  while (file >> int_literal) {
    strings.push_back(int_literal);
    values.push_back(std::stoi(int_literal));
  }
  file.close();

  Scanner scanner(file_path);
  int n = strings.size();
  for (int i = 0; i < n; ++i) {
    auto token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), strings[i]);
    ASSERT_EQ(token->GetTokenType(), TokenType::INTEGER_LITERAL);
    IntegerLiteralToken *integer_literal_token =
        dynamic_cast<IntegerLiteralToken *>(token.get());
    ASSERT_EQ(integer_literal_token->GetValue(), values[i]);
  }
}

TEST_F(ScannerTest, TestScanKeywords) {
  fs::path file_path = test_file_directory / "TestScanKeywords.punkt";
  std::vector<std::string> strings;
  ReadWordsIntoVector(file_path, strings);

  Scanner scanner(file_path);
  int n = strings.size();
  for (int i = 0; i < n; ++i) {
    auto token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), strings[i]);
    ASSERT_EQ(token->GetTokenType(), TokenType::KEYWORD);
  }
}

TEST_F(ScannerTest, TestScanKeywordsAndIdentifiers) {
  fs::path file_path =
      test_file_directory / "TestScanKeywordsAndIdentifiers.punkt";
  std::vector<std::string> strings;
  ReadWordsIntoVector(file_path, strings);

  Scanner scanner(file_path);
  int n = strings.size();
  for (int i = 0; i < n; ++i) {
    auto token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), strings[i]);
    if (keyword_utils::IsKeyword(strings[i])) {
      ASSERT_EQ(token->GetTokenType(), TokenType::KEYWORD);
    } else {
      ASSERT_EQ(token->GetTokenType(), TokenType::IDENTIFIER);
    }
  }
}

TEST_F(ScannerTest, TestScanPunctuatorsSimple) {
  fs::path file_path = test_file_directory / "TestScanPunctuatorsSimple.punkt";
  std::vector<std::string> strings;
  ReadWordsIntoVector(file_path, strings);
  ASSERT_EQ(strings.size(), 11);

  Scanner scanner(file_path);
  std::shared_ptr<Token> token = nullptr;
  std::shared_ptr<PunctuatorToken> punctuator_token = nullptr;
  int num_scanned_punctuators = 10;
  std::vector<Punctuator> punctuator_enums = {
      Punctuator::OPEN_BRACE,
      Punctuator::CLOSE_BRACE,
      Punctuator::OPEN_PARENTHESIS,
      Punctuator::CLOSE_PARENTHESIS,
      Punctuator::TERMINATOR,
      Punctuator::ASSIGN,
      Punctuator::PLUS,
      Punctuator::MINUS,
      Punctuator::MUL,
      Punctuator::DIV,
  };

  for (int i = 0; i < num_scanned_punctuators; ++i) {
    token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), strings[i]);
    ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
    punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
    ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), punctuator_enums[i]);
  }

  for (int i = 0; i < num_scanned_punctuators; ++i) {
    token = scanner.Next();
    ASSERT_EQ(token->GetLexeme(), std::string(1, strings[10][i]));
    ASSERT_EQ(token->GetTokenType(), TokenType::PUNCTUATOR);
    punctuator_token = std::dynamic_pointer_cast<PunctuatorToken>(token);
    ASSERT_EQ(punctuator_token->GetPunctuatorEnum(), punctuator_enums[i]);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
