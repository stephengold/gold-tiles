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

void Play::add(Tile const &tile, GridRef const &square) {
    TileSquare ts(tile, square);
    insert(ts);
}

Tiles Play::getTiles(void) const {
    Tiles result;
    
    const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        Tile tile = ts->getTile();
        result.insert(tile);
    }

    return result;
}

Locus Play::getSquares(void) const {
    Locus result;
    
    const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
		if (!ts->isSwap()) {
            GridRef square = ts->getSquare();
            result.insert(square);
		}
    }

    return result;
}

void Play::getUserChoice(Tiles const &availableTiles) {
    clear();

    while (true) {
		Strings alts;
		if (size() == 0) {
			alts.insert("pass");
		} else {
			alts.insert("go");
		}

		TileSquare ts;
		string input = ts.getUserChoice(availableTiles, alts);
		D(cout << "input=\'" << input << "'" << endl);
        if (input == "pass" || input == "go") {
            break;
        }

        insert(ts);
    }
}

bool Play::isPass(void) const {
	bool result = (size() == 0);

	return result;
}

bool Play::isPureSwap(void) const {
	bool result = true;

    const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        if (!ts->isSwap()) {
        	result = false;
			break;
		}
	}

	return result;
}

bool Play::isSwap(void) const {
    bool result = false;

	const_iterator ts;
	for (ts = begin(); ts != end(); ts++) {
		if (ts->isSwap()) {
			result = true;
			break;
		}
	}

	return result;
}

bool Play::repeatsTile(void) const {
    bool result = false;
    
    if (size() > 1) {
        Tiles tiles;
        const_iterator ts;
        
        for (ts = begin(); ts != end(); ts++) {
            Tile t = ts->getTile();
            if (tiles.contains(t)) {
                result = true;
                break;
            } else {
                tiles.insert(t);
            }
        }
    }
    
    return result;
}

bool Play::repeatsSquare(void) const {
    bool result = false;
    
    if (size() > 1) {
        Locus squares;
        const_iterator ts;
        
        for (ts = begin(); ts != end(); ts++) {
    		if (!ts->isSwap()) {
                GridRef s = ts->getSquare();
                if (squares.contains(s)) {
                    result = true;
                    break;
                } else {
                    squares.insert(s);
                }
			}
        }
    }
     
    return result;
}

string Play::toString(void) const {
	string result;

    result += "{";
    const_iterator ts;
    for (ts = begin(); ts != end(); ts++) {
        if (ts != begin()) {
            result += ", ";
        } 
        result += ts->toString();
    }       
    result += "}";

    return result;
}
