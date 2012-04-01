#ifndef MARKINGS_HPP_INCLUDED
#define MARKINGS_HPP_INCLUDED

// File:    markings.hpp
// Purpose: Markings class header file
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

/*
A Markings object represents information needed to draw a tile in the GUI.

The Markings class encapsulates the tile's foreground color, its glyphs,
and the display method for each glyph.
*/

#include "gui/color.hpp"  // HASA ColorType
#include "tile.hpp"       // HASA ACountType


class Markings {
public:
	// public constants
	static const ACountType GLYPH_CNT = 4;

	// public lifecycle
	Markings(Tile const &, DisplayModes const &);  
    // Markings(Markings const &); compiler-generated copy constructor is OK
    // ~Markings(void);  compiler-generated destructor is OK

	// public operators
    // Markings &operator=(Markings const &);  compiler-generated assignment method is OK

	// misc public methods
	AValueType   Glyph(AIndexType) const;
	ACountType   GlyphCnt(void) const;
	ColorType    GlyphColor(void) const;
	ADisplayType Mode(AIndexType) const;

private:
    // private constants
	static const ColorType msGlyphColors[Tile::VALUE_CNT_MAX];

	// private data
	ACountType   mGlyphCnt;
	ColorType    mGlyphColor;
	AValueType   mGlyphs[GLYPH_CNT];
	ADisplayType mModes[GLYPH_CNT];
};
#endif // !defined(MARKINGS_HPP_INCLUDED)
