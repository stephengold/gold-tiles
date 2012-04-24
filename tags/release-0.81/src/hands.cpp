// File:     hands.cpp
// Location: src
// Purpose:  implement Hands class
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

#include "hands.hpp"


// misc methods

void Hands::Append(Hand const& rHand) {
	push_back(rHand);

	ASSERT(!IsEmpty());
}

unsigned Hands::Count(void) const {
    unsigned const result = size();

	return result;
}

Hands::Iterator Hands::Find(String const& rName) {
	ASSERT(!rName.IsEmpty());

	Iterator i_hand;
	for (i_hand = begin(); i_hand != end(); i_hand++) {
		if (i_hand->Name() == rName) {
			return i_hand;
		}
	}

    FAIL(); // not found
	return i_hand;
}

ScoreType Hands::MaxScore(void) const {
	ASSERT(!IsEmpty());

	ScoreType result = 0;

	ConstIterator i_hand;
	for (i_hand = begin(); i_hand != end(); i_hand++) {
		ScoreType const score = i_hand->Score();
		if (score > result) {
			result = score;
		}
	}

	return result;
}

void Hands::Next(ConstIterator& riCurrent) const {
	// advance const iterator to next hand
	riCurrent++;
    if (riCurrent >= end()) {
        riCurrent = begin();
    }
}

void Hands::Next(Iterator& riCurrent) {
	// advance iterator to next hand
	riCurrent++;
    if (riCurrent >= end()) {
        riCurrent = begin();
    }
}

void Hands::NextWorking(Iterator& riCurrent) {
	// advance iterator to next hand which has not resigned
	Iterator const start = riCurrent;
	Next(riCurrent);

	while (riCurrent->HasResigned() && riCurrent != start) {
		Next(riCurrent);
	}
}

void Hands::Previous(ConstIterator& riCurrent) const {
	// advance const iterator to next hand
    if (riCurrent == begin()) {
        riCurrent = end();
    }
	riCurrent--;
}

void Hands::Previous(Iterator& riCurrent) {
	// advance iterator to next hand
    if (riCurrent == begin()) {
        riCurrent = end();
    }
	riCurrent--;
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

bool Hands::IsEmpty(void) const {
	bool result = (Count() == 0);

	return result;
}

