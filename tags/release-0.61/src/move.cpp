// File:    move.cpp
// Purpose: Move class
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

#include <iostream>
#include "cells.hpp"
#include "move.hpp"
#include "string.hpp"
#include "strings.hpp"
#include "tile.hpp"
#include "tiles.hpp"

// lifecycle

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.

// operators

// The compiler-generated assignment operator is fine.

Move::operator Cells(void) const {
    Cells result;
    
    ConstIteratorType i_tile_cell;
    for (i_tile_cell = begin(); i_tile_cell != end(); i_tile_cell++) {
		if (!i_tile_cell->IsSwap()) {
            Cell cell = Cell(*i_tile_cell);
            result.insert(cell);
		}
    }

    return result;
}

Move::operator String(void) const {
	String result;

    result += "{";
    const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        if (ts != begin()) {
            result += ", ";
        } 
        result += (String)*ts;
    }       
    result += "}";

    return result;
}

Move::operator Tiles(void) const {
    Tiles result;
    
    ConstIteratorType i_tile_cell;
    for (i_tile_cell = begin(); i_tile_cell != end(); i_tile_cell++) {
        TileCell tc = *i_tile_cell;
        Tile tile = tc.operator Tile();
        result.Add(tile);
    }

    return result;
}

// misc

void Move::Add(Tile const &rTile) {
    TileCell tile_cell(rTile);
    insert(tile_cell);
}

void Move::Add(Tile const &rTile, Cell const &rCell) {
    TileCell tile_cell(rTile, rCell);
    insert(tile_cell);
}

unsigned Move::Count(void) const {
    unsigned result = size();

	return result;
}

void Move::GetUserChoice(Tiles const &rAvailableTiles) {
    MakeEmpty();

    while (true) {
		Strings alts;
		if (IsPass()) {
			alts.Append("pass");
		} else {
			alts.Append("move");
		}

		TileCell tile_cell;
		String input = tile_cell.GetUserChoice(rAvailableTiles, alts);
		D(std::cout << "input=\'" << input << "'" << std::endl);
        if (input == "pass" || input == "move") {
            break;
        }

        insert(tile_cell);
    }
}


void Move::MakeEmpty(void) {
	clear();
}

// inquiries

bool Move::InvolvesSwap(void) const {
    bool result = false;

	ConstIteratorType i_tile;
	for (i_tile = begin(); i_tile != end(); i_tile++) {
		if (i_tile->IsSwap()) {
			result = true;
			break;
		}
	}

	return result;
}

bool Move::IsPass(void) const {
	bool result = (Count() == 0);

	return result;
}

bool Move::IsPureSwap(void) const {
	bool result = true;

    ConstIteratorType i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
        if (!i_tile->IsSwap()) {
        	result = false;
			break;
		}
	}

	return result;
}

bool Move::RepeatsTile(void) const {
    bool result = false;
    
    if (Count() > 1) {
        Tiles tiles_seen;
        ConstIteratorType i_tile;
        
        for (i_tile = begin(); i_tile != end(); i_tile++) {
            Tile tile = i_tile->operator Tile();
            if (tiles_seen.Contains(tile)) {
                result = true;
                break;
            } else {
                tiles_seen.Add(tile);
            }
        }
    }
    
    return result;
}

bool Move::RepeatsCell(void) const {
    bool result = false;
    
    if (Count() > 1) {
        Cells cells_seen;
        ConstIteratorType i_tile;
        
        for (i_tile = begin(); i_tile != end(); i_tile++) {
    		if (!i_tile->IsSwap()) {
                Cell cell = Cell(*i_tile);
                if (cells_seen.Contains(cell)) {
                    result = true;
                    break;
                } else {
                    cells_seen.Add(cell);
                }
			}
        }
    }
     
    return result;
}
