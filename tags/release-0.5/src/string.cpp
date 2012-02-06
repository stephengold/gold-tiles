// File:    string.cpp
// Purpose: String class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include "string.hpp"

static std::string itos(int integer) {
   std::ostringstream sout;
   sout << integer;
   return sout.str(); 
}

String::String(void):
    std::string() {}
String::String(char ch):
	std::string(1, ch) {}
String::String(unsigned repeat, char ch):
	std::string(repeat, ch) {}
String::String(std::string const &str):
	std::string(str) {}
String::String(char const p_ch[]):
	std::string(p_ch) {}
String::String(int integer):
	std::string(::itos(integer)) {}

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