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
#include "strings.hpp"

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


// lifecycle

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

String::String(Strings const &rList, String const &rSeparator) {
	Strings::ConstIterator i_string;
	for (i_string = rList.Begin(); i_string != rList.End(); i_string++) {
		if (i_string != rList.Begin()) {
			*this += rSeparator;
		}
		*this += *i_string;
	}
}


// operators

String::operator TextType(void) const {
	TextType const result = c_str();

	return result;
}

String::operator int(void) const {
    TextType const text = c_str();
    int const result = ::atoi(text);

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
		bool const is_graphic = (::isgraph(ch) != 0);

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
	unsigned const len = Length();
	if (len > 0 && at(len - 1) == space) {  // trailing non-graphic character
		erase(len - 1, 1);  // trim it
	}
}

unsigned String::Length(void) const {
	unsigned const result = size();

	return result;
}

// purge non-graphic characters
String String::Purge(void) const {
	String result;

	for (unsigned i_char = 0; i_char < Length(); i_char++) {
        char const ch = at(i_char);
		bool const is_graphic = (::isgraph(ch) != 0);

		if (is_graphic) {
			result += ch;
		}
	}

	return result;
}

// enclose the string in single quotes
String String::Quote(void) const {
	String const result("'" + *this + "'");

	return result;
}


// inquiry methods

bool String::IsEmpty(void) const {
	bool const result = (Length() == 0);

	return result;
}
