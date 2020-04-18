#include "TelnetHandler.h"
#include <array>

namespace GwiezdnyPyl {

namespace Colors {
	const std::string reset = "\x1B[0m";
	const std::string bold = "\x1B[1m";
	const std::string dim = "\x1B[2m";
	const std::string under = "\x1B[4m";
	const std::string reverse = "\x1B[7m";
	const std::string hide = "\x1B[8m";

	const std::string clearscreen = "\x1B[2J";
	const std::string clearline = "\x1B[2K";

	const std::string black = "\x1B[30m";
	const std::string red = "\x1B[31m";
	const std::string green = "\x1B[32m";
	const std::string yellow = "\x1B[33m";
	const std::string blue = "\x1B[34m";
	const std::string magenta = "\x1B[35m";
	const std::string cyan = "\x1B[36m";
	const std::string white = "\x1B[37m";

	const std::string bblack = "\x1B[40m";
	const std::string bred = "\x1B[41m";
	const std::string bgreen = "\x1B[42m";
	const std::string byellow = "\x1B[43m";
	const std::string bblue = "\x1B[44m";
	const std::string bmagenta = "\x1B[45m";
	const std::string bcyan = "\x1B[46m";
	const std::string bwhite = "\x1B[47m";

	const std::string newline = "\r\n\x1B[0m";

	const std::array<std::array<std::array<std::string, 3>, 3>, 3> telnetColors = {
		black + dim, blue + dim, blue + bold,
		green + dim, cyan + dim, blue + bold,
		green + bold, green + bold, cyan + bold,
		red + dim, magenta + dim, magenta + bold,
		yellow + dim, white + dim, blue + bold,
		green + bold, green + bold, cyan + bold,
		red + bold, red + bold, magenta + bold,
		yellow + dim, red + bold, magenta + bold,
		yellow + bold, yellow + bold, white + bold };
}

inline char ASCIIToHex(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'a')
		return c - 'a' + 10;
	return 0;
}
	
void TelnetHandler::translate(connection& conn, const std::string& buffer)
{
	conn.getHandler()->handle(buffer);
}

void TelnetHandler::sendString(connection& conn, const std::string& string)
{
	const auto str = translateColors(string);
	conn.bufferData(str);
}

std::string TelnetHandler::translateColors(const std::string& str)
{
	auto translatedStr = str;
	auto i = translatedStr.find('<');


	while (i != std::string::npos)
	{
		if (translatedStr[i + 1] == '$') {
			const auto j = translatedStr.find('>', i);
			if (j != std::string::npos) {
				translateStringColor(i, j, translatedStr);
			}
		} else if (translatedStr[i + 1] == '#') {
			const auto j = translatedStr.find('>', i);
			if (j != std::string::npos) {
				translateNumberColor(i, j, translatedStr);
			}
		}
		i = translatedStr.find('<', i + 1);
	}

	return translatedStr;
}

void TelnetHandler::translateStringColor(std::string::size_type i, std::string::size_type j, std::string& str)
{
	const auto col = str.substr(i, j - i + 1);

	if (col == "<$black>")
		str.replace(i, j - i + 1, Colors::black);
	else if (col == "<$red>")
		str.replace(i, j - i + 1, Colors::red);
	else if (col == "<$green>")
		str.replace(i, j - i + 1, Colors::green);
	else if (col == "<$yellow>")
		str.replace(i, j - i + 1, Colors::yellow);
	else if (col == "<$blue>")
		str.replace(i, j - i + 1, Colors::blue);
	else if (col == "<$magenta>")
		str.replace(i, j - i + 1, Colors::magenta);
	else if (col == "<$cyan>")
		str.replace(i, j - i + 1, Colors::cyan);
	else if (col == "<$white>")
		str.replace(i, j - i + 1, Colors::white);
	else if (col == "<$bold>")
		str.replace(i, j - i + 1, Colors::bold);
	else if (col == "<$dim>")
		str.replace(i, j - i + 1, Colors::dim);
	else if (col == "<$reset>")
		str.replace(i, j - i + 1, Colors::reset);
}

void TelnetHandler::translateNumberColor(std::string::size_type i, std::string::size_type j, std::string& str)
{
	if (j - i != 8) {
		std::cerr << "HEX code invalid" << std::endl;
		return;
	}

	auto col = str.substr(i + 2, j - i);
	auto r = ASCIIToHex(col[0]) * 16 + ASCIIToHex(col[1]);
	auto g = ASCIIToHex(col[2]) * 16 + ASCIIToHex(col[3]);
	auto b = ASCIIToHex(col[4]) * 16 + ASCIIToHex(col[5]);

	r = r / 86;
	g = g / 86;
	b = b / 86;

	str.replace(i, j - i + 1, Colors::telnetColors[r][g][b]);
}

}
