#ifndef BASEBOARD_HPP_INCLUDED
#define BASEBOARD_HPP_INCLUDED

// File:    baseboard.hpp
// Purpose: BaseBoard class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A BaseBoard object represents a two-dimensional playing grid
 which grows automatically in four directions.  The grid is
 composed of cells on which Tile objects may be played.
 Cells are referenced by means of GridRef objects.
 
 The Board class extends BaseBoard and adds functionality.
*/

#include <map>
#include "gridref.hpp"
#include "tile.hpp"

class BaseBoard {
    // private datatype
    typedef map<GridRef,Tile>::const_iterator const_iterator;
    typedef map<GridRef,Tile>::iterator iterator;
    typedef map<GridRef,Tile>::size_type size_type;

    // private data
    int _maxN, _maxS, _maxE, _maxW; // extent of played area
    map<GridRef,Tile> _cells;

    // private methods
    const_iterator find(int n, int e) const;
    iterator find(int n, int e);

    public:
        BaseBoard(void);
        // BaseBoard(BaseBoard const &);  compiler-generated copy constructor is OK
        // BaseBoard &operator=(BaseBoard const &)};  compiler-generated assignment method is OK
        // ~BaseBoard(void);  compiler-generated destructor is OK

        void display(void) const;
        void emptyCell(GridRef const &);
        bool findTile(Tile const &, GridRef &) const;
		Tile const *getCell(GridRef const &) const;
        int getMaxN(void) const;
        int getMaxS(void) const;
        int getMaxE(void) const;
        int getMaxW(void) const;
		void playOnCell(GridRef const &, Tile const &);
        string toString(void) const;
};

#endif
