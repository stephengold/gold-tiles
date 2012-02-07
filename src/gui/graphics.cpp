// File:    graphics.cpp
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

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/color.hpp"
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/rect.hpp"
#include "string.hpp"

Graphics::Graphics(
    HDC device,
    HWND window,
    bool releaseMe,
    bool bufferFlag,
    unsigned width,
    unsigned height)
{
    mDevice = device;
    mWindow = window;
    mReleaseMe = releaseMe;
    mHeight = height;
    mWidth = width;
    
    if (!bufferFlag) {
        mDraw = device;

    } else { // double buffering
        mDraw = ::CreateCompatibleDC(mDevice);
        ASSERT(mDraw != NULL);
        ASSERT(mDraw != mDevice);

        HGDIOBJ bitmap = ::CreateCompatibleBitmap(mDevice, mWidth, mHeight);
        ASSERT(bitmap != NULL);
        mBitmapSave = ::SelectObject(mDraw, bitmap);
        ASSERT(mBitmapSave != NULL);
    }

    // brush color for filling shapes
    mBrushBkColor = COLOR_WHITE;
    HBRUSH brush = ::CreateSolidBrush(mBrushBkColor);
    ASSERT(brush != NULL);
    mBrushSave = ::SelectObject(mDraw, brush);
    ASSERT(mBrushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    ColorType success = (ColorType)::SetBkColor(mDraw, mBrushBkColor);
    ASSERT(success != CLR_INVALID);

    // foreground color for text and broken lines
    // (same as pen color)
    success = (ColorType)::SetTextColor(mDraw, mPenTextColor);
    ASSERT(success != CLR_INVALID);

    // pen color for outlining shapes
    int penStyle = PS_SOLID;
    int penWidth = 0; // means single pixel
    mPenTextColor = COLOR_BLACK;
    HPEN pen = ::CreatePen(penStyle, penWidth, mPenTextColor);
    ASSERT(pen != NULL);
    mPenSave = ::SelectObject(mDraw, pen);
    ASSERT(mPenSave != NULL);
}

Graphics::~Graphics(void) {
    HGDIOBJ brush = ::SelectObject(mDraw, mBrushSave);
    ASSERT(brush != NULL);
    BOOL success = ::DeleteObject(brush);
    ASSERT(success != 0);

    HGDIOBJ pen = ::SelectObject(mDraw, mPenSave);
    ASSERT(pen != NULL);
    success = ::DeleteObject(pen);
    ASSERT(success != 0);

    if (mDraw != mDevice) {
        ::DeleteDC(mDraw);
    }
    if (mReleaseMe) {
        int success = ::ReleaseDC(mWindow, mDevice);
        ASSERT(success != 0);
    }
}

// methods

void Graphics::Close(void) {
    if (mDraw != mDevice) {
        int dest_x = 0;
        int dest_y = 0;
        int source_x = 0;
        int source_y = 0;
        DWORD options = SRCCOPY;
        BOOL success = ::BitBlt(mDevice, dest_x, dest_y, mWidth, mHeight, 
                                mDraw, source_x, source_y, options);
        ASSERT(success != 0);
         
        HGDIOBJ bitmap = ::SelectObject(mDraw, mBitmapSave);
        ASSERT(bitmap != NULL);
        success = ::DeleteObject(bitmap);
        ASSERT(success != 0);
    }
}

void Graphics::DrawLine(int x1, int y1, int x2, int y2) {
    BOOL success = ::MoveToEx(mDraw, x1, y1, NULL);
	ASSERT(success);

	success = ::LineTo(mDraw, x2, y2);
	ASSERT(success);
}

void Graphics::DrawPolygon(Poly const &rPolygon, Rect const &rBounds) {
    Rect squared = rBounds.CenterSquare();
    
    unsigned numberOfPoints = rPolygon.Count();
    POINT *points = new POINT[numberOfPoints];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, numberOfPoints, squared);

    BOOL success = ::Polygon(mDraw, points, numberOfPoints);
    ASSERT(success);
} 

Rect Graphics::DrawRectangle(
	int top,
    int left,
    unsigned width,
    unsigned height)
{
	int right = left + width;
	int bottom = top + height;
	BOOL success = ::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	Rect result(top, left, width, height);
	
	return result;
}

Rect Graphics::DrawRoundedSquare(
    int top,
    int left,
    unsigned edge,
    unsigned circleDiameter)
{
    ASSERT(edge > circleDiameter);
    
    unsigned ellipseWidth = circleDiameter;
    unsigned ellipseHeight = circleDiameter;
    int bottom = top + edge;
    int right = left + edge;
    BOOL success = ::RoundRect(mDraw, left, top, right, bottom,
                             ellipseWidth, ellipseHeight);
    ASSERT(success != 0);
        
    Rect result(top, left, edge, edge);
    
    return result;
}

void Graphics::DrawText(Rect const &rRect, char const *text) {
    int textLen = ::strlen(text);
     
    UINT format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    RECT r = (RECT)rRect;
    BOOL success = ::DrawText(mDraw, text, textLen, &r, format);
    ASSERT(success != 0);
}

void Graphics::DrawText(Rect const &rRect, String const &rText) {
    unsigned length = rText.Length();
    char *copyText = new char[length + 1];
    ::strcpy_s(copyText, length + 1, rText.c_str());
    DrawText(rRect, copyText);
    delete[] copyText;
}

void Graphics::GetColors(ColorType &rBrushBkColor, ColorType &rPenTextColor) const {
    rPenTextColor = mPenTextColor;
    rBrushBkColor = mBrushBkColor;
}

unsigned Graphics::TextHeight(void) const {
    unsigned result = 16;
    
    return result;
}

unsigned Graphics::TextWidth(char const *text) const {
    int length = ::strlen(text);
    SIZE extent;
    BOOL success = ::GetTextExtentPoint32(mDraw, text, length, &extent);
    unsigned result = extent.cx;
    
    return result;
}

unsigned Graphics::TextWidth(String const &rText) const {
    unsigned length = rText.Length();
    char *copyText = new char[length + 1];
    ::strcpy_s(copyText, length + 1, rText.c_str());
    unsigned result = TextWidth(copyText);
    delete[] copyText;
    return result;
}

void Graphics::UseColors(ColorType brushBkColor, ColorType penTextColor) {
    if (brushBkColor != mBrushBkColor) {
        HBRUSH brush = ::CreateSolidBrush(brushBkColor);
        ASSERT(brush != NULL);
        HGDIOBJ old = ::SelectObject(mDraw, brush);
        ASSERT(old != NULL);
        BOOL success = ::DeleteObject(old);
        ASSERT(success != 0);

        ColorType oldColor = (ColorType)::SetBkColor(mDraw, brushBkColor);
        ASSERT(oldColor != CLR_INVALID);

        mBrushBkColor = brushBkColor;
    }
       
    if (penTextColor != mPenTextColor) {
        ColorType oldColor = (ColorType)::SetTextColor(mDraw, penTextColor);
        ASSERT(oldColor != CLR_INVALID);

        int penStyle = PS_SOLID;
        int penWidth = 0; // means a pen one pixel wide
        HPEN pen = ::CreatePen(penStyle, penWidth, penTextColor);
        ASSERT(pen != NULL);
        HGDIOBJ old = ::SelectObject(mDraw, pen);
        ASSERT(old != NULL);
        BOOL success = ::DeleteObject(old);
        ASSERT(success != 0);
        
        mPenTextColor = penTextColor;
    }
}

#endif
