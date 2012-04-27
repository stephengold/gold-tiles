// File:     tiles.cpp
// Location: src
// Purpose:  implement Tiles class
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
#include "strings.hpp"
#include "tiles.hpp"


// static constants

const String Tiles::PREFIX("tiles{");
const String Tiles::SEPARATOR(" ");
const String Tiles::SUFFIX("}");


// lifecycle

// Construct an empty container.
Tiles::Tiles(void) {
}

// Copy and validate indices.
Tiles::Tiles(Indices const& rIndices, bool remoteFlag) {
	ConstIterator i_index;
    for (i_index = rIndices.begin(); i_index != rIndices.end(); i_index++) {
	   IndexType const index = *i_index;
	   Tile const tile = Tile(index, remoteFlag); // validation happens here
	   Add(tile);
    }
}

// Parse a save/send string.
Tiles::Tiles(String const& rString, bool remoteFlag) {
	bool const has_prefix = rString.HasPrefix(PREFIX);
	bool const has_suffix = rString.HasSuffix(SUFFIX);
	if (!has_prefix || !has_suffix) {
		FAIL(); // TODO recovery
	}
	String const body = rString.Suffix(PREFIX).Prefix(SUFFIX);

	Strings const words(body, SEPARATOR);

	Strings::ConstIterator i_word;
	for (i_word = words.Begin(); i_word != words.End(); i_word++) {
		String const word = Tile(*i_word, remoteFlag);
		Tile const tile(word);
        Add(tile);
	}
}

// Construct a container for a single Tile.
Tiles::Tiles(Tile const& rTile) {
    Add(rTile);
}


// operators

// Convert to a string for save/send.
Tiles::operator String(void) const {
    String result(PREFIX);

    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
        if (i_tile != begin()) {
            result += SEPARATOR;
        }

		Tile const tile = *i_tile;
        result += String(tile);
    }       
    result += SUFFIX;

    return result;
}


// misc methods

void Tiles::Add(Tile::IdType id) {
	ASSERT(Tile::IsValid(id));

	Indices::Add(id);
}

// generate tiles for the stock bag - RECURSIVE
void Tiles::AddAllTiles(AttrIndexType attributeIndex, Tile& rModelTile) {
    AttrCntType const na = Combo::AttributeCnt();
	if (attributeIndex < na) {
		AttrType const max = Combo::ValueMax(attributeIndex);
		for (AttrType attr = 0; attr <= max; attr++) {
        	rModelTile.SetAttribute(attributeIndex, attr);
	        AddAllTiles(attributeIndex + 1, rModelTile);
         }
	} else {
        ASSERT(attributeIndex == na);
        Tile const clone = rModelTile.CloneAndSetBonus();
		Add(clone);
	}
}

unsigned Tiles::BonusFactor(void) const {
	unsigned long factor = 1;

    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
		Tile const tile = *i_tile;
		if (tile.HasBonus()) {
			factor *= 2;
		}
	}

	ASSERT(factor > 0);
	ASSERT(factor <= UINT_MAX);
	unsigned result = unsigned(factor);

	return result;
}

// construct "runs":  subsets of mutually-compatible tiles - RECURSIVE
void Tiles::BuildRuns(Tiles const& rRunSoFar, Tiles& rLongestRun) const {
	if (IsEmpty()) {
		if (rRunSoFar.Count() > rLongestRun.Count()) {
			rLongestRun = rRunSoFar;
		}
	} else {
		Tiles run(rRunSoFar);
	    Tiles remainder(*this);
	    Tile const first = remainder.PullFirstTile();

		// build runs without the first tile
		remainder.BuildRuns(run, rLongestRun);

		if (run.AreAllCompatibleWith(first)) {
		   // build runs with the first tile
		   run.Add(first);
	       remainder.BuildRuns(run, rLongestRun);
		}
	}
}

AttrIndexType Tiles::CommonAttribute(void) const {
	ASSERT(AreAllCompatible());
	ASSERT(Count() > 1);
	ASSERT(Combo::AttributeCnt() == 2);

    ConstIterator i_tile = begin();
    ASSERT(i_tile != end());
    Tile const first_tile = *i_tile;

	i_tile++;
    ASSERT(i_tile != end());
    Tile const second_tile = *i_tile;

	AttrIndexType const result = first_tile.CommonAttribute(second_tile);

	return result;
}

String Tiles::Description(void) const {
    String result("{");

    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
        if (i_tile != begin()) {
			// append a separator
            result += ", ";
        }

		Tile const tile = *i_tile;
		String const string = tile.Description();
        result += string;
    }       
    result += "}";

    return result;
}

// return the largest subset of mutually compatible tiles
Tiles Tiles::LongestRun(void) const {
	// clones are never compatible, so consider only the unique tiles
	Tiles const unique = UniqueTiles();

	Tiles result;
    Tiles const empty_run;
	unique.BuildRuns(empty_run, result);
    
	ASSERT(result.AreAllCompatible());
	return result;
}

Tile Tiles::PullFirstTile(void) {
	ASSERT(!IsEmpty());

    Iterator const i_tile = begin();
    ASSERT(i_tile != end());
    Tile const result = *i_tile;
    erase(i_tile);

    return result;
}

Tile Tiles::PullRandomTile(void) {
	ASSERT(!IsEmpty());

	unsigned const n = Count();
    ASSERT(n > 0);
    unsigned const r = ::rand() % n;
    
     // find the "r"th tile in the bag
    Iterator i_tile = begin();
    ASSERT(i_tile != end());
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
        ASSERT(i_tile != end());
    }

    Tile const result = *i_tile;
    erase(i_tile);

	ASSERT(Count() == n-1);
    return result;
}

void Tiles::PullRandomTiles(unsigned tileCnt, Tiles& rBag) {
    for (unsigned draw_cnt = 0; draw_cnt < tileCnt; ++draw_cnt) {
        if (rBag.IsEmpty()) {
            break;
        }
        Tile const tile = rBag.PullRandomTile();
		Add(tile);
	}
}

// add one instance of every possible combination of attributes
void Tiles::Restock(void) {
	AttrIndexType const attribute_index = 0;
	Tile model_tile;

    AddAllTiles(attribute_index, model_tile);
}

void Tiles::UnClone(Tile& rClone) const {
	ASSERT(rClone.Id() == Tile::ID_DEFAULT);
	ASSERT(ContainsOpt(rClone));

    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
	   Tile const tile = *i_tile;
       if (rClone.HasOpt(tile)) {
           rClone = tile;
           break;
       }
    }

	ASSERT(rClone.Id() != Tile::ID_DEFAULT);
	ASSERT(Contains(rClone));
}

// return a new set containing only one instance of each clone
Tiles Tiles::UniqueTiles(void) const {
    Tiles result;
    
    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
		Tile const tile = *i_tile;
        if (!result.ContainsOpt(tile)) {
            result.Add(tile);
        }
    }
    
    return result; 
}


// inquiry methods

bool Tiles::AreAllCompatible(void) const {
    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
		Tile const tile = *i_tile;
        ConstIterator i_tile2 = i_tile;
        for (i_tile2++; i_tile2 != end(); i_tile2++) {
     		Tile const tile2 = *i_tile2;
            if (!tile2.IsCompatibleWith(&tile)) {
                return false;
            }
        }
    }
    
    return true; 
}

bool Tiles::AreAllCompatibleWith(Tile const& rTile) const {
	bool result = true;

    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
  		Tile const tile = *i_tile;
        if (!tile.IsCompatibleWith(&rTile)) {
            return false;
        }
    }

	return result;
}

bool Tiles::ContainsOpt(TileOpt const& rTileOpt) const {
    bool result = false;
    
    ConstIterator i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
		Tile const tile = *i_tile;
        if (tile.HasOpt(rTileOpt)) {
            result = true;
            break;
        }
    }
    
    return result;
}
