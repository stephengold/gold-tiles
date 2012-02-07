#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

// File:    canvas.hpp
// Purpose: Canvas class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
A Canvas object represents ...

The Canvas class is implemented ...
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <vector>
#include <windows.h>
#include "gui/graphics.hpp"
#include "tile.hpp"

class Canvas: public Graphics {
public:
	// lifecycle
    Canvas(void) { ASSERT(false); };
    Canvas(Canvas const &) { ASSERT(false); };
    Canvas(HDC, HWND, bool releaseMe, unsigned width, unsigned height);
    // ~Canvas(void);  compiler-generated destructor is OK
        
	// operators
    Canvas &operator=(Canvas const &) { ASSERT(false); };

	// misc
	Rect DrawCell(int top, int left, unsigned width,
               ColorType cellColor, ColorType gridColor);
    Rect DrawTile(int top, int left, unsigned width, 
               ACountType numGlyphs, const AValueType glyphs[],
               ColorType tile, ColorType glyph);

private:
    static std::vector<Poly> msGlyphs;
    
	// misc
    void DrawGlyph(Rect bounds, AIndexType, AValueType, 
                   ColorType background, ColorType glyph);
    void InitGlyphs(void);            
};

#endif
#endif
