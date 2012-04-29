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

#include "strings.hpp"
#include "tilecell.hpp"


// static constants

const String TileCell::SEPARATOR("@");
const String TileCell::SWAP("swap");


// lifecycle

TileCell::TileCell(void) {
    mSwapFlag = true;
}

TileCell::TileCell(Tile const& rTile):
mTile(rTile)
{
    mSwapFlag = true;
}

TileCell::TileCell(String const& rString, bool remoteFlag) {
    Strings const parts(rString, SEPARATOR);
    ASSERT(parts.Count() == 2); // TODO recovery

    String const first = parts.First();
    mTile = Tile(IndexType(first), remoteFlag);

    mSwapFlag = true;
    String const second = parts.Second();
    if (second != SWAP) {
        mCell = Cell(second);
        mSwapFlag = false;
    }
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

bool TileCell::operator!=(TileCell const& rOther) const {
    bool equal = (mTile == rOther.mTile
        && mSwapFlag == rOther.mSwapFlag);

    if (equal && !mSwapFlag) {
        equal = (mCell == rOther.mCell);
    }
    bool const result = !equal;

    return result;
}

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
    String result = String(mTile.Id()) + SEPARATOR;

    if (IsSwap()) {
        result += SWAP;
    } else {
        result += String(mCell);
    }

    return result;
}

TileCell::operator Tile(void) const {
    return mTile;
}


// misc methods

String TileCell::Description(void) const {
    String result = mTile.Description();

    if (IsSwap()) {
        result += " to swap area";
    } else {
        result += " on " + String(mCell);
    }

    return result;
}

String TileCell::GetUserChoice(Tiles const& rAvailableTiles, Strings const& rAlternatives) {
    String const result = mTile.GetUserChoice(rAvailableTiles, rAlternatives);
    if (TileOpt(mTile).MatchesDescription(result)) {
        mSwapFlag = mCell.GetUserChoice("swap");
    }

    return result;
}


// inquiry methods

bool TileCell::IsSwap(void) const {
    return mSwapFlag;
}

