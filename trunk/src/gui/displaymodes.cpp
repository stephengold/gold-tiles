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
#include "gui/markings.hpp"


// lifecycle

DisplayModes::DisplayModes(void) {
	for (AIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		ADisplayType const mode = Tile::DefaultDisplayMode(i_attr);
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}
}

DisplayModes::DisplayModes(DisplayModes const &rOther) {
	for (AIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		ADisplayType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}
}

// The compiler-generated destructor is OK.


// operators

DisplayModes &DisplayModes::operator=(DisplayModes const &rOther) {
	for (AIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		ADisplayType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ADISPLAY_MIN);
		ASSERT(mode <= ADISPLAY_MAX);

		mArray[i_attr] = mode;
	}

	return *this;
}


// misc methods

// clean up  display modes for the start of a new game
void DisplayModes::Cleanup(void) {
	// can't display more than 4 glyphs per tile
	ACountType const glyph_cnt = GlyphCnt();
	if (glyph_cnt > Markings::GLYPH_CNT) {
		ADisplayType const mode = ADISPLAY_COLOR;
		SetMode(0, mode);
	}

	// only one attribute can use color
	while (ColorCnt() > 1) {
		AIndexType const ind = SecondColorIndex();
		ADisplayType const mode = ADISPLAY_MIN;
		SetMode(ind, mode);
	}

	ASSERT(ColorCnt() <= 1);
	ASSERT(GlyphCnt() <= Markings::GLYPH_CNT);
}

ACountType DisplayModes::ColorCnt(void) const {
	AIndexType result = 0;
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		if (IsColor(i_attr)) {
			result++;
		}
	}

	return result;
}

ACountType DisplayModes::GlyphCnt(void) const {
	ACountType result = 0;
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		if (!IsColor(i_attr)) {
			result++;
		}
	}

	return result;
}

ADisplayType DisplayModes::Mode(AIndexType ind) const {
	ASSERT(ind < Tile::ATTRIBUTE_CNT_MAX);

	ADisplayType const result = mArray[ind];

	ASSERT(result >= ADISPLAY_MIN);
	ASSERT(result <= ADISPLAY_MAX);
	return result;
}

AIndexType DisplayModes::SecondColorIndex(void) const {
	ACountType count = 0;
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		ADisplayType const mode = Mode(i_attr);
		if (mode == ADISPLAY_COLOR) {
			count++;
			if (count == 2) {
				ASSERT(i_attr > 0);
				return i_attr;
			}
		}
	}

	FAIL();
	return 0;
}

void DisplayModes::SetMode(AIndexType ind, ADisplayType mode) {
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
