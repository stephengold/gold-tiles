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

#include <string>  //  ISA std::string

#ifndef __STDC_WANT_SECURE_LIB__
// for envs that don't provide strcpy_s()
#define strcpy_s(a, sz, b) strcpy(a, b)
#endif

class String: public std::string {
public:
	// public lifecycle
    String(void);
	String(char);
	String(unsigned, char);
	String(char const []);
	String(int); // itoa
	String(unsigned); // itoa
	String(long); // itoa
	String(unsigned long); // itoa
	String(std::string const &);
	// String(String const &);  compiler-generated copy constructor is OK
    // ~String(void);  compiler-generated destructor is OK

	// public operators
    // String &operator=(String const &);  compiler-generated assignment method is OK
	operator char const*(void) const;
	operator int(void) const; // atoi

	// misc public methods
	void Capitalize(void);
	unsigned Length(void) const;

	// public inquiry methods
	bool IsBlank(void) const;
	bool IsEmpty(void) const;
};

#endif // !defined(STRING_HPP_INCLUDED)
