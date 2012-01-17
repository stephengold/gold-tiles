#ifndef TILESQUARE_HPP
#define TILESQUARE_HPP

// File:    tilesquare.hpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp" // GridRef
#include "tile.hpp"

class TileSquare {
    GridRef square;
    Tile tile;
    
    public:
        TileSquare(Tile const &, GridRef const &);
        
        GridRef getSquare(void) const;  
        Tile getTile(void) const;
        bool operator<(TileSquare const &) const;
};

#endif
