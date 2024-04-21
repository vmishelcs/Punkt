#ifndef PUNCTUATOR_SCANNER_H_
#define PUNCTUATOR_SCANNER_H_

#include <input_handler/located_char_stream.h>
#include <token/punctuator_token.h>

#include <memory>
#include <vector>

#include "punctuator_buffer.h"

class PunctuatorScanner {
 public:
  static std::unique_ptr<PunctuatorToken> Scan(
      LocatedChar first_char, std::unique_ptr<LocatedCharStream> &input_stream);

 private:
  PunctuatorScanner(LocatedChar first_char,
                    std::unique_ptr<LocatedCharStream> &input_stream)
      : scanned(first_char), input_stream(input_stream) {}

  std::unique_ptr<PunctuatorToken> ScanPunctuator();
  void FindLatestValidPunctuator();

  PunctuatorBuffer scanned;
  std::unique_ptr<LocatedCharStream> &input_stream;
};

#endif  // PUNCTUATOR_SCANNER_H_
