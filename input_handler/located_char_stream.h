#ifndef LOCATED_CHAR_STREAM_H_
#define LOCATED_CHAR_STREAM_H_

#include <fstream>

#include "input_handler.h"

class LocatedCharStream {
public:
	static LocatedCharStream *FromFileName(std::string file_name);
	~LocatedCharStream();

	LocatedChar Next();
	LocatedChar NextNonwhitespaceChar();
	LocatedChar Peek();

private:
	LocatedCharStream(InputHandler *input_handler);

	InputHandler *input_handler;
	LocatedChar next_char;
};

#endif // LOCATED_CHAR_STREAM_H_
