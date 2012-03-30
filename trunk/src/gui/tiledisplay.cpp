// File:    tiledisplay.cpp
// Purpose: TileDisplay class
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
#include "gui/tiledisplay.hpp"

// static data

const ColorType TileDisplay::msGlyphColors[Tile::VALUE_CNT_MAX] = {
    COLOR_BLACK,      COLOR_RED,       COLOR_DARK_BLUE, 
    COLOR_DARK_GREEN, COLOR_PURPLE,    COLOR_BROWN, 
    COLOR_DARK_GRAY,  COLOR_PINK,      COLOR_LIGHT_BLUE
};


// lifecycle

TileDisplay::TileDisplay(Tile const &rTile,	DisplayModes const &rDisplayModes) {
	mGlyphColor = COLOR_BLACK;
	AIndexType i_glyph = 0;

    for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		 ADisplayType const display_mode = rDisplayModes.Mode(i_attr);
		 if (display_mode == ADISPLAY_COLOR) {
			 AValueType const color_index = rTile.Attribute(i_attr);
	         mGlyphColor = msGlyphColors[color_index];
		 } else {
			 ASSERT(i_glyph < GLYPH_CNT);
			 mModes[i_glyph] = display_mode;
             mGlyphs[i_glyph] = rTile.Attribute(i_attr);
			 i_glyph++;
		 }
    }

	mGlyphCnt = i_glyph;
	ASSERT(mGlyphCnt > 0);
	ASSERT(mGlyphCnt <= GLYPH_CNT);
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment operator is fine.


// misc methods

AValueType TileDisplay::Glyph(AIndexType ind) const {
	ASSERT(ind < mGlyphCnt);

	AValueType result = mGlyphs[ind];

	return result;
}

unsigned TileDisplay::GlyphCnt(void) const {
    return mGlyphCnt;
}

ColorType TileDisplay::GlyphColor(void) const {
	return mGlyphColor;
}

ADisplayType TileDisplay::Mode(AIndexType ind) const {
	ASSERT(ind < mGlyphCnt);

	ADisplayType result = mModes[ind];

	return result;
}
