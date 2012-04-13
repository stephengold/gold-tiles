// File:     cells.cpp
// Location: src
// Purpose:  implement Cells class
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

#include "cells.hpp"
#include "direction.hpp"


// lifecycle

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

#if 0
Cells::operator String(void) const {
    String result("{");

	ConstIterator gr;
    for (gr = begin(); gr != end(); gr++) {
        if (gr != begin()) {
            result += ", ";
        }
        result += String(*gr);
    }
    result += "}";

    return result;
}
#endif

// The compiler-generated assignment method is fine.


// misc methods

void Cells::Add(Cell const &cell) {
	ASSERT(!Contains(cell));

	insert(cell);

	ASSERT(Contains(cell));
}

unsigned Cells::Count(void) const {
	unsigned const result = size();

	return result;
}

void Cells::MakeEmpty(void) {
	clear();
	
	ASSERT(IsEmpty());
}


// inquiry methods

bool Cells::AreAllInSameOrtho(Direction const &rDirection) const {
    bool result = true;
    
    if (Count() > 1) {
        ConstIterator i_cell = begin();
        IndexType const ortho = i_cell->Ortho(rDirection);
        for (i_cell++; i_cell != end(); i_cell++) {
            if (i_cell->Ortho(rDirection) != ortho) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

bool Cells::Contains(Cell const &rCell) const {
	ConstIterator const i_cell = find(rCell);
    bool const result = (i_cell != end());
    
    return result;
}

bool Cells::IsAnyStart(void) const {
    bool result = false;
    
    for (ConstIterator i_cell = begin(); i_cell != end(); i_cell++) {
        if (i_cell->IsStart()) {
            result = true;
            break;
        }
    }
    
    return result;
}

bool Cells::IsEmpty(void) const {
	bool const result = (size() == 0);

	return result;
}
