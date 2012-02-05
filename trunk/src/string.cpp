// File:    string.cpp
// Purpose: implement String class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <sstream>
#include "string.hpp"

String::String(void) {
    std::string("");
}

String::String(char ch) {
	std::string(1, ch);
}

String::String(unsigned repeat, char ch) {
	std::string(repeat, ch);
}

String::String(std::string const &str):  std::string(str) {}

String::String(char const p_ch[]): std::string(p_ch) {}

String::String(int integer) {
   std::ostringstream sout;
   sout << integer;
   std::string(sout.str());
}

String::operator int(void) const {
	unsigned n = Length();

    char *buf = new char[n + 1];
    for (unsigned i = 0; i < n; i++) {
        buf[i] = (*this)[i];
    }
    buf[n] = '\0';

    int result = ::atoi(buf);
    delete[] buf;
    return result;
}

// methods

unsigned String::Length(void) const {
	unsigned result = size();

	return result;
}