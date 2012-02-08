#ifndef INDICES_HPP_INCLUDED
#define INDICES_HPP_INCLUDED

// File:    indices.hpp
// Purpose: Indexes class
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
An Indices object represents a set of indices, which may be rows or 
columns in a grid or tile IDs.

The Indices class is implemented as a set of integers.
*/

#include <set>

class Indices: public std::set<long> {
public:
	typedef std::set<long>::const_iterator ConstIteratorType;
	typedef std::set<long>::iterator IteratorType;

	// misc
	void     Add(long);
	void     Remove(long);

	// access
    unsigned Count(void) const;

	// inquiry
    bool     Contains(long) const;
	bool     IsEmpty(long) const;
};

#endif
