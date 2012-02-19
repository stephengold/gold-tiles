#ifndef BASEBOARD_HPP_INCLUDED
#define BASEBOARD_HPP_INCLUDED

// File:    baseboard.hpp
// Purpose: BaseBoard class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
A BaseBoard object represents a two-dimensional playing grid
which grows automatically in four directions.  The grid is
composed of cells on which Tile objects may be played.
Cells may be referenced by means of Cell objects.
 
The Basboard class implements minimal functionality.
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
    // BaseBoard &operator=(BaseBoard const &);  compiler-generated assignment operator is OK
	operator String(void) const;

	// misc public methods
	unsigned    Count(void) const;
	int	        EastMax(void) const;
    Tile const *GetCell(Cell const &) const;
    bool        LocateTileId(TileIdType, Cell &) const;
	void        MakeEmpty(void);
    void        MakeEmpty(Cell const &);
    int	        NorthMax(void) const;
	void        PlayOnCell(Cell const &, Tile const &);
    int	        SouthMax(void) const;
    int	        WestMax(void) const;

private:
    typedef std::map<Cell,Tile>::const_iterator	        ConstIteratorType;
    typedef std::map<Cell,Tile>::iterator		        IteratorType;
    typedef std::map<TileIdType,Cell>::const_iterator	ConstTileIteratorType;
    typedef std::map<TileIdType,Cell>::iterator		    TileIteratorType;

	int mNorthMax, mSouthMax, mEastMax, mWestMax; // extent of the played area

    std::map<Cell,Tile>       mCells;
    std::map<TileIdType,Cell> mTiles;

	// misc private methods
    ConstIteratorType Find(IndexType northing, IndexType easting) const;
    IteratorType	  Find(IndexType northing, IndexType easting);
};

#endif
