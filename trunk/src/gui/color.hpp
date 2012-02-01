#ifndef COLOR_HPP_INCLUDED
#define COLOR_HPP_INCLUDED

// File:    color.hpp
// Purpose: Color enum for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

enum Color {
    BLACK_COLOR       = PALETTERGB(0, 0, 0),
    DARK_GRAY_COLOR   = PALETTERGB(80, 80, 80),
    LIGHT_GRAY_COLOR  = PALETTERGB(192, 192, 192),
    WHITE_COLOR       = PALETTERGB(255, 255, 255),

    RED_COLOR         = PALETTERGB(255, 0, 0),
    PINK_COLOR        = PALETTERGB(255, 128, 128),

    BROWN_COLOR       = PALETTERGB(90, 40, 0),
    YELLOW_COLOR      = PALETTERGB(255, 180, 0),
    
    DARK_GREEN_COLOR  = PALETTERGB(0, 100, 0),
    GREEN_COLOR       = PALETTERGB(0, 255, 0),
    LIGHT_GREEN_COLOR = PALETTERGB(64, 255, 64),

    DARK_BLUE_COLOR  = PALETTERGB(0, 0, 160),
    LIGHT_BLUE_COLOR = PALETTERGB(128, 128, 255),
    
    PURPLE_COLOR     = PALETTERGB(128, 0, 128)
};

#endif
#endif
