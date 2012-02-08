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


// operators
bool Tiles::operator==(Tiles const &rOther) const {
	ASSERT(false);  // not implemented yet
	return false;
}

Tile Tiles::operator[](unsigned r) const {
    ConstIteratorType i_tile = mMap.begin();
    ASSERT(i_tile != mMap.end());
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
        ASSERT(i_tile != mMap.end());
	}
	Tile result = i_tile->second;

	return result;
}

Tiles::operator String(void) const {
    String result;

    result += "{";
    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
        if (i_tile != mMap.begin()) {
            result += ", ";
        } 
		Tile tile = i_tile->second;
        result += String(tile);
    }       
    result += "}";

    return result;
}


// misc

void Tiles::Add(Tile const &tile) {
	ASSERT(!Contains(tile));

	TileIdType id = tile.Id();
	std::pair<const TileIdType,Tile> add(id, tile);
    std::pair<IteratorType, bool> ins = mMap.insert(add);
    bool success = ins.second;
    ASSERT(success);

	ASSERT(Contains(tile));
}

void Tiles::AddTiles(Tiles const &tiles) {
    ConstIteratorType i_tile;
    for (i_tile = tiles.mMap.begin(); i_tile != tiles.mMap.end(); i_tile++) {
		Tile tile = i_tile->second;
		Add(tile);
	}
}


unsigned Tiles::Count(void) const {
	unsigned result = mMap.size();

	return result;
}

Tile Tiles::DrawRandomTile(void) {
	ASSERT(!IsEmpty());
    unsigned n = Count();
    ASSERT(n > 0);
    unsigned r = ::rand() % n;
    
     // find the "r"th tile in the bag
    IteratorType i_tile = mMap.begin();
    ASSERT(i_tile != mMap.end());
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
        ASSERT(i_tile != mMap.end());
    }

    Tile result = i_tile->second;
    mMap.erase(i_tile);

	ASSERT(Count() == n-1);
    return result;
}

unsigned Tiles::DrawTiles(unsigned tileCnt, Tiles &bag) {
	unsigned draw_cnt;

    for (draw_cnt = 0; draw_cnt < tileCnt; ++draw_cnt) {
        if (bag.IsEmpty()) {
            break;
        }
        Tile tile = bag.DrawRandomTile();
        D(std::cout << "Drew " << String(tile) << "." << std::endl);
		Add(tile);
	}

	return draw_cnt;
}

Tile Tiles::FindTile(TileIdType id) const {
    ConstIteratorType i_tile = mMap.find(id);
    ASSERT(i_tile != mMap.end());
	ASSERT(i_tile->first == id);
    Tile result = i_tile->second;
    
    return result; 
}

void Tiles::GetUserChoice(Tiles const &rAvailableTiles) {
    MakeEmpty();

    while (true) {
		Strings alts;
		if (IsEmpty()) {
			alts.Append("none");
		} else {
   		    alts.Append("end");
		}

		Tile tile;
		String input = tile.GetUserChoice(rAvailableTiles, alts);
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

Tiles Tiles::LongestRun(void) const {
	Tiles unique = UniqueTiles();
	D(std::cout << plural(unique.Count(), "unique tile") << ":" << std::endl
        << " " << String(unique) << "." << std::endl);

	Tiles result;
	String raString;
    ConstIteratorType i_tile;
    for (i_tile = unique.mMap.begin(); i_tile != unique.mMap.end(); i_tile++) {
        Tile tile = i_tile->second;
        for (AIndexType ind = 0; ind < Tile::AttributeCnt(); ind++) {
            AValueType value = tile.Attribute(ind);
            Tiles run;
            ConstIteratorType i_tile2;
            for (i_tile2 = i_tile; i_tile2 != unique.mMap.end(); i_tile2++) {
				Tile tile2 = i_tile2->second;
                if (tile2.HasAttribute(ind, value)) {
                    run.Add(tile2);
                }
            }
            if (run.Count() > result.Count()) {
                result = run;
                raString = attributeToString(ind, value);
            }
        }
    }
    
    D(std::cout << "Found a run of " << result.Count() << " " 
		<< raString << plural(result.Count()) << "." << std::endl);

	return result;
}

void Tiles::MakeEmpty(void) {
	mMap.clear();
	ASSERT(IsEmpty());
}

void Tiles::RemoveTile(Tile const &rTile) {
	ASSERT(Contains(rTile));

	TileIdType id = rTile.Id();
	RemoveTileId(id);

	ASSERT(!Contains(rTile));
}

void Tiles::RemoveTileId(TileIdType id) {
	ASSERT(ContainsId(id));

	IteratorType i_tile = mMap.find(id);
    ASSERT(i_tile != mMap.end());
	ASSERT(i_tile->first == id);

	mMap.erase(i_tile);

	ASSERT(!ContainsId(id));
}

void Tiles::RemoveTiles(Tiles const &rTiles) {
	ASSERT(Count() >= rTiles.Count());

	ConstIteratorType i_tile;
	for (i_tile = rTiles.mMap.begin(); i_tile != rTiles.mMap.end(); i_tile++) {
        RemoveTileId(i_tile->first);
    }
}

void Tiles::UnClone(Tile &rClone) const {
	ASSERT(ContainsClone(rClone));

    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
	   Tile tile = i_tile->second;
       if (rClone.IsClone(tile)) {
           rClone = tile;
           break;
       }
    }

	ASSERT(Contains(rClone));
}

// return a new set containing only one copy of each clone
Tiles Tiles::UniqueTiles(void) const {
    Tiles result;
    
    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile tile = i_tile->second;
        if (!tile.IsCloneAny(result)) {
            result.Add(tile);
        }
    }
    
    return result; 
}


// inquiries

bool Tiles::AreAllCompatible(void) const {
    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile tile = i_tile->second;
        ConstIteratorType i_tile2 = i_tile;
        for (i_tile2++; i_tile2 != mMap.end(); i_tile2++) {
     		Tile tile2 = i_tile2->second;
            if (!tile2.IsCompatibleWith(&tile)) {
                return false;
            }
        }
    }
    
    return true; 
}

bool Tiles::Contains(Tile const &tile) const {
	TileIdType id = tile.Id();
    bool result = ContainsId(id);
    
    return result;
}

bool Tiles::ContainsId(TileIdType id) const {
    ConstIteratorType i_tile = mMap.find(id);
    bool result = (i_tile != mMap.end());
    
    return result;
}

bool Tiles::ContainsClone(Tile const &rTile) const {
    bool result = false;
    
    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
		Tile tile = i_tile->second;
        if (tile.IsClone(rTile)) {
            result = true;
            break;
        }
    }
    
    return result;
}

bool Tiles::IsEmpty(void) const {
    bool result = (Count() == 0);
    
    return result;
}

bool Tiles::IsValid(void) const {
    bool result = true;
    
    ConstIteratorType i_tile;
    for (i_tile = mMap.begin(); i_tile != mMap.end(); i_tile++) {
        if (!i_tile->second.IsValid()) {
            result = false;
            break;
        }
    }

    return result;
}
