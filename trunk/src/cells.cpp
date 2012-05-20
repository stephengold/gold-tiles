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

Cells::Cells(void) {
}

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.

Cells::Cells(Cell const& rCell) {
    Add(rCell);
}


// operators

#if 0
Cells::operator String(void) const {
    String result("{");

    ConstIterator i_cell;
    for (i_cell = begin(); i_cell != end(); i_cell++) {
        if (i_cell != begin()) {
            result += ", ";
        }
        result += String(*i_cell);
    }
    result += "}";

    return result;
}
#endif

// The implicitly defined assignment method is fine.


// misc methods

void Cells::Add(Cell const& rCell) {
    ASSERT(!Contains(rCell));

    insert(rCell);

    ASSERT(Contains(rCell));
}

void Cells::AddCells(Cells const& rCells) {
    ConstIterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        Cell const cell = *i_cell;
        Add(cell);
    }
}

SizeType Cells::Count(void) const {
    SizeType const result = SizeType(size());

    return result;
}

Cell Cells::First(void) const {
    ASSERT(!IsEmpty());

    ConstIterator const i_cell = begin();
    ASSERT(i_cell != end());
    Cell const result = *i_cell;

    return result;
}

void Cells::MakeEmpty(void) {
    clear();

    ASSERT(IsEmpty());
}


// inquiry methods

bool Cells::Contains(Cell const& rCell) const {
    ConstIterator const i_cell = find(rCell);
    bool const result = (i_cell != end());

    return result;
}

bool Cells::ContainsAll(Cells const& rCells) const {
    bool result = true;

    for (ConstIterator i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        Cell const cell = *i_cell;
        if (!Contains(cell)) {
            result = false;
            break;
        }
    }

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
