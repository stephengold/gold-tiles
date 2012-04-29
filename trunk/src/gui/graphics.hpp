#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

// File:     graphics.hpp
// Location: src/gui
// Purpose:  declare Graphics class
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
A Graphics object is used to draw shapes and text to the client area
of a Window.

The Graphics class is implemented by encapsulating a pair of GDI device 
contexts:  mDraw for drawing and mDevice for the physical device.
*/

#include "gui/color.hpp"  // HASA ColorType
#include "gui/rect.hpp"   // HASA Rect


class Graphics {
public:
    // public constants
    static const PixelCntType FONT_HEIGHT_DEFAULT = 24;

    // public lifecycle
    // no default constructor
    Graphics(Win::HDC, Window&, bool releaseMe, bool doubleBufferingOption);
    virtual ~Graphics(void);

    // misc public methods
    void         Close(void);
    void         DrawEquilateral(Rect const&, bool invert);
    void         DrawHexagon(Rect const&);
    void         DrawLine(Point const&, Point const&);
    void         DrawLine(LogicalXType, LogicalYType, LogicalXType, LogicalYType);
    void         DrawPolygon(Poly const&, Rect const&, bool invert = false, bool fill = true);
    Rect         DrawRectangle(Rect const&);
    Rect         DrawRectangle(LogicalYType, LogicalXType, PixelCntType width, PixelCntType height);
    void         DrawRoundedSquare(Point const&, PixelCntType edge, PixelCntType diameter);
    void         DrawTextLine(Rect const&, TextType, TextType alt = NULL);
    void         DrawTextMultiline(Rect const&, TextType);
    static Rect  InteriorEquilateral(Rect const&, bool invert);
    static Rect  InteriorHexagon(Rect const&);
    static Rect  InteriorRoundedSquare(Point const&, PixelCntType edge, PixelCntType diameter);
    PixelCntType TextHeight(void) const;
    PixelCntType TextWidth(TextType) const;
    void         UseColors(ColorType brushBk, ColorType penText);
    void         UseFont(PixelCntType height);
    void         UseFont(PixelCntType height, PixelCntType width);

private:
    // private types
    typedef unsigned short TextSizeType;

    // private constants
    static const PixelCntType FONT_HEIGHT_MAX = 999;
    static const PixelCntType FONT_HEIGHT_MIN = 4;
    static const PixelCntType FONT_WIDTH_MIN = 4;
    static const TextSizeType TEXT_SIZE_MAX = 13;
    static const TextSizeType TEXT_SIZE_CNT = TEXT_SIZE_MAX + 1;

    // private data - saved GDI handles
    Win::HGDIOBJ mBitmapSave;
    Win::HGDIOBJ mBrushSave;
    Win::HGDIOBJ mFontSave;
    Win::HGDIOBJ mPenSave;

    // private data - current colors and fonts
    ColorType    mBackgroundColor;  // background color and mode for text and broken lines
    ColorType    mBrushColor;	    // brush color for filling shapes
    ColorType    mPenColor;         // pen color for broken lines
    ColorType    mTextColor;        // foreground color for text
    TextSizeType mTextSize;         // size for text

    // private data - preselected fonts
    Win::HFONT   mFonts[TEXT_SIZE_CNT];
    PixelCntType mFontHeight[TEXT_SIZE_CNT];
    PixelCntType mFontWidth[TEXT_SIZE_CNT];

    // private data - standard shapes
    static Poly msEquilateral;
    static Poly msHexagon;

    // private data - misc
    Win::HDC  mDevice;
    Win::HDC  mDraw;
    Rect      mRect;
    bool      mReleaseMe;
    Win::HWND mWindow;

    // private lifecycle
    Graphics(Graphics const&);  // not copyable

    // private operators
    Graphics& operator=(Graphics const&);  // not assignable

    // misc private methods
    void CreateFonts(void);
    TextSizeType 
         FindTextSize(PixelCntType height);
    TextSizeType 
         FindTextSize(PixelCntType height, PixelCntType width);
    static void
         InitializeEquilateral(void);
    static void
         InitializeHexagon(void);
    void MeasureFonts(void);
    static PixelCntType 
         NominalHeight(TextSizeType size);
    void SetBackgroundColor(ColorType);
    void SetBrushColor(ColorType);
    void SetPenColor(ColorType);
    void SetTextColor(ColorType);
    void SetTextSize(TextSizeType);
    void UseBrushBackgroundColors(ColorType);
    void UsePenTextColors(ColorType);
};
#endif // !defined(GRAPHICS_HPP_INCLUDED)
