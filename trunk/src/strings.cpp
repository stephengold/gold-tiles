// File:    strings.cpp
// Purpose: Strings class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "strings.hpp"

// constructors, assignment, and destructor

// The compiler.generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated assignment method is OK.
// The compiler-generated destructor is OK.

// methods

void Strings::Append(String const &str) {
    mList.push_back(str);
}

Strings::ConstIteratorType Strings::Begin(void) const {
    ConstIteratorType result;
	result = mList.begin();

	return result;
}

Strings::ConstIteratorType Strings::End(void) const {
    ConstIteratorType result;
	result = mList.end();

	return result;
}

Strings::ConstIteratorType Strings::Find(String const &string) const {
    ConstIteratorType result;
	for (result = Begin(); result != End(); result++) {
		if (*result == string) {
			break;
		}
	}

	return result;
}
