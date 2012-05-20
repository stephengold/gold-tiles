// File:     baseboard.cpp
// Location: src
// Purpose:  implement BaseBoard class
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

#include "baseboard.hpp"
#include "tiles.hpp"


// lifecycle

// construct an empty board
BaseBoard::BaseBoard(void) {
    mNorthMax = 0;
    mSouthMax = 0;
    mEastMax = 0;
    mWestMax = 0;
}

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment operator is fine.

// get a list of all tiles played
BaseBoard::operator Tiles(void) const {
    Tiles result;

    CellConstIterator i_cell;
    for (i_cell = mCells.begin(); i_cell != mCells.end(); i_cell++) {
        Tile const tile = i_cell->second;
        result.Add(tile);
    }

    return result;
}


// misc methods

SizeType BaseBoard::Count(void) const {
    SizeType const result = SizeType(mCells.size());

    ASSERT(mTiles.size() == result);

    return result;
}

// get the eastern limit of the board
ColumnType BaseBoard::EastMax(void) const {
    return mEastMax;
}

// get iterators to specific cells
BaseBoard::CellConstIterator BaseBoard::Find(
    RowType northing, 
    ColumnType easting) const
{
    Cell const cell(northing, easting);
    CellConstIterator result = mCells.find(cell);

    return result;
}

BaseBoard::CellIterator BaseBoard::Find(
    RowType northing, 
    ColumnType easting)
{
    Cell const cell(northing, easting);
    CellIterator const result = mCells.find(cell);

    return result;
}

// get a pointer to the Tile (if any) in a specific cell
Tile const* BaseBoard::GetCell(Cell const& rCell) const {
    Tile const* p_result = NULL;

    CellConstIterator const i_cell = mCells.find(rCell);
    if (i_cell != mCells.end()) {
        p_result = &(i_cell->second);
    }

    return p_result;
}

// Locate the Cell (if any) which contains a specific Tile.
bool BaseBoard::LocateTile(Tile::IdType id, Cell& rCell) const {
    bool result = false;
    if (Tile::IsValid(id)) {
        Tile const tile(id, false);
        TileConstIterator const i_tile = mTiles.find(tile);
        if (i_tile != mTiles.end()) {
            rCell = i_tile->second;
            result = true;
        }
    }

    return result;
}

// Reset the board.
void BaseBoard::MakeEmpty(void) {
    mNorthMax = 0;
    mSouthMax = 0;
    mEastMax = 0;
    mWestMax = 0;
    mCells.clear();
    mTiles.clear();
}

// Make a specific cell empty.
void BaseBoard::MakeEmpty(Cell const& rCell) {
    RowType const row = rCell.Row();
    ColumnType const column = rCell.Column();

    CellIterator i_cell = Find(row, column);
    Tile const tile = i_cell->second;
    ASSERT(i_cell != mCells.end());
    mCells.erase(i_cell);

    TileIterator const i_tile = mTiles.find(tile);
    ASSERT(i_tile != mTiles.end());
    mTiles.erase(i_tile);

    // Shrink the limits as needed.
    while (row != 0 && IsEmptyRow(row)) {
        if (row == mNorthMax) {
            mNorthMax --;
        } else if (row == -mSouthMax) {
            mSouthMax --;
        } else {
            break;
        }
    }
    while (column != 0 && IsEmptyColumn(column)) {
        if (column == mEastMax) {
            mEastMax --;
        } else if (column == -mWestMax) {
            mWestMax --;
        } else {
            break;
        }
    }
}

// get the northern limit of the board
RowType BaseBoard::NorthMax(void) const {
    return mNorthMax;
}

// Play a tile on a specific cell.
void BaseBoard::PlayOnCell(Cell const& rCell, Tile const& rTile) {
    ASSERT(GetCell(rCell) == NULL);
    ASSERT(mTiles.find(rTile) == mTiles.end());

    // expand the limits as needed
    RowType const row = rCell.Row();
    if (row > mNorthMax) {
        mNorthMax = row;
    }
    if (row < -mSouthMax) {
        mSouthMax = -row;
    }

    ColumnType const column = rCell.Column();
    if (column > mEastMax) {
        mEastMax = column;
    }
    if (column < -mWestMax) {
        mWestMax = -column;
    }

    mCells[rCell] = rTile;
    mTiles[rTile] = rCell;

    ASSERT(GetCell(rCell) != NULL);
    ASSERT(mTiles.find(rTile) != mTiles.end());
}

// get the southern limit of the board
RowType BaseBoard::SouthMax(void) const {
    return mSouthMax;
}

// get the western limit of the board
ColumnType BaseBoard::WestMax(void) const {
    return mWestMax;
}


// inquiry methods

bool BaseBoard::IsEmptyColumn(ColumnType column) const {
    bool result = true;

    for (RowType row = -mSouthMax; row <= mNorthMax; row++) {
        CellConstIterator const i_cell = Find(row, column);
        if (i_cell != mCells.end()) {
            result = false;
            break;
        }
    }

    return result;
}

bool BaseBoard::IsEmptyRow(RowType row) const {
    bool result = true;

    for (ColumnType column = -mWestMax; column <= mEastMax; column++) {
        CellConstIterator const i_cell = Find(row, column);
        if (i_cell != mCells.end()) {
            result = false;
            break;
        }
    }

    return result;
}
