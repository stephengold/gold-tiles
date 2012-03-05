// File:    hands.cpp
// Purpose: Hands class
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

#include "hands.hpp"


// misc methods

unsigned Hands::Count(void) const {
    unsigned result = size();

	return result;
}

void Hands::Next(ConstIterator &riCurrent) const {
	// advance iterator to next hand
	riCurrent++;
    if (riCurrent >= end()) {
        riCurrent = begin();
    }
}

void Hands::Next(Iterator &riCurrent) {
	// advance iterator to next hand
	riCurrent++;
    if (riCurrent >= end()) {
        riCurrent = begin();
    }
}

void Hands::NextWorking(Iterator &riCurrent) {
	// advance iterator to next hand which has not resigned
	Iterator start = riCurrent;
	Next(riCurrent);

	while (riCurrent->HasResigned() && riCurrent != start) {
		Next(riCurrent);
	}
}


// inquiry methods

bool Hands::HasAnyGoneOut(void) const {
   bool result = false;

   ConstIterator i_hand;
   for (i_hand = begin(); i_hand != end(); i_hand++) {
	   if (i_hand->HasGoneOut()) {
		   result = true;
		   break;
	   }
   }

   return result;
}

bool Hands::HaveAllResigned(void) const {
	bool result = true;

	ConstIterator i_hand;
    for (i_hand = begin(); i_hand != end(); i_hand++) {
	    if (!i_hand->HasResigned()) {
		    result = false;
		    break;
	    }
    }

	return result;
}
