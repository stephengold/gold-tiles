#ifndef BASEBOARD_HPP
#define BASEBOARD_HPP

// File:    baseboard.hpp
// Purpose: BaseBoard class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A BaseBoard object describes a two-dimensional playing surface
 which may be extended in four directions.  The surface is
 modeled as a grid of cells onto which Tile objects may be
 played.  Cells are referenced by means of GridRef objects.
 A given cell may only be played on once.
 
 The Board class extends BaseBoard and adds functionality.
*/

#include <map>
#include "gridref.hpp"
#include "tile.hpp"

class BaseBoard {
    // private datatype
    typedef map<GridRef,Tile>::const_iterator const_iterator;

    // private data
    int maxN, maxS, maxE, maxW; // extent of played area
    map<GridRef,Tile> cells;

    // private methods
    const_iterator find(int n, int e) const;

    public:
        BaseBoard(void);

        void display(void) const;
		Tile const *getCell(GridRef const &) const;
		void playOnCell(GridRef const &, Tile const &);
        string toString(void) const;
};

#endif
