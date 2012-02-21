// File:    baseboard.cpp
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

#include <iostream>
#include "baseboard.hpp"
#include "string.hpp"

// lifecycle

// construct an empty board
BaseBoard::BaseBoard(void) {
    mNorthMax = 0;
    mSouthMax = 0;
    mEastMax = 0;
    mWestMax = 0;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment operator is OK.

// convert the entire board to a string
BaseBoard::operator String(void) const {
    D(std::cout << "maxN=" << mNorthMax << " maxS=" << mSouthMax 
		   << " maxE=" << mEastMax << " maxW=" << mWestMax
           << " in Board::operator String()" << std::endl);

    String result(5, ' ');
    for (int column = -(int)mWestMax; column <= (int)mEastMax; column++) {
       String column_tag(column);
       result += String(5 - column_tag.Length(), ' ');
       result += column_tag;
    }
    result += "\n";
    for (int row = (int)mNorthMax; row >= -(int)mSouthMax; row--) {
        String row_tag(row);
        result += String(5 - row_tag.Length(), ' ');
        result += row_tag;
	    for (int column = -(int)mWestMax; column <= (int)mEastMax; column++) {
            ConstIteratorType it = Find(row, column);
			if (it == mCells.end()) {
				result += " .";
                result += Tile::StringEmpty();
                result += ".";
			} else {
                Tile tile = it->second;
				result += " [";
                result += String(tile);
                result += "]";
			}
		}
		result += "\n";
	}

	return result;
}


// misc methods

unsigned BaseBoard::Count(void) const {
	unsigned result = mCells.size();
	ASSERT(mTiles.size() == result);

	return result;
}

// get the eastern limit of the board
int BaseBoard::EastMax(void) const {
    int result = mEastMax;

	return result;
}

// get iterators to specific cells
BaseBoard::ConstIteratorType BaseBoard::Find(IndexType northing, IndexType easting) const {
    Cell ref(northing, easting);
    ConstIteratorType result = mCells.find(ref);

    return result;
}
BaseBoard::IteratorType BaseBoard::Find(IndexType northing, IndexType easting) {
    Cell ref(northing, easting);
    IteratorType result = mCells.find(ref);

    return result;
}

// get a pointer to the Tile (if any) in a specific cell
Tile const *BaseBoard::GetCell(Cell const &rSquare) const {
    Tile const *p_result = NULL;

    ConstIteratorType it = mCells.find(rSquare);
    if (it != mCells.end()) {
        p_result = &(it->second);
    }

    return p_result;
}

// locate the Cell which contains a specific Tile
bool BaseBoard::LocateTileId(TileIdType id, Cell &rCell) const {
    ConstTileIteratorType i_tile = mTiles.find(id);
    bool result = (i_tile != mTiles.end());
    if (result) {
        rCell = i_tile->second;
    }
    return result;
}

// reset the board
void BaseBoard::MakeEmpty(void) {
	mNorthMax = 0;
	mSouthMax = 0;
	mEastMax = 0;
	mWestMax = 0;
    mCells.clear();
    mTiles.clear();
}

// make a specific cell empty
void BaseBoard::MakeEmpty(Cell const &rCell) {
    int row = rCell.Row();
    int column = rCell.Column();
    
    IteratorType i_cell = Find(row, column);
    Tile tile = i_cell->second;
    TileIdType id = tile.Id();
    ASSERT(i_cell != mCells.end());
    mCells.erase(i_cell);
    
    TileIteratorType i_tile = mTiles.find(id);
    ASSERT(i_tile != mTiles.end());
    mTiles.erase(i_tile);
}

// get the northern limit of the board
int BaseBoard::NorthMax(void) const {
    int result = mNorthMax;

	return result;
}

// play a Tile on a specific cell
void BaseBoard::PlayOnCell(Cell const &rCell, Tile const &rTile) {
    ASSERT(GetCell(rCell) == NULL);
    TileIdType id = rTile.Id();
    ASSERT(mTiles.find(id) == mTiles.end());

    int n = rCell.Row();
    if (n > (int)mNorthMax) {
        mNorthMax = n;
    }
    if (n < -(int)mSouthMax) {
        mSouthMax = -n;
    }

    int e = rCell.Column();
    if (e > (int)mEastMax) {
        mEastMax = e;
    }
    if (e < -(int)mWestMax) {
        mWestMax = -e;
    }

    mCells[rCell] = rTile;
    mTiles[id] = rCell;

    ASSERT(GetCell(rCell) != NULL);
    ASSERT(mTiles.find(id) != mTiles.end());
}

// get the southern limit of the board
int BaseBoard::SouthMax(void) const {
    int result = mSouthMax;

	return result;
}

// get the western limit of the board
int BaseBoard::WestMax(void) const {
    int result = mWestMax;

	return result;
}
