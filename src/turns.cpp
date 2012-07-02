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


// static constants

const String Turns::PREFIX("turns{");
const String Turns::SEPARATOR(" ");
const String Turns::SUFFIX("}");


// lifecycle

// The implicitly defined default constructor is OK.
// The implicitly defined copy constructor is OK.
// The implicitly defined destructor is OK.


// operators

// The implicitly defined assignment method is OK.

// Convert to a string for save/send.
Turns::operator String(void) const {
    String result(PREFIX);

    ConstIterator i_turn;
    for (i_turn = begin(); i_turn != end(); i_turn++) {
        if (i_turn != begin()) {
            result += SEPARATOR;
        }

        Turn const turn = *i_turn;
        String const turn_string = turn;
        result += turn_string;
    }       
    result += SUFFIX;

    return result;
}


// misc methods

SizeType Turns::Count(void) const {
    SizeType const result = SizeType(size());

    return result;
}

// convert an iterator to an index
SizeType Turns::Index(Iterator const& rIterator) const {
    // count the number of iterator values preceding the argument
    SizeType result = 0;
    ConstIterator iterator;
    for (iterator = begin(); iterator != rIterator; iterator++) {
        ASSERT(iterator != end());
        result++;
    }

    return result;
}

SizeType Turns::IndexLastPlay(void) const {
    SizeType result = Count();

    for (ConstReverseIterator i_turn = rbegin(); i_turn != rend(); i_turn++) {
        Move const move = *i_turn;
        if (move.IsPlay()) {
            break;
        }
        result--;
    }

    return result;
}

