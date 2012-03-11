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

#include <locale>
#include <sstream>
#include "string.hpp"

static std::string itos(int integer) {
   std::ostringstream sout;
   sout << integer;
   return sout.str(); 
}
static std::string utos(unsigned integer) {
   std::ostringstream sout;
   sout << integer;
   return sout.str(); 
}
static std::string ltos(long integer) {
   std::ostringstream sout;
   sout << integer;
   return sout.str(); 
}
static std::string ultos(unsigned long integer) {
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
String::String(unsigned integer):
	std::string(::utos(integer)) {}
String::String(long integer):
	std::string(::ltos(integer)) {}
String::String(unsigned long integer):
	std::string(::ultos(integer)) {}

// operators

String::operator char const *(void) const {
	char const *result = c_str();

	return result;
}

String::operator int(void) const {
    char const *text = c_str();
    int result = ::atoi(text);

	return result;
}

// misc methods

// capitalize the first letter of each word
// trim leading and trailing non-graphic characters
// convert adjacent non-graphic characters a single space
void String::Capitalize(void) {
	bool after_graphic = false;
	String result;
	char const space = ' ';

	for (unsigned i_char = 0; i_char < Length(); i_char++) {
        char ch = at(i_char);
		bool is_graphic = (::isgraph(ch) != 0);

		if (!is_graphic) {
			ch = space;
		} else if (!after_graphic) { // first letter of a word
		    ch = char(::toupper(ch));
		}

		if (is_graphic || after_graphic) {
			result += ch;
		}

		after_graphic = is_graphic;
	}

	*this = result;
	unsigned len = Length();
	if (len > 0 && at(len - 1) == space) {  // trailing non-graphic character
		erase(len - 1, 1);  // trim it
	}
}

unsigned String::Length(void) const {
	unsigned result = size();

	return result;
}

// inquiry methods

bool String::IsEmpty(void) const {
	bool result = (Length() == 0);

	return result;
}
