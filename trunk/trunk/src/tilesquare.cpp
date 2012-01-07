#include "TileSquare.hpp"

TileSquare::TileSquare(void) {
    this->tile = NULL;
    this->square = NULL;
}

TileSquare::TileSquare(Tile * tile, GridRef * square) {
    this->tile = tile;
    this->square = square;
}

Tile * TileSquare::getTile(void) {
    return this->tile;
}

GridRef * TileSquare::getSquare(void) {
    return this->square;
}
