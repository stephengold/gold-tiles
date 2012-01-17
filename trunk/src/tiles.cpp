// File:    tiles.cpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <sstream>
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

string Tiles::toString(void) const {
    ostringstream sout;

    sout << "{";
    const_iterator tile;
    for (tile = begin(); tile != end(); tile++) {
        if (tile != begin()) {
            sout << ", ";
        } 
        sout << tile->toString();
    }       
    sout << "}";
    string result = sout.str();

    return result;
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



