// File:    tiles.cpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include "tiles.hpp"

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

void Tiles::getUserChoice(/* User &, */ Tiles const &availableTiles) {
    clear();
    while (true) {
        cout << "Enter a tile name or 'end':" << endl;
        
        string input;
        cin >> input;
        if (input == "end") {
            break;
        }
        Tile tile(input);
        ASSERT(isValid());
        if (input != tile.toString()) {
            cout << "'" << input << "' is invalid." << endl;
        } else if (!tile.isCloneAny(availableTiles)) {
            cout << input << " is unavailable." << endl;
        } else if (tile.isCloneAny(*this)) {
            cout << input << " is already selected." << endl;
        } else {
            availableTiles.unClone(tile);
            insert(tile);
        }
    }
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
