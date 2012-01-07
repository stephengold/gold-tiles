// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "TileSquare.hpp"

TileSquare::TileSquare(void) {
    this->tile = NULL;
    this->square = NULL;
}

TileSquare::TileSquare(Tile *tile, GridRef *square) {
    this->tile = tile;
    this->square = square;
}

Tile * TileSquare::getTile(void) const {
    return this->tile;
}

GridRef * TileSquare::getSquare(void) const {
    return this->square;
}
