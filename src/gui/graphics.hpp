#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

// File:    graphics.hpp
// Purpose: Graphics class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 Encapsulates a GDI device context.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

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
