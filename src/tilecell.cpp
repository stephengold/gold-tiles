// File:    tilecell.cpp
// Purpose: TileCell class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "string.hpp"
#include "tilecell.hpp"

// constructors, assignment, destructor, and casts

TileCell::TileCell(void) {
	mSwapFlag = true;
}

TileCell::TileCell(Tile const &rTile):
    mTile(rTile)
{
	mSwapFlag = true;
}

TileCell::TileCell(Tile const &rTile, Cell const &rCell):
    mTile(rTile), mCell(rCell)
{
	mSwapFlag = false;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated assignment method is OK.
// The compiler-generated destructor is OK.

TileCell::operator Cell(void) const {
	ASSERT(!mSwapFlag);
    Cell result = mCell;

	return result;
}

TileCell::operator String(void) const {
	String result = String(mTile);
	if (mSwapFlag) {
		result += "@swap";
	} else {
		result += "@";
		result += String(mCell);
	}

	return result;
}

TileCell::operator Tile(void) const {
    Tile result = mTile;

	return result;
}

// methods

String TileCell::GetUserChoice(Tiles const &rAvailableTiles, Strings const &rAlts) {
    String result = mTile.GetUserChoice(rAvailableTiles, rAlts);
	if (result == String(mTile)) {
        mSwapFlag = mCell.GetUserChoice("swap");
	}
	return result;
}

bool TileCell::IsSwap(void) const {
	bool result = mSwapFlag;

	return result;
}

bool TileCell::operator<(TileCell const &rOther) const {
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