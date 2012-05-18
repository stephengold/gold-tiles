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
Individual cells may be referenced by means of Cell objects.

The Baseboard class implements minimal functionality using two maps:
one to map cells to tiles and a reverse map to which maps tiles to cells.
Also, the limits of the played area are cached.

The Board class extends BaseBoard to add functionality.
*/

#include <map>       // HASA std::map
#include "cell.hpp"  // HASA Cell
#include "tile.hpp"  // HASA Tile


class BaseBoard {
public:
    // public lifecycle
    BaseBoard(void);
    // BaseBoard(BaseBoard const&);  implicitly defined copy constructor
    // ~BaseBoard(void);  implicitly defined destructor

    // public operators
    // BaseBoard& operator=(BaseBoard const&);  implicitly defined assignment operator
    operator Tiles(void) const;

    // misc public methods
    unsigned    Count(void) const;
    ColumnType  EastMax(void) const;
    Tile const* GetCell(Cell const&) const;
    bool        LocateTile(Tile::IdType, Cell&) const;
    void        MakeEmpty(void);
    void        MakeEmpty(Cell const&);
    RowType     NorthMax(void) const;
    void        PlayOnCell(Cell const&, Tile const&);
    RowType     SouthMax(void) const;
    ColumnType  WestMax(void) const;

private:
    // private types
    typedef std::map<Cell,Tile>     CellMap;
    typedef CellMap::const_iterator	CellConstIterator;
    typedef CellMap::iterator		CellIterator;
    typedef std::map<Tile,Cell>     TileMap;
    typedef TileMap::const_iterator TileConstIterator;
    typedef TileMap::iterator		TileIterator;

    // private data
    CellMap    mCells;
    RowType    mNorthMax, mSouthMax; // limits of the range of played rows
    ColumnType mEastMax, mWestMax;   // limits of the range of played columns
    TileMap    mTiles;

    // misc private methods
    CellConstIterator Find(RowType, ColumnType) const;
    CellIterator      Find(RowType, ColumnType);

    // private inquiry methods
    bool IsEmptyColumn(ColumnType) const;
    bool IsEmptyRow(RowType) const;
};
#endif // !defined(BASEBOARD_HPP_INCLUDED)
