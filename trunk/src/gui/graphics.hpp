#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

// File:    graphics.hpp
// Purpose: Graphics class
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
 Encapsulates a GDI device context.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "color.hpp"
#include "rect.hpp"

class Graphics {
public:
    // lifecycle
    Graphics(HDC, HWND, bool releaseMe, bool buffer, 
              PCntType width, PCntType height);
	// no default constructor
	// no copy constructor
	~Graphics(void);

	// operators
    Graphics &operator=(Graphics const &) { ASSERT(false); };

	// misc public methods
    void     Close(void);
	void     DrawLine(Point const &, Point const &);
	void     DrawLine(LogicalXType, LogicalYType, LogicalXType, LogicalYType);
    void     DrawPolygon(Poly const &, Rect const &);
    Rect     DrawRectangle(Rect const &);
    Rect     DrawRectangle(LogicalYType, LogicalXType, PCntType width, PCntType height);
    Rect     DrawRoundedSquare(Point const &, PCntType edge, PCntType diameter);
    void     DrawText(Rect const &, char const *);
    void     DrawText(Rect const &, String const &);
    void     GetColors(ColorType &rBrushBk, ColorType &rPenText) const;
    PCntType TextHeight(void) const;
    PCntType TextWidth(char const *) const;
    PCntType TextWidth(String const &) const;
    void     UseColors(ColorType brushBk, ColorType penText);

private:
	HGDIOBJ   mBitmapSave, mBrushSave, mPenSave;
    ColorType mBrushBkColor, mPenTextColor;
    HDC       mDevice, mDraw;
    Rect      mRect;
    bool      mReleaseMe;
    HWND      mWindow;
};

#endif
#endif
