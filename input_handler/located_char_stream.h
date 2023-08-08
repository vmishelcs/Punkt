#ifndef LOCATED_CHAR_STREAM_H_
#define LOCATED_CHAR_STREAM_H_

#include <fstream>

#include "input_handler.h"

class LocatedCharStream {
public:
	LocatedCharStream(InputHandler input_handler);

	LocatedChar Next();
	LocatedChar NextNonwhitespaceChar();
	LocatedChar Peek();

private:
	LocatedChar next_char;
};

#endif // LOCATED_CHAR_STREAM_H_
