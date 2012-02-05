// File:    cells.cpp
// Purpose: Cells class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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

// public methods

void Cells::Add(Cell const &cell) {
	ASSERT(!Contains(cell));
	insert(cell);
}

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

unsigned Cells::Count(void) const {
	unsigned result = size();

	return result;
}