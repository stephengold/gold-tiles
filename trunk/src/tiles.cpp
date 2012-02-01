// File:    tiles.cpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include "tiles.hpp"

void Tiles::addTile(Tile const &tile) {
    std::pair<Tiles::iterator, bool> ins;
    ins = insert(tile);
    bool success = ins.second;
    assert(success);
}

void Tiles::addTiles(Tiles const &tiles) {
    const_iterator tile;
    for (tile = tiles.begin(); tile != tiles.end(); tile++) {
        std::pair<Tiles::iterator, bool> ins;
        ins = insert(*tile);
        bool success = ins.second;
        ASSERT(success);
	}
}

bool Tiles::areAllCompatible(void) const {
    const_iterator i;
    for (i = begin(); i != end(); i++) {
        const_iterator j = i;
        for (j++; j != end(); j++) {
            if (!i->isCompatibleWith(&*j)) {
                return false;
            }
        }
    }
    
    return true; 
}

bool Tiles::contains(Tile const &tile) const {
    const_iterator t = find(tile);
    bool result = (t != end());
    
    return result;
}

Tile Tiles::drawRandomTile(void) {
    unsigned n = size();
    ASSERT(n > 0);
    unsigned r = rand() % n;
    
     // find the "r"th tile in the bag
    iterator tile = begin();
    for (unsigned ti = 0; ti < r; ti++) {
        tile++;
    }
    ASSERT(tile != end());

    Tile result = *tile;
    erase(tile);

    return result;
}

unsigned Tiles::drawTiles(unsigned tileCount, Tiles &bag) {
	unsigned numTilesDrawn;

    for (numTilesDrawn = 0; numTilesDrawn < tileCount; ++numTilesDrawn) {
        if (bag.isEmpty()) {
            break;
        }
        Tile tile = bag.drawRandomTile();
        D(cout << "Drew " << tile.toString() << "." << endl);
		insert(tile);
	}

	return numTilesDrawn;
}

Tile Tiles::findTile(TileId id) const {
    Tile result;
    const_iterator tile;
    for (tile = begin(); tile != end(); tile++) {
        if (tile->getId() == id) {
            result = *tile;
            break;
        }
    }
    ASSERT(tile != end());
    
    return result; 
}

Tiles Tiles::getLongestRun(void) const {
	Tiles unique = uniqueTiles();
	D(cout << plural(unique.size(), "unique tile") << ":" << endl
        << " " << unique.toString() << "." << endl);

	Tiles result;
	string raString;
    const_iterator tile;
    for (tile = unique.begin(); tile != unique.end(); tile++) {
        Tile t = *tile;
        for (AIndex ind = 0; ind < t.getNumAttributes(); ind++) {
            AValue value = t.getAttribute(ind);
            Tiles run;
            const_iterator t2;
            for (t2 = tile; t2 != unique.end(); t2++) {
                if (t2->hasAttribute(ind, value)) {
                    run.insert(*t2);
                }
            }
            if (run.size() > result.size()) {
                result = run;
                raString = attributeToString(ind, value);
            }
        }
    }
    
    D(cout << "Found a run of " << result.size() << " " 
		<< raString << plural(result.size()) << "." << endl);

	return result;
}

void Tiles::getUserChoice(Tiles const &availableTiles) {
    clear();

    while (true) {
		Strings alts;
		if (size() == 0) {
			alts.insert("none");
		} else {
   		    alts.insert("end");
		}

		Tile tile;
		string input = tile.getUserChoice(availableTiles, alts);
		if (input == "end" || input == "none") {
			break;
		}

		if (tile.isCloneAny(*this)) {
            cout << input << " is already selected." << endl;
        } else {
            availableTiles.unClone(tile);
            insert(tile);
        }
    }
}

bool Tiles::isEmpty(void) const {
    bool result = (size() == 0);
    
    return result;
}

bool Tiles::isValid(void) const {
    bool result = true;
    
    Tiles::iterator tile;
    for (tile = begin(); tile != end(); tile++) {
        if (!tile->isValid()) {
            result = false;
            break;
        }
    }

    return result;
}

void Tiles::removeTile(Tile const &tile) {
	iterator position = find(tile);
    ASSERT(position != end());
	erase(position);
}

void Tiles::removeTiles(Tiles const &tiles) {
	const_iterator tile;
	for (tile = tiles.begin(); tile != tiles.end(); tile++) {
        removeTile(*tile);
    }
}

string Tiles::toString(void) const {
    string result;

    result += "{";
    const_iterator tile;
    for (tile = begin(); tile != end(); tile++) {
        if (tile != begin()) {
            result += ", ";
        } 
        result += tile->toString();
    }       
    result += "}";

    return result;
}

void Tiles::unClone(Tile &clone) const {
    const_iterator t;
    for (t = begin(); t != end(); t++) {
       if (clone.isClone(*t)) {
           clone = *t;
           break;
       }
    }
}

// return a new set containing only one copy of each clone
Tiles Tiles::uniqueTiles(void) const {
    Tiles result;
    
    const_iterator tile;
    for (tile = begin(); tile != end(); tile++) {
        if (!tile->isCloneAny(result)) {
            result.insert(*tile);
        }
    }
    
    return result; 
}
