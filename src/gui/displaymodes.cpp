// File:    displaymodes.cpp
// Purpose: DisplayModes class
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

#include "gui/displaymodes.hpp"
#include "tile.hpp"


// lifecycle

DisplayModes::DisplayModes(void) {
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		ASSERT(i_attr < Tile::ATTRIBUTE_CNT_MAX);

		ADisplayType const mode = Tile::DefaultDisplayMode(i_attr);
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}
}

DisplayModes::DisplayModes(DisplayModes const &rOther) {
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		ASSERT(i_attr < Tile::ATTRIBUTE_CNT_MAX);

		ADisplayType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}
}

// The compiler-generated destructor is OK.


// operators

DisplayModes &DisplayModes::operator=(DisplayModes const &rOther) {
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		ASSERT(i_attr < Tile::ATTRIBUTE_CNT_MAX);

		ADisplayType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}

	return *this;
}


// misc methods

ADisplayType DisplayModes::Mode(AIndexType ind) const {
	ASSERT(ind < Tile::AttributeCnt());
	ASSERT(ind < Tile::ATTRIBUTE_CNT_MAX);

	ADisplayType const result = mArray[ind];

	ASSERT(result >= ADISPLAY_MIN);
	ASSERT(result <= ADISPLAY_MAX);
	return result;
}

void DisplayModes::SetMode(AIndexType ind, ADisplayType mode) {
	ASSERT(ind < Tile::AttributeCnt());
	ASSERT(ind < Tile::ATTRIBUTE_CNT_MAX);
	ASSERT(mode >= ADISPLAY_MIN);
	ASSERT(mode <= ADISPLAY_MAX);

	mArray[ind] = mode;
}


// inquiry methods
bool DisplayModes::IsColor(AIndexType ind) const {
	ADisplayType const mode = Mode(ind);
	bool const result = (mode == ADISPLAY_COLOR);

	return result;
}
