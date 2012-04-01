// File:    tiles.cpp
// Purpose: Tiles class
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
#include "string.hpp"
#include "strings.hpp"
#include "tiles.hpp"

// lifecycle

// default constructor
Tiles::Tiles(void)
{}

// construct a container for a single Tile
Tiles::Tiles(Tile const &rTile) {
    Add(rTile);
}


// operators

// return a copy of the Rth tile in the list
Tile Tiles::operator[](unsigned r) const {
    ASSERT(r < mMap.size());
    
    ConstIterator i_tile = mMap.begin();
    ASSERT(i_tile != mMap.end());
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
        ASSERT(i_tile != mMap.end());
	}
	Tile const result = i_tile->second;

	return result;
}

Tiles::operator String(void) const {
    String result("{");

    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
        if (i_tile != mMap.begin()) {
			// append a separator
            result += ", ";
        }

		Tile const tile = i_tile->second;
		String str = String(tile);
		str = str.Purge();
        result += str;
    }       
    result += "}";

    return result;
}

Tiles::operator Indices(void) const {
    Indices result;
    
    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		TileIdType const id = i_tile->first;
		result.Add(id);
    }
    
    return result;
}


// misc methods

void Tiles::Add(Tile const &tile) {
	ASSERT(!Contains(tile));

	TileIdType const id = tile.Id();
	std::pair<const TileIdType,Tile> const add(id, tile);
    std::pair<Iterator, bool> const ins = mMap.insert(add);
    bool const success = ins.second;
    ASSERT(success);

	ASSERT(Contains(tile));
}

// generate tiles for the stock bag - RECURSIVE
void Tiles::AddAllTiles(AIndexType attributeIndex, Tile &rModelTile) {
    ACountType const na = Tile::AttributeCnt();
	if (attributeIndex < na) {
		AValueType const max = Tile::ValueMax(attributeIndex);
		for (AValueType attr = 0; attr <= max; attr++) {
        	rModelTile.SetAttribute(attributeIndex, attr);
	        AddAllTiles(attributeIndex + 1, rModelTile);
         }
	} else {
        ASSERT(attributeIndex == na);
        Tile clone = rModelTile.CloneAndSetBonus();
		Add(clone);
	}
}

// merge with another Tiles object
void Tiles::AddTiles(Tiles const &tiles) {
    ConstIterator i_tile;
    for (i_tile = tiles.mMap.begin(); i_tile != tiles.mMap.end(); i_tile++) {
		Tile const tile = i_tile->second;
		Add(tile);
	}
}

// build runs of mutually-compatible tiles - RECURSIVE
void Tiles::BuildRuns(Tiles const &rRunSoFar, Tiles &rLongestRun) const {
	if (IsEmpty()) {
		if (rRunSoFar.Count() > rLongestRun.Count()) {
			rLongestRun = rRunSoFar;
		}
	} else {
		Tiles run(rRunSoFar);
	    Tiles remainder(*this);
	    Tile const first = remainder.DrawFirstTile();

		// build runs without the first tile
		remainder.BuildRuns(run, rLongestRun);

		if (run.AreAllCompatibleWith(first)) {
		   // build runs with the first tile
		   run.Add(first);
	       remainder.BuildRuns(run, rLongestRun);
		}
	}
}

unsigned Tiles::Count(void) const {
	unsigned const result = mMap.size();

	return result;
}

Tile Tiles::DrawFirstTile(void) {
	ASSERT(!IsEmpty());

    Iterator const i_tile = mMap.begin();
    ASSERT(i_tile != mMap.end());
    Tile const result = i_tile->second;
    mMap.erase(i_tile);

    return result;
}

Tile Tiles::DrawRandomTile(void) {
	ASSERT(!IsEmpty());

	unsigned const n = Count();
    ASSERT(n > 0);
    unsigned const r = ::rand() % n;
    
     // find the "r"th tile in the bag
    Iterator i_tile = mMap.begin();
    ASSERT(i_tile != mMap.end());
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
        ASSERT(i_tile != mMap.end());
    }

    Tile const result = i_tile->second;
    mMap.erase(i_tile);

	ASSERT(Count() == n-1);
    return result;
}

void Tiles::DrawTiles(unsigned tileCnt, Tiles &rBag) {
    for (unsigned draw_cnt = 0; draw_cnt < tileCnt; ++draw_cnt) {
        if (rBag.IsEmpty()) {
            break;
        }
        Tile const tile = rBag.DrawRandomTile();
		Add(tile);
	}
}

Tile Tiles::FindTile(TileIdType id) const {
    ASSERT(ContainsId(id));
     
    ConstIterator const i_tile = mMap.find(id);
    ASSERT(i_tile != mMap.end());
	ASSERT(i_tile->first == id);
    Tile const result = i_tile->second;
    
    ASSERT(result.Id() == id);
    return result; 
}

void Tiles::GetUserChoice(Tiles const &rAvailableTiles) {
    MakeEmpty();

    for (;;) {
		Strings alts;
		if (IsEmpty()) {
			alts.Append("none");
		} else {
   		    alts.Append("end");
		}

		Tile tile;
		String const input = tile.GetUserChoice(rAvailableTiles, alts);
		if (input == "end" || input == "none") {
			break;
		}

		if (tile.IsCloneAny(*this)) {
            std::cout << input << " is already selected." << std::endl;
        } else {
            rAvailableTiles.UnClone(tile);
            Add(tile);
        }
    }
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

void Tiles::MakeEmpty(void) {
	mMap.clear();

	ASSERT(IsEmpty());
}

void Tiles::RemoveTile(Tile const &rTile) {
	ASSERT(Contains(rTile));

	TileIdType const id = rTile.Id();
	RemoveTileId(id);

	ASSERT(!Contains(rTile));
}

void Tiles::RemoveTileId(TileIdType id) {
	ASSERT(ContainsId(id));

	Iterator const i_tile = mMap.find(id);
    ASSERT(i_tile != mMap.end());
	ASSERT(i_tile->first == id);

	mMap.erase(i_tile);

	ASSERT(!ContainsId(id));
}

void Tiles::RemoveTiles(Tiles const &rTiles) {
	ASSERT(Count() >= rTiles.Count());

	ConstIterator i_tile;
	for (i_tile = rTiles.mMap.begin(); i_tile != rTiles.mMap.end(); i_tile++) {
		TileIdType id = i_tile->first;
        RemoveTileId(id);
    }
}

// add one instance of every possible combination of attributes
void Tiles::Restock(void) {
	AIndexType const attribute_index = 0;
	Tile model_tile;

    AddAllTiles(attribute_index, model_tile);
}

void Tiles::UnClone(Tile &rClone) const {
	ASSERT(ContainsClone(rClone));

    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
	   Tile const tile = i_tile->second;
       if (rClone.IsClone(tile)) {
           rClone = tile;
           break;
       }
    }

	ASSERT(Contains(rClone));
}

// return a new set containing only one instance of each clone
Tiles Tiles::UniqueTiles(void) const {
    Tiles result;
    
    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile const tile = i_tile->second;
        if (!tile.IsCloneAny(result)) {
            result.Add(tile);
        }
    }
    
    return result; 
}


// inquiry methods

bool Tiles::AreAllCompatible(void) const {
    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile const tile = i_tile->second;
        ConstIterator i_tile2 = i_tile;
        for (i_tile2++; i_tile2 != mMap.end(); i_tile2++) {
     		Tile const tile2 = i_tile2->second;
            if (!tile2.IsCompatibleWith(&tile)) {
                return false;
            }
        }
    }
    
    return true; 
}

bool Tiles::AreAllCompatibleWith(Tile const &rTile) const {
	bool result = true;

    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
  		Tile const tile = i_tile->second;
        if (!tile.IsCompatibleWith(&rTile)) {
            return false;
        }
    }

	return result;
}

bool Tiles::Contains(Tile const &rTile) const {
	TileIdType const id = rTile.Id();
    bool const result = ContainsId(id);
    
    return result;
}

bool Tiles::ContainsId(TileIdType id) const {
    ConstIterator const i_tile = mMap.find(id);
    bool const result = (i_tile != mMap.end());
    
    return result;
}

bool Tiles::ContainsClone(Tile const &rTile) const {
    bool result = false;
    
    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile const tile = i_tile->second;
        if (tile.IsClone(rTile)) {
            result = true;
            break;
        }
    }
    
    return result;
}

bool Tiles::IsEmpty(void) const {
    bool const result = (Count() == 0);
    
    return result;
}

bool Tiles::IsValid(void) const {
    bool result = true;
    
    ConstIterator i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
        if (!i_tile->second.IsValid()) {
            result = false;
            break;
        }
    }

    return result;
}
