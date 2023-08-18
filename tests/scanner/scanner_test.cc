#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <scanner/scanner.h>

class ScannerTest : public ::testing::Test {
protected:
	std::string test_file_directory = SCANNER_TEST_FILE_DIRECTORY;
};

TEST_F(ScannerTest, TestInit) {
	Scanner scanner(test_file_directory + "TestInit.punkt");
}

TEST_F(ScannerTest, TestScanIdentifiers) {
	std::ifstream file(test_file_directory + "TestScanIdentifiers.punkt");
	std::string word;
	std::vector<std::string> words;
	while (file >> word) {
		words.push_back(word);
	}
	file.close();

	Scanner scanner(test_file_directory + "TestScanIdentifiers.punkt");
	for (auto word : words) {
		auto token = scanner.GetNextToken();
		ASSERT_EQ(token->GetLexeme(), word);
		ASSERT_EQ(token->GetTokenType(), TokenType::IDENTIFIER);
	}
}

int main(int argc, char **argv) {
	::google::InitGoogleLogging(argv[0]);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
