// File:    markings.cpp
// Purpose: Markings class
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

#include <vector>
#include "gui/displaymodes.hpp"
#include "gui/markings.hpp"


// static data

const ColorType Markings::msGlyphColors[Tile::VALUE_CNT_MAX] = {
    COLOR_BLACK,      COLOR_RED,       COLOR_DARK_BLUE, 
    COLOR_DARK_GREEN, COLOR_PURPLE,    COLOR_BROWN, 
    COLOR_DARK_GRAY,  COLOR_PINK,      COLOR_LIGHT_BLUE
};


// lifecycle

Markings::Markings(Tile const &rTile, DisplayModes const &rDisplayModes) {
	mGlyphColor = COLOR_BLACK;
	AttrIndexType i_glyph = 0;

    for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		 AttrModeType const display_mode = rDisplayModes.Mode(i_attr);
		 AttrType const attr_value = rTile.Attribute(i_attr);
		 ASSERT(attr_value < Tile::VALUE_CNT_MAX);

		 if (display_mode == ATTR_MODE_COLOR) {
	         mGlyphColor = msGlyphColors[attr_value];
		 } else {
			 ASSERT(i_glyph < GLYPH_CNT_MAX);
			 mModes[i_glyph] = display_mode;
             mGlyphs[i_glyph] = attr_value;
			 i_glyph++;
		 }
    }

	mGlyphCnt = i_glyph;
	ASSERT(mGlyphCnt > 0);
	ASSERT(mGlyphCnt <= GLYPH_CNT_MAX);
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment operator is fine.


// misc methods

AttrType Markings::Glyph(AttrIndexType ind) const {
	ASSERT(ind < mGlyphCnt);

	AttrType result = mGlyphs[ind];

	ASSERT(result < Tile::VALUE_CNT_MAX);
	return result;
}

AttrCntType Markings::GlyphCnt(void) const {
    return mGlyphCnt;
}

ColorType Markings::GlyphColor(void) const {
	return mGlyphColor;
}

AttrModeType Markings::Mode(AttrIndexType ind) const {
	ASSERT(ind < mGlyphCnt);

	AttrModeType result = mModes[ind];

	return result;
}
