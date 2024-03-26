#ifndef TEXT_LOCATION_H_
#define TEXT_LOCATION_H_

#include <iostream>
#include <string>

struct TextLocation {
	TextLocation(std::string file_name, int line, int column)
		: file_name(file_name)
		, line(line)
		, column(column)
	{}
	TextLocation(const TextLocation& tl)
		: file_name(tl.file_name)
		, line(tl.line)
		, column(tl.column)
	{}

	std::string ToString() const {
		std::string result = file_name + ':' + std::to_string(line) + ':' + std::to_string(column);
		return result;
	}

	std::string file_name;
	int line;
	int column;
};

inline std::ostream& operator<<(std::ostream& os, const TextLocation& tl) {
    os << tl.file_name << ":" << tl.line << ":" << tl.column;
    return os;
}

#endif // TEXT_LOCATION_H_
