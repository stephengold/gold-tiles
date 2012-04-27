#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

// File:     canvas.hpp
// Location: src/gui
// Purpose:  declare Canvas class
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
A Canvas object represents the client area of the game window.

The Canvas class extends the Graphics class with features specific to the
Gold Tile Game.
*/

#include <vector>           // HASA std::vector
#include "gui/graphics.hpp" // ISA Graphics
#include "gui/markings.hpp" // USES Markings
#include "tile.hpp"         // USES AttrCntType

class Canvas: public Graphics {
public:
	// public lifecycle
    // no default constructor
    explicit Canvas(Window&);
    // ~Canvas(void);  compiler-generated destructor is OK
        
	// misc public methods
    void DrawBlankTile(Point const&, PixelCntType width, PixelCntType height, 
		     ColorType, bool odd);
	Rect DrawCell(Point const&, PixelCntType width, PixelCntType height,
             ColorType cellColor, ColorType gridColor, bool odd);
    void DrawTargetArrow(Rect const&);
    Rect DrawTile(Markings const&, ColorType tileColor, Point const& center, 
		     PixelCntType width, PixelCntType height, bool border, bool odd);
	static Rect 
		 InteriorGridShape(Point const& center, PixelCntType width, 
		     PixelCntType height, bool odd);

private:
	// private constants
	static const unsigned TILE_POINTEDNESS = 3; // corners of square tiles

	// private data
    static std::vector<Poly> msShapes; // TODO more choices
	static Poly msTargetArrow;
    
	// private lifecycle
    Canvas(Canvas const&);  // not copyable

	// private operators
    Canvas& operator=(Canvas const&);  // not assignable

	// misc private methods
	void        DrawGridShape(Point const& center, PixelCntType width, 
		            PixelCntType height, bool oddFlag);
    void        DrawMarking(Rect const& bounds, AttrModeType, AttrType, ColorType bg,
                    ColorType fg);
    static void	InitializeShapes(void);         
    static void	InitializeTargetArrow(void);            
};
#endif // !defined(CANVAS_HPP_INCLUDED)
