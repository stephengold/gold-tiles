// File:    play.cpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "play.hpp"

Tiles Play::getTiles(void) const {
    Tiles result;
    
    Play::const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        Tile tile = ts->getTile();
        result.insert(tile);
    }

    return result;
}

Locus Play::getSquares(void) const {
    Locus result;
    
    Play::const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        GridRef square = ts->getSquare();
        result.insert(square);
    }

    return result;
}

bool Play::repeatsTile(void) const {
    bool result = false;
    
    if (size() > 1) {
        Tiles tileSet;
        Play::const_iterator ts;
        
        for (ts = begin(); ts != end(); ts++) {
            Tile t = ts->getTile();
            if (tileSet.find(t) != tileSet.end()) {
                result = true;
                break;
            } else {
                tileSet.insert(t);
            }
        }
    }
    
    return result;
}

bool Play::repeatsSquare(void) const {
    bool result = false;
    
    if (size() > 1) {
        Locus squareSet;
        Play::const_iterator ts;
        
        for (ts = begin(); ts != end(); ts++) {
            GridRef s = ts->getSquare();
            if (squareSet.find(s) != squareSet.end()) {
                result = true;
                break;
            } else {
                squareSet.insert(s);
            }
        }
    }
     
    return result;
}
