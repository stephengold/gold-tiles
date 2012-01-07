#ifndef TILESQUARE_HPP
#define TILESQUARE_HPP

// File:    tilesquare.hpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp"
#include "tile.hpp"

using namespace std;

class TileSquare {
    GridRef *square;
    Tile *tile;
    
    public:
        TileSquare(void);
        TileSquare(Tile *, GridRef *);
        GridRef *getSquare(void) const;  
        Tile *getTile(void) const;
};

#endif
