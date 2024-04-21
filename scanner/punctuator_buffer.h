#ifndef PUNCTUATOR_BUFFER_H_
#define PUNCTUATOR_BUFFER_H_

#include <input_handler/located_char.h>

#include <string>
#include <vector>

class PunctuatorBuffer {
 public:
  PunctuatorBuffer(LocatedChar first_char) { buffer.push_back(first_char); }

  void Append(LocatedChar ch) { buffer.push_back(ch); }

  LocatedChar PopBack() {
    LocatedChar ch = buffer.back();
    buffer.pop_back();
    return ch;
  }

  std::string ToString() const {
    std::string result;
    for (const LocatedChar& ch : buffer) {
      result.push_back(ch.character);
    }
    return result;
  }

  bool IsEmpty() const { return buffer.empty(); }

  TextLocation GetLocation() const { return buffer.front().location; }

 private:
  std::vector<LocatedChar> buffer;
};

#endif  // PUNCTUATOR_BUFFER_H_
