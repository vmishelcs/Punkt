#include <glog/logging.h>
#include <gtest/gtest.h>
#include <scanner/scanner.h>


class ScannerTest : public ::testing::Test {
protected:
};

TEST_F(ScannerTest, TestInit) {
	Scanner *scanner = Scanner::MakeFromFileName("");
    delete scanner;
}

int main(int argc, char **argv) {
	::google::InitGoogleLogging(argv[0]);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
