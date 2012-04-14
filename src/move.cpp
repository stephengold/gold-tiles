// File:     move.cpp
// Location: src
// Purpose:  implement Move class
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

#include <iostream>
#include "cells.hpp"
#include "move.hpp"
#include "strings.hpp"
#include "tiles.hpp"


// lifecycle

Move::Move(void):
    mSet()
{
	mResignFlag = false;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment operator is fine.

Move::operator Cells(void) const {
    Cells result;
    
    ConstIterator i_tile_cell;
    for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
		if (!i_tile_cell->IsSwap()) {
            Cell const cell = Cell(*i_tile_cell);
            result.insert(cell);
		}
    }

    return result;
}

Move::operator String(void) const {
	String result("{");

	if (mResignFlag) {
		result += "RESIGN ";
	}

    ConstIterator i_tile_cell;
    for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
        if (i_tile_cell != Begin()) {
            result += ", ";
        } 
        result += String(*i_tile_cell);
	}
    result += "}";

    return result;
}

Move::operator Tiles(void) const {
    Tiles result;
    
    ConstIterator i_tile_cell;
    for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
        TileCell const tc = *i_tile_cell;
        Tile tile = tc.operator Tile();  // TODO
        result.Add(tile);
    }

    return result;
}


// misc methods

void Move::Add(Tile const& rTile) {
	// add a tile swap
    TileCell const tile_cell(rTile);
    mSet.insert(tile_cell);
}

void Move::Add(Tile const& rTile, Cell const& rCell) {
	// add a tile played to the board
    TileCell const tile_cell(rTile, rCell);
    mSet.insert(tile_cell);
}

Move::ConstIterator Move::Begin(void) const {
	ConstIterator const result = mSet.begin();

	return result;
}

unsigned Move::Count(void) const {
    unsigned const result = mSet.size();

	return result;
}

unsigned Move::CountTilesPlayed(void) const {
    unsigned result = 0;

	if (!InvolvesSwap()) {
		result = Count();
	}

	return result;
}

Move::ConstIterator Move::End(void) const {
	ConstIterator const result = mSet.end();

	return result;
}

void Move::GetUserChoice(Tiles const& rAvailableTiles) {
    MakePass();

    for (;;) {
		Strings alts;
		if (IsPass()) {
			alts.Append("resign");
			alts.Append("pass");
		} else {
			alts.Append("move");
		}

		TileCell tile_cell;
		String input = tile_cell.GetUserChoice(rAvailableTiles, alts);
        if (input == "resign") {
			MakeResign(rAvailableTiles);
            break;
        } else if (input == "pass" || input == "move") {
            break;
        }

        mSet.insert(tile_cell);
    }
}

void Move::MakePass(void) {
	mSet.clear();
	mResignFlag = false;
}

void Move::MakeResign(Tiles const& rTiles) {
	mSet.clear();
	for (unsigned i_tile = 0; i_tile < rTiles.Count(); i_tile++) {
		Tile const tile = rTiles[i_tile];
	    Add(tile);
	}

	mResignFlag = true;
}


// inquiry methods

bool Move::InvolvesSwap(void) const {
    bool result = false;

	ConstIterator i_tile_cell;
	for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
		if (i_tile_cell->IsSwap()) {
			result = true;
			break;
		}
	}

	return result;
}

bool Move::IsPass(void) const {
	bool const result = (Count() == 0) && !mResignFlag;

	return result;
}

bool Move::IsPureSwap(void) const {
	bool result = !mResignFlag;

	if (result) {
		// every tile in the move must also get swapped
        ConstIterator i_tile_cell;
        for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
            if (!i_tile_cell->IsSwap()) {
        	    result = false;
			    break;
		    }
		}
	}

	return result;
}

bool Move::IsResign(void) const {
	return mResignFlag;
}

bool Move::IsPlay(void) const {
	bool const result = (Count() > 0) && !mResignFlag && !InvolvesSwap();

	return result;
}

bool Move::RepeatsTile(void) const {
    bool result = false;
    
    if (Count() > 1) {
        Indices tiles_seen;
        ConstIterator i_tile_cell;
        
        for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
            Tile const tile = i_tile_cell->operator Tile();   // TODO
			TileIdType const id = tile.Id();
            if (tiles_seen.Contains(id)) {
                result = true;
                break;
            } else {
                tiles_seen.Add(id);
            }
        }
    }
    
    return result;
}

bool Move::RepeatsCell(void) const {
    bool result = false;
    
    if (Count() > 1) {
        Cells cells_seen;
        ConstIterator i_tile_cell;
        
        for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
    		if (!i_tile_cell->IsSwap()) {
                Cell const cell = Cell(*i_tile_cell);
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
