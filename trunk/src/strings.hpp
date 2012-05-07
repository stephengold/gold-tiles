#ifndef STRINGS_HPP_INCLUDED
#define STRINGS_HPP_INCLUDED

// File:     strings.hpp
// Location: src
// Purpose:  declare Strings class
// Author:   Stephen Gold sgold@sonic.net
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
A Strings object represents an ordered collection of unique text strings.

The Strings class encapsulates a list of String objects, implemented
using the Standard Template Library.
*/

#include <list>        // HASA std::list
#include "string.hpp"  // HASA String


class Strings {
public:
	// public types
    typedef std::list<String>::const_iterator ConstIterator;
    typedef std::list<String>::iterator       Iterator;

	// public lifecycle
	Strings();
	// Strings(Strings const&);  compiler-generated copy constructor is OK
	Strings(TextType list, TextType delimiters); // split
    // ~Strings(void);  compiler-generated destructor is OK

	// public operators
    // Strings& operator=(Strings const&);  compiler-generated assignment operator is OK

	// misc public methods
	void          Append(String const&);
	ConstIterator Begin(void) const;
	Iterator      Begin(void);
	unsigned      Count(void) const;
	unsigned      Count(String const&) const;
	ConstIterator End(void) const;
	Iterator      End(void);
	ConstIterator Find(String const&) const;
	String        First(void) const;
	String        InventUnique(String const& prefix) const;
	String        InventUnique(String const& prefix, String const& infix, 
		              String const& suffix) const;
	void          MakeEmpty(void);
	String        Second(void) const;
	Strings       Unique(void) const;
	void          Unappend(void);

	// public inquiry methods
	bool Contains(String const&) const;
	bool IsEmpty(void) const;

private:
    // private data
	std::list<String> mList;
};
#endif // !defined(STRINGS_HPP_INCLUDED)
