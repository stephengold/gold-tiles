#ifndef BASEBOARD_HPP_INCLUDED
#define BASEBOARD_HPP_INCLUDED

// File:     baseboard.hpp
// Location: src
// Purpose:  declare BaseBoard class
// Author:   Stephen Gold sgold@sonic.net
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
which grows automatically in all directions.  The grid is
composed of cells on which Tile objects may be played.
Cells are referenced by means of Cell objects.
 
The Baseboard class implements minimal functionality using two maps:
one to map cells to tiles and a reverse map to get cells from tile IDs.
The Board class extends BaseBoard to add functionality.
*/

#include <map>       // HASA std::map
#include "cell.hpp"  // HASA Cell
#include "tile.hpp"  // HASA Tile

class BaseBoard {
public:
	// public lifecycle
    BaseBoard(void);
	void MakeEmpty(void);
    // BaseBoard(BaseBoard const&);  compiler-generated copy constructor is OK
    // ~BaseBoard(void);  compiler-generated destructor is OK

	// public operators
    // BaseBoard& operator=(BaseBoard const&);  compiler-generated assignment operator is OK
	operator Tiles(void) const;

	// misc public methods
	unsigned    Count(void) const;
	IndexType   EastMax(void) const;
    Tile const* GetCell(Cell const&) const;
    bool        LocateTileId(TileIdType, Cell&) const;
    void        MakeEmpty(Cell const&);
    IndexType   NorthMax(void) const;
	void        PlayOnCell(Cell const&, Tile const&);
    IndexType   SouthMax(void) const;
    IndexType	WestMax(void) const;

private:
	// private types
	typedef std::map<Cell,Tile>       CellMap;
    typedef CellMap::const_iterator	  CellConstIterator;
    typedef CellMap::iterator		  CellIterator;
	typedef std::map<TileIdType,Cell> TileMap;
	typedef TileMap::const_iterator   TileConstIterator;
    typedef TileMap::iterator		  TileIterator;

	// private data
    CellMap   mCells;
	IndexType mNorthMax, mSouthMax, mEastMax, mWestMax; // extent of the played area
    TileMap   mTiles;

	// misc private methods
    CellConstIterator Find(IndexType northing, IndexType easting) const;
    CellIterator	  Find(IndexType northing, IndexType easting);
    
    // private inquiry methods
    bool IsEmptyColumn(IndexType) const;
    bool IsEmptyRow(IndexType) const;
};
#endif // !defined(BASEBOARD_HPP_INCLUDED)
