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

#ifndef __STDC_WANT_SECURE_LIB__
// for envs that don't provide strcpy_s()
#define strcpy_s(a, sz, b) strcpy(a, b)
#endif

// lifecycle

Graphics::Graphics(
    HDC device,
    HWND window,
    bool releaseMe,
    bool bufferFlag,
    PCntType width,
    PCntType height):
    mRect(0, 0, width, height)
{
    mDevice = device;
    mWindow = window;
    mReleaseMe = releaseMe;
    
    if (!bufferFlag) {
        mDraw = device;

    } else { // double buffering
        mDraw = ::CreateCompatibleDC(mDevice);
        ASSERT(mDraw != NULL);
        ASSERT(mDraw != mDevice);

        HGDIOBJ bitmap = ::CreateCompatibleBitmap(mDevice, width, height);
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

// misc methods

void Graphics::Close(void) {
    if (mDraw != mDevice) {
        LogicalXType dest_x = 0;
        LogicalYType dest_y = 0;
        PCntType width = mRect.Width();
        PCntType height = mRect.Height();
        LogicalXType source_x = 0;
        LogicalYType source_y = 0;
        DWORD options = SRCCOPY;
        BOOL success = ::BitBlt(mDevice, dest_x, dest_y, width, height, 
                                mDraw, source_x, source_y, options);
        ASSERT(success != 0);
         
        HGDIOBJ bitmap = ::SelectObject(mDraw, mBitmapSave);
        ASSERT(bitmap != NULL);
        success = ::DeleteObject(bitmap);
        ASSERT(success != 0);
    }
}

void Graphics::DrawLine(Point const &rPoint1, Point const &rPoint2) {
    LogicalXType x1 = rPoint1.X();
    LogicalYType y1 = rPoint1.Y();
    LogicalXType x2 = rPoint2.X();
    LogicalYType y2 = rPoint2.Y();

    DrawLine(x1, y1, x2, y2);
}

void Graphics::DrawLine(
    LogicalXType x1,
    LogicalYType y1,
    LogicalXType x2,
    LogicalYType y2)
{
    BOOL success = ::MoveToEx(mDraw, x1, y1, NULL);
	ASSERT(success);

	success = ::LineTo(mDraw, x2, y2);
	ASSERT(success);
}

void Graphics::DrawPolygon(Poly const &rPolygon, Rect const &rBounds) {
    Rect squared = rBounds.CenterSquare();
    
    unsigned pointCnt = rPolygon.Count();
    POINT *points = new POINT[pointCnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, pointCnt, squared);

    BOOL success = ::Polygon(mDraw, points, pointCnt);
    ASSERT(success);
} 

Rect Graphics::DrawRectangle(Rect const &rRect) {
	LogicalXType left = rRect.LeftX();
	LogicalYType top = rRect.TopY();
	LogicalXType right = rRect.RightX();
	LogicalYType bottom = rRect.BottomY();
	BOOL success = ::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	return rRect;
}

Rect Graphics::DrawRectangle(
	LogicalYType top,
    LogicalXType left,
    PCntType width,
    PCntType height)
{
	LogicalXType right = left + width;
	LogicalYType bottom = top + height;
	BOOL success = ::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	Rect result(top, left, width, height);
	
	return result;
}

Rect Graphics::DrawRoundedSquare(
    Point const &rPoint,
    PCntType edge,
    PCntType circleDiameter)
{
    ASSERT(edge > circleDiameter);
    
    PCntType ellipseWidth = circleDiameter;
    PCntType ellipseHeight = circleDiameter;
    LogicalXType leftX = rPoint.X();
    LogicalXType rightX = leftX + edge;
    LogicalYType topY = rPoint.Y();
    LogicalYType bottomY = topY + edge;
    BOOL success = ::RoundRect(mDraw, leftX, topY, rightX, bottomY,
                             ellipseWidth, ellipseHeight);
    ASSERT(success != 0);

    // estimate the dimensions of the interior rectangle        
    topY += circleDiameter/2;
    leftX += circleDiameter/2;
    edge -= circleDiameter;
    Rect result(topY, leftX, edge, edge);
    
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
    char *copy_text = new char[length + 1];
    ::strcpy_s(copy_text, length + 1, rText.c_str());

    DrawText(rRect, copy_text);

    delete[] copy_text;
}

void Graphics::GetColors(ColorType &rBrushBkColor, ColorType &rPenTextColor) const {
    rPenTextColor = mPenTextColor;
    rBrushBkColor = mBrushBkColor;
}

PCntType Graphics::TextHeight(void) const {
    PCntType result = 16;
    
    return result;
}

PCntType Graphics::TextWidth(char const *text) const {
    int length = ::strlen(text);
    SIZE extent;
    BOOL success = ::GetTextExtentPoint32(mDraw, text, length, &extent);
    PCntType result = extent.cx;
    
    return result;
}

PCntType Graphics::TextWidth(String const &rText) const {
    unsigned length = rText.Length();
    char *copy_text = new char[length + 1];
    
    ::strcpy_s(copy_text, length + 1, rText.c_str());
    PCntType result = TextWidth(copy_text);
    
    delete[] copy_text;
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
