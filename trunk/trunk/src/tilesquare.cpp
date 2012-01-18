// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "TileSquare.hpp"

// constructors, assignment, and destructor

// The compiler.generated default constructor is fine.

TileSquare::TileSquare(Tile const &t, GridRef const &s):
    tile(t), square(s)
{
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// accessors

Tile TileSquare::getTile(void) const {
    return tile;
}

GridRef TileSquare::getSquare(void) const {
    return square;
}

//

void TileSquare::getUserChoice(Tiles const &availableTiles) {
    tile.getUserChoice(availableTiles);
    square.getUserChoice();
}

bool TileSquare::operator<(TileSquare const &other) const {
     bool result;
     
     if (tile == other.tile) {
         result = (square < other.square);
     } else {
         result = (tile < other.tile);
     }
     
     return result;
}

