#ifndef STRING_HPP_INCLUDED
#define STRING_HPP_INCLUDED

// File:    string.hpp
// Purpose: String class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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