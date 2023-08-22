#ifndef LOCATED_CHAR_STREAM_H_
#define LOCATED_CHAR_STREAM_H_

#include <fstream>
#include <memory>

#include "input_handler.h"

class LocatedCharStream {
public:
	LocatedCharStream(std::string input_file_name);

	LocatedChar Next();
	LocatedChar NextNonwhitespaceChar();
	LocatedChar Peek() const;
	void PutBack(LocatedChar ch);

	bool HasNext() const;

private:
	std::unique_ptr<InputHandler> input_handler;
	LocatedChar next_char;
};

#endif // LOCATED_CHAR_STREAM_H_
