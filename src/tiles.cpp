// File:    tiles.cpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include "string.hpp"
#include "strings.hpp"
#include "tiles.hpp"

Tiles::operator String(void) const {
    String result;

    result += "{";
    ConstIteratorType tile;
    for (tile = begin(); tile != end(); tile++) {
        if (tile != begin()) {
            result += ", ";
        } 
        result += String(*tile);
    }       
    result += "}";

    return result;
}

void Tiles::AddTile(Tile const &tile) {
    std::pair<Tiles::iterator, bool> ins;
    ins = insert(tile);
    bool success = ins.second;
    ASSERT(success);
}

void Tiles::AddTiles(Tiles const &tiles) {
    ConstIteratorType i_tile;
    for (i_tile = tiles.begin(); i_tile != tiles.end(); i_tile++) {
		AddTile(*i_tile);
	}
}

bool Tiles::AreAllCompatible(void) const {
    ConstIteratorType i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
		Tile tile = *i_tile;
        ConstIteratorType i_tile2 = i_tile;
        for (i_tile2++; i_tile2 != end(); i_tile2++) {
            if (!i_tile2->IsCompatibleWith(&tile)) {
                return false;
            }
        }
    }
    
    return true; 
}

bool Tiles::Contains(Tile const &rTile) const {
    ConstIteratorType i_tile = find(rTile);
    bool result = (i_tile != end());
    
    return result;
}

unsigned Tiles::Count(void) const {
	unsigned result = size();

	return result;
}

Tile Tiles::DrawRandomTile(void) {
    unsigned n = Count();
    ASSERT(n > 0);
    unsigned r = ::rand() % n;
    
     // find the "r"th tile in the bag
    IteratorType i_tile = begin();
    for (unsigned i = 0; i < r; i++) {
        i_tile++;
    }
    ASSERT(i_tile != end());

    Tile result = *i_tile;
    erase(i_tile);

    return result;
}

unsigned Tiles::DrawTiles(unsigned tileCount, Tiles &bag) {
	unsigned draw_cnt;

    for (draw_cnt = 0; draw_cnt < tileCount; ++draw_cnt) {
        if (bag.IsEmpty()) {
            break;
        }
        Tile tile = bag.DrawRandomTile();
        D(std::cout << "Drew " << String(tile) << "." << std::endl);
		insert(tile);
	}

	return draw_cnt;
}

Tile Tiles::FindTile(TileIdType id) const {
    Tile result;
    ConstIteratorType i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
        if (i_tile->Id() == id) {
            result = *i_tile;
            break;
        }
    }
    ASSERT(i_tile != end());
    
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
            insert(tile);
        }
    }
}

bool Tiles::IsEmpty(void) const {
    bool result = (Count() == 0);
    
    return result;
}

bool Tiles::IsValid(void) const {
    bool result = true;
    
    IteratorType i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
        if (!i_tile->IsValid()) {
            result = false;
            break;
        }
    }

    return result;
}

Tiles Tiles::LongestRun(void) const {
	Tiles unique = UniqueTiles();
	D(std::cout << plural(unique.Count(), "unique tile") << ":" << std::endl
        << " " << String(unique) << "." << std::endl);

	Tiles result;
	String raString;
    ConstIteratorType i_tile;
    for (i_tile = unique.begin(); i_tile != unique.end(); i_tile++) {
        Tile t = *i_tile;
        for (AIndexType ind = 0; ind < t.AttributeCnt(); ind++) {
            AValueType value = t.Attribute(ind);
            Tiles run;
            ConstIteratorType i_tile2;
            for (i_tile2 = i_tile; i_tile2 != unique.end(); i_tile2++) {
                if (i_tile2->HasAttribute(ind, value)) {
                    run.insert(*i_tile2);
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
	clear();
}

void Tiles::RemoveTile(Tile const &rTile) {
	IteratorType i_tile = find(rTile);
    ASSERT(i_tile != end());
	erase(i_tile);
}

void Tiles::RemoveTiles(Tiles const &rTiles) {
	ConstIteratorType i_tile;
	for (i_tile = rTiles.begin(); i_tile != rTiles.end(); i_tile++) {
        RemoveTile(*i_tile);
    }
}

void Tiles::UnClone(Tile &rClone) const {
    ConstIteratorType i_tile;
    for (i_tile = begin(); i_tile != end(); i_tile++) {
       if (rClone.IsClone(*i_tile)) {
           rClone = *i_tile;
           break;
       }
    }
}

// return a new set containing only one copy of each clone
Tiles Tiles::UniqueTiles(void) const {
    Tiles result;
    
    ConstIteratorType tile;
    for (tile = begin(); tile != end(); tile++) {
        if (!tile->IsCloneAny(result)) {
            result.insert(*tile);
        }
    }
    
    return result; 
}
