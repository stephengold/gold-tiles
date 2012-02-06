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

// constructors, assignment, destructor, and stringification

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

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

// methods

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

Cells Move::GetCells(void) const {
    Cells result;
    
    const_iterator i_tile_cell;
    for (i_tile_cell = begin(); i_tile_cell != end(); i_tile_cell++) {
		if (!i_tile_cell->IsSwap()) {
            Cell cell = Cell(*i_tile_cell);
            result.insert(cell);
		}
    }

    return result;
}

Tiles Move::GetTiles(void) const {
    Tiles result;
    
    const_iterator i_tile_cell;
    for (i_tile_cell = begin(); i_tile_cell != end(); i_tile_cell++) {
        Tile tile = Tile(*i_tile_cell);
        result.insert(tile);
    }

    return result;
}

void Move::GetUserChoice(Tiles const &rAvailableTiles) {
    MakeEmpty();

    while (true) {
		Strings alts;
		if (IsPass()) {
			alts.Append("pass");
		} else {
			alts.Append("go");
		}

		TileCell tile_cell;
		String input = tile_cell.GetUserChoice(rAvailableTiles, alts);
		D(std::cout << "input=\'" << input << "'" << std::endl);
        if (input == "pass" || input == "go") {
            break;
        }

        insert(tile_cell);
    }
}

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

void Move::MakeEmpty(void) {
	clear();
}

bool Move::RepeatsTile(void) const {
    bool result = false;
    
    if (Count() > 1) {
        Tiles tiles_so_far;
        ConstIteratorType i_tile;
        
        for (i_tile = begin(); i_tile != end(); i_tile++) {
            Tile tile = Tile(*i_tile);
            if (tiles_so_far.Contains(tile)) {
                result = true;
                break;
            } else {
                tiles_so_far.AddTile(tile);
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