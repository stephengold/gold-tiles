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
	for (AttrIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		AttrModeType const mode = Tile::DefaultDisplayMode(i_attr);
		ASSERT(mode >= ATTR_MODE_MIN);
		ASSERT(mode <= ATTR_MODE_MAX);

		mArray[i_attr] = mode;
	}
}

DisplayModes::DisplayModes(DisplayModes const &rOther) {
	for (AttrIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		AttrModeType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ATTR_MODE_MIN);
		ASSERT(mode <= ATTR_MODE_MAX);

		mArray[i_attr] = mode;
	}
}

// The compiler-generated destructor is OK.


// operators

DisplayModes &DisplayModes::operator=(DisplayModes const &rOther) {
	for (AttrIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
		AttrModeType const mode = rOther.mArray[i_attr];
		ASSERT(mode >= ATTR_MODE_MIN);
		ASSERT(mode <= ATTR_MODE_MAX);

		mArray[i_attr] = mode;
	}

	return *this;
}


// misc methods

// clean up  display modes for the start of a new game
void DisplayModes::Cleanup(void) {
	// can't display more than 4 glyphs per tile
	AttrCntType const glyph_cnt = GlyphCnt();
	if (glyph_cnt > Markings::GLYPH_CNT_MAX) {
		AttrModeType const mode = ATTR_MODE_COLOR;
		SetMode(0, mode);
	}

	// only one attribute can use color
	while (ColorCnt() > 1) {
		AttrIndexType const ind = SecondColorIndex();
		AttrModeType const mode = ATTR_MODE_MIN;
		SetMode(ind, mode);
	}

	ASSERT(ColorCnt() <= 1);
	ASSERT(GlyphCnt() <= Markings::GLYPH_CNT_MAX);
}

AttrCntType DisplayModes::ColorCnt(void) const {
	AttrIndexType result = 0;
	for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		if (IsColor(i_attr)) {
			result++;
		}
	}

	return result;
}

AttrCntType DisplayModes::GlyphCnt(void) const {
	AttrCntType result = 0;
	for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		if (!IsColor(i_attr)) {
			result++;
		}
	}

	return result;
}

AttrModeType DisplayModes::Mode(AttrIndexType ind) const {
	ASSERT(ind < Tile::ATTRIBUTE_CNT_MAX);

	AttrModeType const result = mArray[ind];

	ASSERT(result >= ATTR_MODE_MIN);
	ASSERT(result <= ATTR_MODE_MAX);
	return result;
}

AttrIndexType DisplayModes::SecondColorIndex(void) const {
	AttrCntType count = 0;
	for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		AttrModeType const mode = Mode(i_attr);
		if (mode == ATTR_MODE_COLOR) {
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

void DisplayModes::SetMode(AttrIndexType ind, AttrModeType mode) {
	ASSERT(ind < Tile::ATTRIBUTE_CNT_MAX);
	ASSERT(mode >= ATTR_MODE_MIN);
	ASSERT(mode <= ATTR_MODE_MAX);

	mArray[ind] = mode;
}


// inquiry methods
bool DisplayModes::IsColor(AttrIndexType ind) const {
	AttrModeType const mode = Mode(ind);
	bool const result = (mode == ATTR_MODE_COLOR);

	return result;
}
