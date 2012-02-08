#ifndef STRING_HPP_INCLUDED
#define STRING_HPP_INCLUDED

// File:    string.hpp
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

/*
A String object represents a text string (sequence of characters).
*/

#include <string>

class String: public std::string {
public:
	// lifecycle
    String(void);
	String(char);
	String(unsigned, char);
	String(char const []);
	String(int); // itoa
	String(std::string const &);
	// String(String const &);  compiler-generated copy constructor is OK
    // ~String(void);  compiler-generated destructor is OK

	// operators
    // String &operator=(String const &)};  compiler-generated assignment method is OK
	operator int(void) const; // atoi

	// access
	unsigned Length(void) const;
};

#endif
