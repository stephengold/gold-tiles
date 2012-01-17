// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "TileSquare.hpp"

// constructors

TileSquare::TileSquare(Tile const &t, GridRef const &s)
: tile(t), square(s) {}

// accessors

Tile TileSquare::getTile(void) const {
    return tile;
}

GridRef TileSquare::getSquare(void) const {
    return square;
}

// compare

bool TileSquare::operator<(TileSquare const &other) const {
     bool result;
     
     if (tile == other.tile) {
         result = (square < other.square);
     } else {
         result = (tile < other.tile);
     }
     
     return result;
}
