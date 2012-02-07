// File:    cells.cpp
// Purpose: Cells class
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

#include "cells.hpp"
#include "string.hpp"

// constructors, assignment, and destructor

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

Cells::operator String(void) const {
    String result;

    result += "{";
    const_iterator gr;
    for (gr = begin(); gr != end(); gr++) {
        if (gr != begin()) {
            result += ", ";
        }
        result += (String)*gr;
    }
    result += "}";

    return result;
}

// misc methods

void Cells::Add(Cell const &cell) {
	ASSERT(!Contains(cell));
	insert(cell);
}

unsigned Cells::Count(void) const {
	unsigned result = size();

	return result;
}

void Cells::MakeEmpty(void) {
	clear();
}

// inquiry methods

bool Cells::AreAllInSameRow(void) const {
    bool result = true;
    
    if (Count() > 1) {
        ConstIteratorType i_cell = begin();
        int row = i_cell->Row();
        for (; i_cell != end(); i_cell++) {
            if (i_cell->Row() != row) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

bool Cells::AreAllInSameColumn(void) const {
    bool result = true;
    
    if (Count() > 1) {
        ConstIteratorType i_cell = begin();
        int column = i_cell->Column();
        for ( ; i_cell != end(); i_cell++) {
            if (i_cell->Column() != column) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

bool Cells::Contains(Cell const &rCell) const {
	ConstIteratorType i_cell = find(rCell);
    bool result = (i_cell != end());
    
    return result;
}
