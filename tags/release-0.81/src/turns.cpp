// File:     turns.cpp
// Location: src
// Purpose:  implement Turns class
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

#include "turns.hpp"


// lifecycle

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.


// misc methods

unsigned Turns::Count(void) const {
	unsigned const result = size();

	return result;
}

// convert an iterator to an index
unsigned Turns::Index(Iterator const& rIterator) const {

	// count the number of iterator values preceding the argument
	unsigned result = 0;
	ConstIterator iterator;
	for (iterator = begin(); iterator != rIterator; iterator++) {
		ASSERT(iterator != end());
		result++;
	}

	return result;
}

unsigned Turns::IndexLastPlay(void) const {
	unsigned result = Count();

	for (ConstReverseIterator i_turn = rbegin(); i_turn != rend(); i_turn++) {
		Move const move = *i_turn;
		if (move.IsPlay()) {
			break;
		}
		result--;
	}

	return result;
}
