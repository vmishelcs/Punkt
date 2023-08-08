#ifndef TEXT_LOCATION_H_
#define TEXT_LOCATION_H_

#include <string>

struct TextLocation {
	TextLocation(std::string file_name, unsigned int line, unsigned int column)
		: file_name(file_name)
		, line(line)
		, column(column)
	{}

	std::string file_name;
	unsigned int line;
	unsigned int column;
};

#endif // TEXT_LOCATION_H_
