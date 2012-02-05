#ifndef COLOR_HPP_INCLUDED
#define COLOR_HPP_INCLUDED

// File:    color.hpp
// Purpose: ColorType for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

enum ColorType {
    COLOR_BLACK       = PALETTERGB(0, 0, 0),
    COLOR_DARK_GRAY   = PALETTERGB(80, 80, 80),
    COLOR_LIGHT_GRAY  = PALETTERGB(192, 192, 192),
    COLOR_WHITE       = PALETTERGB(255, 255, 255),

    COLOR_RED         = PALETTERGB(255, 0, 0),
    COLOR_PINK        = PALETTERGB(255, 128, 128),

    COLOR_BROWN       = PALETTERGB(120, 60, 0),
    COLOR_YELLOW      = PALETTERGB(255, 180, 0),
    
    COLOR_DARK_GREEN  = PALETTERGB(0, 100, 0),
    COLOR_GREEN       = PALETTERGB(0, 255, 0),
    COLOR_LIGHT_GREEN = PALETTERGB(64, 255, 64),

    COLOR_DARK_BLUE  = PALETTERGB(0, 0, 160),
    COLOR_LIGHT_BLUE = PALETTERGB(128, 128, 255),
    
    COLOR_PURPLE     = PALETTERGB(128, 0, 128)
};

#endif
#endif
