#ifndef TILESQUARE_HPP
#define TILESQUARE_HPP

// File:    tilesquare.hpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridref.hpp"
#include "tile.hpp"

class TileSquare {
    Tile tile;
    GridRef square;
    public:
        TileSquare(Tile const &, GridRef const &);
        
        void display(void) const;
        GridRef getSquare(void) const;  
        Tile getTile(void) const;
        void getUserChoice(Tiles const &);
        bool operator<(TileSquare const &) const;
        string toString(void) const;
};

#endif
