// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "TileSquare.hpp"

// constructors, assignment, and destructor

TileSquare::TileSquare(void) {
	swapFlag = true;
}

TileSquare::TileSquare(Tile const &t):
    tile(t)
{
	swapFlag = true;
}

TileSquare::TileSquare(Tile const &t, GridRef const &s):
    tile(t), square(s)
{
	swapFlag = false;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// accessors

Tile TileSquare::getTile(void) const {
    return tile;
}

GridRef TileSquare::getSquare(void) const {
	ASSERT(!swapFlag);
    return square;
}

bool TileSquare::isSwap(void) const {
	return swapFlag;
}

// public methods

string TileSquare::getUserChoice(Tiles const &availableTiles, Strings const &alts) {
    string result = tile.getUserChoice(availableTiles, alts);
	if (result == tile.toString()) {
        swapFlag = square.getUserChoice("swap");
	}
	return result;
}

bool TileSquare::operator<(TileSquare const &other) const {
     bool result;
     
     if (tile == other.tile) {
		 if (swapFlag == other.swapFlag) {
			 if (swapFlag == true) {
				 result = false;
			 } else {
				 result = (square < other.square);
			 }
		 } else {
             result = (swapFlag < other.swapFlag);
		 }
     } else {
         result = (tile < other.tile);
     }
     
     return result;
}

