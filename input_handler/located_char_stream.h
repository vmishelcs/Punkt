#ifndef LOCATED_CHAR_STREAM_H_
#define LOCATED_CHAR_STREAM_H_

#include <fstream>
#include <memory>

#include "input_handler.h"

/// @brief Simple implementation of a stream of LocatedChars.
class LocatedCharStream {
 public:
  LocatedCharStream(fs::path file_path);

  LocatedChar Next();
  LocatedChar NextNonwhitespaceChar();
  LocatedChar Peek() const;
  void PutBack(LocatedChar ch);

  bool HasNext() const;

 private:
  std::unique_ptr<InputHandler> input_handler;
  LocatedChar next_char;
};

#endif  // LOCATED_CHAR_STREAM_H_
