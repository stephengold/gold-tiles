#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

// File:    canvas.hpp
// Purpose: Canvas class
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
A Canvas object represents the client area of the top window.

The Canvas class extends the Graphics class with features specific to the
Gold Tile Game.

*/

#include <vector>            // HASA std::vector
#include "gui/graphics.hpp"  // ISA Graphics
#include "tile.hpp"          // USES ACountType

class Canvas: public Graphics {
public:
	// public lifecycle
    Canvas(Win::HDC, Win::HWND, bool releaseMe, PCntType width, PCntType height);
    // no default constructor
    // ~Canvas(void);  compiler-generated destructor is OK
        
	// misc public methods
    void        DrawBlankTile(Point const &, PCntType width, PCntType height, 
		             ColorType, bool odd);
	Rect        DrawCell(Point const &, PCntType width, PCntType height,
                     ColorType cellColor, ColorType gridColor, bool odd);
    void        DrawTarget(Rect const &);
    Rect        DrawTile(Point const &, PCntType width, PCntType height,
                     ACountType numGlyphs, const AValueType glyphs[],
                     ColorType tile, ColorType glyph, bool odd);
	static Rect InteriorGridShape(Point const &rCenter, PCntType width, 
		             PCntType height, bool oddFlag);

private:
	// private constants
	static const unsigned TILE_POINTEDNESS = 3;

	// private data
    static std::vector<Poly> msShapes; // TODO more options
    
	// private lifecycle
    Canvas(Canvas const &);  // not copyable

	// private operators
    Canvas &operator=(Canvas const &);  // not assignable

	// misc private methods
    void        DrawGlyph(Rect const &rBounds, AIndexType, AValueType, 
                   ColorType background, ColorType glyph);
	void        DrawGridShape(Point const &rCenter, PCntType width, 
		               PCntType height, bool oddFlag);
    static void InitShapes(void);            
};

#endif
