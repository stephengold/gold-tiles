// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "TileSquare.hpp"

TileSquare::TileSquare(void) {
    tile = NULL;
    square = NULL;
}

TileSquare::TileSquare(Tile *t, GridRef *s) {
    tile = t;
    square = s;
}

Tile *TileSquare::getTile(void) const {
    return tile;
}

GridRef *TileSquare::getSquare(void) const {
    return square;
}
