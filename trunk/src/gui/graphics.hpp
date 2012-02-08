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

class Graphics {
public:
    // lifecycle
    Graphics(void) { ASSERT(false); };
    Graphics(Graphics const &) { ASSERT(false); };
    Graphics(HDC, HWND, bool releaseMe, bool buffer, 
                   unsigned width, unsigned height);
    ~Graphics(void);

	// operators
    Graphics &operator=(Graphics const &) { ASSERT(false); };

	// misc
    void     Close(void);
	void     DrawLine(int x1, int y1, int x2, int y2);
    void     DrawPolygon(Poly const &, Rect const &);
    Rect     DrawRectangle(int top, int left, unsigned width, unsigned height);
    Rect     DrawRoundedSquare(int top, int left, unsigned edge, unsigned diameter);
    void     DrawText(Rect const &, char const *);
    void     DrawText(Rect const &, String const &);
    unsigned TextWidth(char const *) const;
    unsigned TextWidth(String const &) const;
    void     UseColors(ColorType brushBk, ColorType penText);

	// access
    void     GetColors(ColorType &rBrushBk, ColorType &rPenText) const;
    unsigned TextHeight(void) const;

private:
	HGDIOBJ   mBitmapSave, mBrushSave, mPenSave;
    ColorType mBrushBkColor, mPenTextColor;
    HDC       mDevice, mDraw;
    unsigned  mHeight, mWidth;
    bool      mReleaseMe;
    HWND      mWindow;
};

#endif
#endif
