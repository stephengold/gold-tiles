#ifndef STRINGS_HPP_INCLUDED
#define STRINGS_HPP_INCLUDED

// File:    strings.hpp
// Purpose: Strings class
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
A Strings object represents an ordered collection of unique text strings.

The Strings class encapsulates a list of String objects.
*/

#include <list>
#include "string.hpp"

class Strings {
public:
    typedef std::list<String>::const_iterator ConstIteratorType;

	// misc public methods
	void              Append(String const &);
	ConstIteratorType Begin(void) const;
	ConstIteratorType End(void) const;
	ConstIteratorType Find(String const &) const;

	// public inquiry methods
	bool Contains(String const &) const;

private:
	std::list<String> mList;
};

#endif
