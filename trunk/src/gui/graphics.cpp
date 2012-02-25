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

#include <math.h>
#include <vector>
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/win_types.hpp"

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
        mDraw = Win::CreateCompatibleDC(mDevice);
        ASSERT(mDraw != NULL);
        ASSERT(mDraw != mDevice);

        HGDIOBJ bitmap = Win::CreateCompatibleBitmap(mDevice, width, height);
        ASSERT(bitmap != NULL);
        mBitmapSave = Win::SelectObject(mDraw, bitmap);
        ASSERT(mBitmapSave != NULL);
    }

    // brush color for filling shapes
    mBrushBkColor = COLOR_WHITE;
    HBRUSH brush = Win::CreateSolidBrush(mBrushBkColor);
    ASSERT(brush != NULL);
    mBrushSave = Win::SelectObject(mDraw, brush);
    ASSERT(mBrushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    ColorType success = (ColorType)Win::SetBkColor(mDraw, mBrushBkColor);
    ASSERT(success != CLR_INVALID);

    // foreground color for text and broken lines
    // (same as pen color)
    success = (ColorType)Win::SetTextColor(mDraw, mPenTextColor);
    ASSERT(success != CLR_INVALID);

    // pen color for outlining shapes
    int penStyle = PS_SOLID;
    int penWidth = 0; // means single pixel
    mPenTextColor = COLOR_BLACK;
    HPEN pen = Win::CreatePen(penStyle, penWidth, mPenTextColor);
    ASSERT(pen != NULL);
    mPenSave = Win::SelectObject(mDraw, pen);
    ASSERT(mPenSave != NULL);
}

/* virtual */ Graphics::~Graphics(void) {
    HGDIOBJ brush = Win::SelectObject(mDraw, mBrushSave);
    ASSERT(brush != NULL);
    BOOL success = Win::DeleteObject(brush);
    ASSERT(success != 0);

    HGDIOBJ pen = Win::SelectObject(mDraw, mPenSave);
    ASSERT(pen != NULL);
    success = Win::DeleteObject(pen);
    ASSERT(success != 0);

    if (mDraw != mDevice) {
        Win::DeleteDC(mDraw);
    }
    if (mReleaseMe) {
        int success = Win::ReleaseDC(mWindow, mDevice);
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
        BOOL success = Win::BitBlt(mDevice, dest_x, dest_y, width, height, 
                                mDraw, source_x, source_y, options);
        ASSERT(success != 0);
         
        HGDIOBJ bitmap = Win::SelectObject(mDraw, mBitmapSave);
        ASSERT(bitmap != NULL);
        success = Win::DeleteObject(bitmap);
        ASSERT(success != 0);
    }
}

void Graphics::DrawEquilateral(Rect const &rBounds, bool pointDownFlag) {
	Poly triangle;
    triangle.Add(0.0, 0.0);
    triangle.Add(0.5, 1.0);
    triangle.Add(1.0, 0.0);
	DrawPolygon(triangle, rBounds, pointDownFlag);
}

void Graphics::DrawHexagon(Rect const &rBounds) {
	Poly hex;

	hex.Add(0.0, 0.5);
	hex.Add(0.25, 1.0);
	hex.Add(0.75, 1.0);
	hex.Add(1.0, 0.5);
	hex.Add(0.75, 0.0);
	hex.Add(0.25, 0.0);
	DrawPolygon(hex, rBounds);
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
    BOOL success = Win::MoveToEx(mDraw, x1, y1, NULL);
	ASSERT(success);

	success = Win::LineTo(mDraw, x2, y2);
	ASSERT(success);
}

void Graphics::DrawPolygon(Poly const &rPolygon, Rect const &rBounds, bool invertFlag) {
    unsigned pointCnt = rPolygon.Count();
    POINT *points = new POINT[pointCnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, pointCnt, rBounds, invertFlag);

    BOOL success = Win::Polygon(mDraw, points, pointCnt);
    ASSERT(success);
} 

Rect Graphics::DrawRectangle(Rect const &rRect) {
	LogicalXType left = rRect.LeftX();
	LogicalYType top = rRect.TopY();
	LogicalXType right = rRect.RightX();
	LogicalYType bottom = rRect.BottomY();
	BOOL success = Win::Rectangle(mDraw, left, top, right, bottom);
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
	BOOL success = Win::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	Rect result(top, left, width, height);
	
	return result;
}

void Graphics::DrawRoundedSquare(
    Point const &rCenter,
    PCntType edge,
    PCntType circleDiameter)
{
    ASSERT(edge > circleDiameter);
    
    PCntType ellipse_width = circleDiameter;
    PCntType ellipse_height = circleDiameter;
    LogicalXType left_x = rCenter.X() - edge/2;
    LogicalXType right_x = left_x + edge;
    LogicalYType top_y = rCenter.Y() - edge/2;
    LogicalYType bottom_y = top_y + edge;
    BOOL success = Win::RoundRect(mDraw, left_x, top_y, right_x, bottom_y,
                             ellipse_width, ellipse_height);
    ASSERT(success != 0);
}

void Graphics::DrawText(Rect const &rRect, char const *text, char const *altText) {
	ASSERT(text != NULL);

	char const *str;

    if (altText == NULL || TextWidth(text) <= rRect.Width()) {
		str = text;
	} else {
		str = altText;
	}

    int str_length = ::strlen(str);

	UINT format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    RECT bounds = (RECT)rRect;
    BOOL success = Win::DrawText(mDraw, str, str_length, &bounds, format);
    ASSERT(success != 0);
}

void Graphics::GetColors(ColorType &rBrushBkColor, ColorType &rPenTextColor) const {
    rPenTextColor = mPenTextColor;
    rBrushBkColor = mBrushBkColor;
}

Rect Graphics::InteriorEquilateral(Rect const &rBounds, bool pointDownFlag) {
	FractionPair pair_ulc(0,0), pair_brc(0,0);
	if (pointDownFlag) {
       pair_ulc = FractionPair(0.291, 0.900);
	   pair_brc = FractionPair(0.709, 0.418);
	} else {
       pair_ulc = FractionPair(0.291, 0.582);
	   pair_brc = FractionPair(0.709, 0.100);
	}

    Point ulc = rBounds.Interpolate(pair_ulc);
    Point brc = rBounds.Interpolate(pair_brc);

	Rect result(ulc, brc);

	return result;
}

Rect Graphics::InteriorHexagon(Rect const &rBounds) {
	double sqrt_3 = ::sqrt(3.0);
	double den = 4 + 4*sqrt_3;
    FractionPair pair_ulc(2/den, (6 + 2*sqrt_3)/den);
    Point ulc = rBounds.Interpolate(pair_ulc);

	FractionPair pair_brc((2 + 4*sqrt_3)/den, (2*sqrt_3 - 2)/den);
    Point brc = rBounds.Interpolate(pair_brc);

	Rect result(ulc, brc);

	return result;
}

Rect Graphics::InteriorRoundedSquare(
    Point const &rCenter,
    PCntType edge,
    PCntType circleDiameter)
{
    LogicalXType left_x = rCenter.X() - edge/2;
    LogicalYType top_y = rCenter.Y() - edge/2;

	// estimate the interior sqaure
	PCntType radius = circleDiameter/2;
	PCntType pad = radius - (unsigned long)(0.7 * float(radius));
    top_y += pad;
    left_x += pad;
    edge -= 2*pad;
    Rect result(top_y, left_x, edge, edge);
    
    return result;
}

PCntType Graphics::TextHeight(void) const {
    PCntType result = 16;
    
    return result;
}

PCntType Graphics::TextWidth(char const *text) const {
    int length = ::strlen(text);
    SIZE extent;
    BOOL success = Win::GetTextExtentPoint32(mDraw, text, length, &extent);
	ASSERT(success != 0);
    PCntType result = extent.cx;
    
    return result;
}

void Graphics::UseColors(ColorType brushBkColor, ColorType penTextColor) {
    if (brushBkColor != mBrushBkColor) {
        HBRUSH brush = Win::CreateSolidBrush(brushBkColor);
        ASSERT(brush != NULL);
        HGDIOBJ old = Win::SelectObject(mDraw, brush);
        ASSERT(old != NULL);
        BOOL success = Win::DeleteObject(old);
        ASSERT(success != 0);

        ColorType oldColor = (ColorType)Win::SetBkColor(mDraw, brushBkColor);
        ASSERT(oldColor != CLR_INVALID);

        mBrushBkColor = brushBkColor;
    }
       
    if (penTextColor != mPenTextColor) {
        ColorType oldColor = (ColorType)Win::SetTextColor(mDraw, penTextColor);
        ASSERT(oldColor != CLR_INVALID);

        int penStyle = PS_SOLID;
        int penWidth = 0; // means a pen one pixel wide
        HPEN pen = Win::CreatePen(penStyle, penWidth, penTextColor);
        ASSERT(pen != NULL);
        HGDIOBJ old = Win::SelectObject(mDraw, pen);
        ASSERT(old != NULL);
        BOOL success = Win::DeleteObject(old);
        ASSERT(success != 0);
        
        mPenTextColor = penTextColor;
    }
}


