// File:     tilecell.cpp
// Location: src
// Purpose:  implement TileCell class
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

#include "tilecell.hpp"


// lifecycle

TileCell::TileCell(void) {
	mSwapFlag = true;
}

TileCell::TileCell(Tile const& rTile):
    mTile(rTile)
{
	mSwapFlag = true;
}

TileCell::TileCell(Tile const& rTile, Cell const& rCell):
    mTile(rTile), 
	mCell(rCell)
{
	mSwapFlag = false;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

bool TileCell::operator<(TileCell const& rOther) const {
     bool result;
     
     if (mTile == rOther.mTile) {
		 if (mSwapFlag == rOther.mSwapFlag) {
			 if (mSwapFlag) {
				 result = false;
			 } else {
				 result = (mCell < rOther.mCell);
			 }
		 } else {
             result = (mSwapFlag < rOther.mSwapFlag);
		 }
     } else {
         result = (mTile < rOther.mTile);
     }
     
     return result;
}

// The compiler-generated assignment method is OK.

TileCell::operator Cell(void) const {
	ASSERT(!mSwapFlag);

    return mCell;
}

TileCell::operator String(void) const {
	String result = String(mTile);
	result = result.Purge();

	if (IsSwap()) {
		result += "@swap";
	} else {
		result += "@";
		result += String(mCell);
	}

	return result;
}

TileCell::operator Tile(void) const {
    return mTile;
}


// misc methods

String TileCell::GetUserChoice(Tiles const& rAvailableTiles, Strings const& rAlternatives) {
    String const result = mTile.GetUserChoice(rAvailableTiles, rAlternatives);
	if (mTile.MatchesString(result)) {
        mSwapFlag = mCell.GetUserChoice("swap");
	}
	
	return result;
}


// inquiry methods

bool TileCell::IsSwap(void) const {
	return mSwapFlag;
}

