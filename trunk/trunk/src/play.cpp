// File:    play.cpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include "locus.hpp"
#include "play.hpp"
#include "tile.hpp"
#include "tiles.hpp"

// constructors, assignment, and destructor

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

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

void Play::getUserChoice(Tiles const &availableTiles) {
    clear();

    while (true) {
        cout << "Enter a tile name or 'end':" << endl;

        string input;
        cin >> input;
        if (input == "end") {
            break;
        }
        Tile tile(input);
        ASSERT(tile.isValid());
        if (input != tile.toString()) {
            cout << "'" << input << "' is invalid." << endl;
        } else if (!tile.isCloneAny(availableTiles)) {
            cout << input << " is not available." << endl;
        } else if (tile.isCloneAny(getTiles())) {
            cout << input << " is already selected." << endl;
        } else {
            availableTiles.unClone(tile);
            
            // choose square to play on
            GridRef square;
            square.getUserChoice();
            TileSquare ts(tile, square);
            insert(ts);
        }
    }
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
