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


// misc

// get iterators to specific cells
BaseBoard::ConstIteratorType BaseBoard::Find(int northing, int easting) const {
    Cell ref(northing, easting);
    ConstIteratorType result = mCells.find(ref);

    return result;
}
BaseBoard::IteratorType BaseBoard::Find(int northing, int easting) {
    Cell ref(northing, easting);
    IteratorType result = mCells.find(ref);

    return result;
}

// locate the Cell which contains a specific Tile
bool BaseBoard::LocateTileId(TileIdType id, Cell &rSquare) const {
    for (int row = mNorthMax; row >= -(int)mSouthMax; row--) {
	    for (int column = -mWestMax; column <= mEastMax; column++) {
            ConstIteratorType i_tile = Find(row, column);
			if (i_tile != mCells.end() && i_tile->second.HasId(id)) {
                rSquare = i_tile->first;
                return true;
            }
        }
    }
    return false;
}

// make a specific cell empty
void BaseBoard::MakeEmpty(Cell const &rSquare) {
    int row = rSquare.Row();
    int column = rSquare.Column();
    IteratorType it = Find(row, column);
    ASSERT(it != mCells.end());
    mCells.erase(it);
}

// play a Tile on a specific cell
void BaseBoard::PlayOnCell(Cell const &rRef, Tile const &rTile) {
    D(std::cout << "Play " << (String)rTile << " on " << (String)rRef << std::endl);

    ASSERT(GetCell(rRef) == NULL);

    int n = rRef.Row();
    if (n > (int)mNorthMax) {
        mNorthMax = n;
    }
    if (n < -(int)mSouthMax) {
        mSouthMax = -n;
    }

    int e = rRef.Column();
    if (e > (int)mEastMax) {
        mEastMax = e;
    }
    if (e < -(int)mWestMax) {
        mWestMax = -e;
    }
    mCells[rRef] = rTile;
}


// access methods

// get the eastern limit of the board
int BaseBoard::EastMax(void) const {
    int result = mEastMax;

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

// get the northern limit of the board
int BaseBoard::NorthMax(void) const {
    int result = mNorthMax;

	return result;
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