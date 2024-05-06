#include "input_handler.h"

#include <logging/punkt_logger.h>

#include <cstdlib>

LocatedChar LocatedChar::EOF_LOCATED_CHAR(0, "", -1, -1);

InputHandler::InputHandler(fs::path file_path) : file_path(file_path) {
  if (file_path.filename().empty()) {
    PunktLogger::LogFatal("no input file");
  }
  input_file = std::ifstream(file_path, std::ios::in);
  if (!input_file.is_open()) {
    PunktLogger::LogFatal("failed to open input file " + file_path.string());
  }

  line_num = 0;
  column_num = 1;
  while (char_stream.empty() && FileHasMoreLines()) {
    PreloadNextLine();
  }
}

InputHandler::~InputHandler() { input_file.close(); }

LocatedChar InputHandler::Next() {
  while (!LineHasMoreChars() && FileHasMoreLines()) {
    PreloadNextLine();
  }
  if (LineHasMoreChars()) {
    LocatedChar lc = char_stream.front();
    char_stream.pop_front();
    return lc;
  }

  if (LocatedChar::EOF_LOCATED_CHAR.location.file_name.length() == 0) {
    // This means we haven't properly initialized the EOF char
    InitializeEOFLocatedChar();
  }
  return LocatedChar::EOF_LOCATED_CHAR;
}

void InputHandler::PutBack(LocatedChar ch) { char_stream.push_front(ch); }

bool InputHandler::LineHasMoreChars() const { return !char_stream.empty(); }

bool InputHandler::FileHasMoreLines() const { return !input_file.eof(); }

void InputHandler::PreloadNextLine() {
  char c = 0;
  column_num = 1;
  line_num += 1;
  while (input_file >> std::noskipws >> c && c != '\n') {
    char_stream.push_back(
        LocatedChar(c, file_path.filename(), line_num, column_num));
    ++column_num;
  }
  if (!input_file.eof()) {
    char_stream.push_back(
        LocatedChar('\n', file_path.filename(), line_num, column_num));
    ++column_num;
  }
}

void InputHandler::InitializeEOFLocatedChar() {
  LocatedChar::EOF_LOCATED_CHAR = LocatedChar(0, this->file_path.filename(),
                                              this->line_num, this->column_num);
}
