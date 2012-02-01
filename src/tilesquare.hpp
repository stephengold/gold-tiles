#ifndef TILESQUARE_HPP_INCLUDED
#define TILESQUARE_HPP_INCLUDED

// File:    tilesquare.hpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A TileSquare object describes both a playing piece 
 (Tile) and how it is to be played.
*/

#include "gridref.hpp"
#include "tile.hpp"

class TileSquare {
    GridRef _square;
	bool _swapFlag;
    Tile _tile;

    public:
		TileSquare(void);
        TileSquare(Tile const &);
        TileSquare(Tile const &, GridRef const &);
        // ~TileSquare(void);  implicitly declared destructor is fine
        
        void display(void) const;
        GridRef getSquare(void) const;  
        Tile getTile(void) const;
        string getUserChoice(Tiles const &, Strings const &);
		bool isSwap(void) const;
        bool operator<(TileSquare const &) const;
        string toString(void) const;
};

#endif
