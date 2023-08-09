#include <glog/logging.h>
#include <iostream>

#include <scanner/scanner.h>

int main(int argc, char **argv) {
	::google::InitGoogleLogging(argv[0]);
	if (argc != 2) {
		LOG(ERROR) << "Token printer application requires an input file argument";
		return 0;
	}
}
