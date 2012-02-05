#ifndef BASEBOARD_HPP_INCLUDED
#define BASEBOARD_HPP_INCLUDED

// File:    baseboard.hpp
// Purpose: BaseBoard class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A BaseBoard object represents a two-dimensional playing grid
 which grows automatically in four directions.  The grid is
 composed of cells on which Tile objects may be played.
 Cells are referenced by means of Cell objects.
 
 Basboard implements minimal functionality.
 The Board class extends BaseBoard to add functionality.
*/

#include <map>
#include "cell.hpp"
#include "tile.hpp"

class BaseBoard {
public:
	// lifecycle
    BaseBoard(void);
    // BaseBoard(BaseBoard const &);  compiler-generated copy constructor is OK
    // ~BaseBoard(void);  compiler-generated destructor is OK

	// operators
    // BaseBoard &operator=(BaseBoard const &);  compiler-generated assignment method is OK
	operator String(void) const;

	// misc
    bool        FindTile(Tile const &, Cell &) const;
    Tile const *GetCell(Cell const &) const;
    void        MakeEmpty(Cell const &);
	void        PlayOnCell(Cell const &, Tile const &);

	// access
	int	EastMax(void) const;
    int	NorthMax(void) const;
    int	SouthMax(void) const;
    int	WestMax(void) const;

private:
    typedef std::map<Cell,Tile>::const_iterator	ConstIteratorType;
    typedef std::map<Cell,Tile>::iterator		IteratorType;
    typedef std::map<Cell,Tile>::size_type		SizeType;

    int mNorthMax, mSouthMax, mEastMax, mWestMax; // extent of played area
    std::map<Cell,Tile> mCells;

	// meat
    ConstIteratorType Find(int northing, int easting) const;
    IteratorType	  Find(int northing, int easting);
};

#endif