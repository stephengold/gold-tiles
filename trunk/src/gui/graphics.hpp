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
A Graphics object is used to draw shapes and text to the client area
of a Window.

The Graphics class is implemented by encapsulating a pair of GDI device 
contexts:  mDraw for drawing and mDevice for the physical device.
*/

#include "gui/color.hpp"  // HASA ColorType
#include "gui/rect.hpp"   // HASA Rect

class Graphics {
public:
    // public lifecycle
    Graphics(Win::HDC, Window &, bool releaseMe, bool doubleBufferingOption);
	// no default constructor
	virtual ~Graphics(void);

	// misc public methods
    void         Close(void);
	void         DrawEquilateral(Rect const &, bool invert);
	void         DrawHexagon(Rect const &);
	void         DrawLine(Point const &, Point const &);
	void         DrawLine(LogicalXType, LogicalYType, LogicalXType, LogicalYType);
    void         DrawPolygon(Poly const &, Rect const &, bool invert = false, bool fill = true);
    Rect         DrawRectangle(Rect const &);
    Rect         DrawRectangle(LogicalYType, LogicalXType, PixelCntType width, PixelCntType height);
    void         DrawRoundedSquare(Point const &, PixelCntType edge, PixelCntType diameter);
    void         DrawText(Rect const &, char const *, char const *alt = NULL);
    void         GetColors(ColorType &brushBk, ColorType &penText) const;
	static Rect  InteriorEquilateral(Rect const &, bool invert);
	static Rect  InteriorHexagon(Rect const &);
    static Rect  InteriorRoundedSquare(Point const &, PixelCntType edge, PixelCntType diameter);
    PixelCntType TextHeight(void) const;
    PixelCntType TextWidth(char const *) const;
    void         UseColors(ColorType brushBk, ColorType penText);

private:
	// private data
	Win::HGDIOBJ mBitmapSave, mBrushSave, mPenSave;
    ColorType    mBrushBkColor, mPenTextColor;
    Win::HDC     mDevice, mDraw;
    Rect         mRect;
    bool         mReleaseMe;
    Win::HWND    mWindow;

	// private lifecycle
	Graphics(Graphics const &);  // not copyable

	// private operators
    Graphics &operator=(Graphics const &);  // not assignable
};
#endif // !defined(GRAPHICS_HPP_INCLUDED)
