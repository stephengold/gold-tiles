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


// static constants
const String Move::PREFIX("move{");
const String Move::RESIGN("resign");
const String Move::SEPARATOR(" ");
const String Move::SUFFIX("}");


// lifecycle

Move::Move(void) {
	mResignFlag = false;
}

// The compiler-generated copy constructor is fine.

// Parse a save/send string.
Move::Move(String const& rString, bool remoteFlag) {
	bool const has_prefix = rString.HasPrefix(PREFIX);
	bool const has_suffix = rString.HasSuffix(SUFFIX);
	if (!has_prefix || !has_suffix) {
		FAIL(); // TODO recovery
	}
	String const body = rString.Suffix(PREFIX).Prefix(SUFFIX);

	Strings const words(body, SEPARATOR);
	mResignFlag = false;

	Strings::ConstIterator i_word;
	for (i_word = words.Begin(); i_word != words.End(); i_word++) {
		String const word = *i_word;
		if (word != RESIGN) {
		    TileCell const tile_cell(word, remoteFlag);
            Add(tile_cell);
		} else {
			mResignFlag = true;
		}
	}
}

// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment operator is fine.

bool Move::operator!=(Move const& rOther) const {
	bool result = (mResignFlag != rOther.mResignFlag);

    if (!result) {
	    result = (Count() != rOther.Count());
    }
    if (!result) {
        ConstIterator i_this;
        ConstIterator i_other = rOther.Begin();
        for (i_this = Begin(); i_this != End(); i_this++) {
            if (*i_this != *i_other) {
			    result = true;
			    break;
            }
			i_other++;
		}
	}

    return result;
}

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
	String result(PREFIX);

	if (mResignFlag) {
		result += RESIGN + SEPARATOR;
	}

    ConstIterator i_tile_cell;
    for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
        if (i_tile_cell != Begin()) {
            result += SEPARATOR;
        } 
        result += String(*i_tile_cell);
	}
    result += SUFFIX;

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

void Move::Add(TileCell const& rTileCell) {
    mSet.insert(rTileCell);
}

void Move::Add(Tile const& rTile, Cell const& rCell) {
	// add a tile played to the board
    TileCell const tile_cell(rTile, rCell);
    Add(tile_cell);
}

void Move::AddSwapTile(Tile const& rTile) {
    TileCell const tile_cell(rTile);
    Add(tile_cell);
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

String Move::Description(void) const {
	String result;

	if (mResignFlag) {
		result = "resigned with " + ::plural(Count(), "tile");
	} else if (InvolvesSwap()) {
		result = "swapped " + ::plural(Count(), "tile");
	} else if (IsPass()) {
		result = "passed";
	} else {
		result = "played ";

        ConstIterator i_tile_cell;
        for (i_tile_cell = Begin(); i_tile_cell != End(); i_tile_cell++) {
            if (i_tile_cell != Begin()) {
                result += ", ";
            }
			TileCell const tile_cell = *i_tile_cell;
            result += tile_cell.Description();
	    }
	}

    return result;
}

Move::ConstIterator Move::End(void) const {
	ConstIterator const result = mSet.end();

	return result;
}

void Move::GetUserChoice(Tiles const& rAvailableTiles, unsigned mustPlay) {
    MakePass();

    for (;;) {
		Strings alts;
		if (IsPass()) {
			alts.Append("resign");
			if (mustPlay == 0) {
			    alts.Append("pass");
			}
		} else if (Count() >= mustPlay) {
			alts.Append("move");
		}

		TileCell tile_cell;
		String const input = tile_cell.GetUserChoice(rAvailableTiles, alts);
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
	Tiles::ConstIterator i_tile;
	for (i_tile = rTiles.begin(); i_tile != rTiles.end(); i_tile++) {
		Tile const tile = *i_tile;
	    AddSwapTile(tile);
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
			Tile::IdType const id = tile.Id();
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
