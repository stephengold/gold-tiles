#ifndef COLOR_HPP_INCLUDED
#define COLOR_HPP_INCLUDED

// File:     color.hpp
// Location: src/gui
// Purpose:  define ColorType enum
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

/*
The ColorType enum names and defines all colors used in the game.
*/

#include "gui/win.hpp"

using Win::BYTE;
using Win::COLORREF;
using Win::DWORD;
using Win::WORD;

enum ColorType {
    COLOR_BLACK       = PALETTERGB(0, 0, 0),
    COLOR_DARK_GRAY   = PALETTERGB(80, 80, 80),
	COLOR_MEDIUM_GRAY = PALETTERGB(128, 128, 128),
    COLOR_LIGHT_GRAY  = PALETTERGB(160, 160, 160),
    COLOR_WHITE       = PALETTERGB(255, 255, 255),

    COLOR_RED         = PALETTERGB(255, 0, 0),
    COLOR_PINK        = PALETTERGB(255, 128, 128),

    COLOR_BROWN       = PALETTERGB(120, 60, 0),
    COLOR_YELLOW      = PALETTERGB(255, 180, 0),
    COLOR_DULL_GOLD   = PALETTERGB(160, 140, 0),
    COLOR_GOLD        = PALETTERGB(255, 204, 0),
    
    COLOR_DARK_GREEN  = PALETTERGB(0, 100, 0),
    COLOR_GREEN       = PALETTERGB(0, 255, 0),
    COLOR_LIGHT_GREEN = PALETTERGB(64, 255, 64),

    COLOR_DARK_BLUE  = PALETTERGB(0, 0, 160),
    COLOR_LIGHT_BLUE = PALETTERGB(128, 128, 255),
    
    COLOR_PURPLE     = PALETTERGB(128, 0, 128),

	COLOR_TRANSPARENT = -1
};
#endif // !defined(COLOR_HPP_INCLUDED)
