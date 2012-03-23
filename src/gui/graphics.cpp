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

#ifdef _WINDOWS
#include <math.h>
#include <vector>
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/win_types.hpp"
#include "gui/window.hpp"

// lifecycle

Graphics::Graphics(HDC device, Window &rWindow, bool releaseMe, bool bufferFlag)
:
    mRect(Rect(rWindow))
{
    mDevice = device;
    mWindow = HWND(rWindow);
    mReleaseMe = releaseMe;
    
    if (!bufferFlag) {
        mDraw = device;

    } else { // double buffering
        mDraw = Win::CreateCompatibleDC(mDevice);
        ASSERT(mDraw != NULL);
        ASSERT(mDraw != mDevice);

		PCntType const width = mRect.Width();
		PCntType const height = mRect.Height();
        HGDIOBJ const bitmap = Win::CreateCompatibleBitmap(mDevice, width, height);
        ASSERT(bitmap != NULL);
        mBitmapSave = Win::SelectObject(mDraw, bitmap);
        ASSERT(mBitmapSave != NULL);
    }

    // brush color for filling shapes
    mBrushBkColor = COLOR_WHITE;
    HBRUSH const brush = Win::CreateSolidBrush(mBrushBkColor);
    ASSERT(brush != NULL);
    mBrushSave = Win::SelectObject(mDraw, brush);
    ASSERT(mBrushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    ColorType success = ColorType(Win::SetBkColor(mDraw, mBrushBkColor));
    ASSERT(success != CLR_INVALID);

    // foreground color for text and broken lines
    // (same as pen color)
    success = ColorType(Win::SetTextColor(mDraw, mPenTextColor));
    ASSERT(success != CLR_INVALID);

    // pen color for outlining shapes
    int const penStyle = PS_SOLID;
    int const penWidth = 0; // means single pixel
    mPenTextColor = COLOR_BLACK;
    HPEN const pen = Win::CreatePen(penStyle, penWidth, mPenTextColor);
    ASSERT(pen != NULL);
    mPenSave = Win::SelectObject(mDraw, pen);
    ASSERT(mPenSave != NULL);
}

/* virtual */ Graphics::~Graphics(void) {
    HGDIOBJ const brush = Win::SelectObject(mDraw, mBrushSave);
    ASSERT(brush != NULL);
    BOOL success = Win::DeleteObject(brush);
    ASSERT(success != 0);

    HGDIOBJ const pen = Win::SelectObject(mDraw, mPenSave);
    ASSERT(pen != NULL);
    success = Win::DeleteObject(pen);
    ASSERT(success != 0);

    if (mDraw != mDevice) {
        Win::DeleteDC(mDraw);
    }
    if (mReleaseMe) {
        int const success = Win::ReleaseDC(mWindow, mDevice);
        ASSERT(success != 0);
    }
}

// misc methods

void Graphics::Close(void) {
    if (mDraw != mDevice) {
		// copy buffer to device
        LogicalXType const dest_x = 0;
        LogicalYType const dest_y = 0;
        PCntType const width = mRect.Width();
        PCntType const height = mRect.Height();
        LogicalXType const source_x = 0;
        LogicalYType const source_y = 0;
        DWORD const options = SRCCOPY;
        BOOL success = Win::BitBlt(mDevice, dest_x, dest_y, width, height, 
                                mDraw, source_x, source_y, options);
        ASSERT(success != 0);
         
        HGDIOBJ const bitmap = Win::SelectObject(mDraw, mBitmapSave);
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
    LogicalXType const x1 = rPoint1.X();
    LogicalYType const y1 = rPoint1.Y();
    LogicalXType const x2 = rPoint2.X();
    LogicalYType const y2 = rPoint2.Y();

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
    unsigned const pointCnt = rPolygon.Count();
    POINT *const points = new POINT[pointCnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, pointCnt, rBounds, invertFlag);

    BOOL const success = Win::Polygon(mDraw, points, pointCnt);
    ASSERT(success);
} 

Rect Graphics::DrawRectangle(Rect const &rRect) {
	LogicalXType const left = rRect.LeftX();
	LogicalYType const top = rRect.TopY();
	LogicalXType const right = rRect.RightX();
	LogicalYType const bottom = rRect.BottomY();
	BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	return rRect;
}

Rect Graphics::DrawRectangle(
	LogicalYType top,
    LogicalXType left,
    PCntType width,
    PCntType height)
{
	LogicalXType const right = left + width;
	LogicalYType const bottom = top + height;
	BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
	ASSERT(success != 0);
	
	Rect const result(top, left, width, height);
	
	return result;
}

void Graphics::DrawRoundedSquare(
    Point const &rCenter,
    PCntType edge,
    PCntType circleDiameter)
{
    ASSERT(edge > circleDiameter);
    
    PCntType const ellipse_width = circleDiameter;
    PCntType const ellipse_height = circleDiameter;
    LogicalXType const left_x = rCenter.X() - edge/2;
    LogicalXType const right_x = left_x + edge;
    LogicalYType const top_y = rCenter.Y() - edge/2;
    LogicalYType const bottom_y = top_y + edge;
    BOOL const success = Win::RoundRect(mDraw, left_x, top_y, right_x, bottom_y,
                             ellipse_width, ellipse_height);
    ASSERT(success != 0);
}

void Graphics::DrawText(Rect const &rRect, char const *text, char const *altText) {
	ASSERT(text != NULL);

	char const *str = "";

    if (altText == NULL || TextWidth(text) <= rRect.Width()) {
		str = text;
	} else {
		str = altText;
	}

    int const str_length = ::strlen(str);
	UINT const format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    RECT bounds = RECT(rRect);
    BOOL const success = Win::DrawText(mDraw, str, str_length, &bounds, format);
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

    Point const ulc = rBounds.Interpolate(pair_ulc);
    Point const brc = rBounds.Interpolate(pair_brc);

	Rect const result(ulc, brc);

	return result;
}

Rect Graphics::InteriorHexagon(Rect const &rBounds) {
	double const den = 4 + 4*SQRT_3;
    FractionPair const pair_ulc(2/den, (6 + 2*SQRT_3)/den);
    Point const ulc = rBounds.Interpolate(pair_ulc);

	FractionPair const pair_brc((2 + 4*SQRT_3)/den, (2*SQRT_3 - 2)/den);
    Point const brc = rBounds.Interpolate(pair_brc);

	Rect const result(ulc, brc);

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
	PCntType const radius = circleDiameter/2;
	PCntType pad = radius - PCntType(0.7 * float(radius));
    top_y += pad;
    left_x += pad;
    edge -= 2*pad;
    Rect const result(top_y, left_x, edge, edge);
    
    return result;
}

PCntType Graphics::TextHeight(void) const {
    PCntType const result = 20;
    
    return result;
}

PCntType Graphics::TextWidth(char const *text) const {
    int const length = ::strlen(text);
    SIZE extent;
    BOOL const success = Win::GetTextExtentPoint32(mDraw, text, length, &extent);
	ASSERT(success != 0);
    PCntType const result = extent.cx;
    
    return result;
}

void Graphics::UseColors(ColorType brushBkColor, ColorType penTextColor) {
    if (brushBkColor != mBrushBkColor) {
        HBRUSH const brush = Win::CreateSolidBrush(brushBkColor);
        ASSERT(brush != NULL);
        HGDIOBJ const old = Win::SelectObject(mDraw, brush);
        ASSERT(old != NULL);
        BOOL const success = Win::DeleteObject(old);
        ASSERT(success != 0);

        ColorType const oldColor = ColorType(Win::SetBkColor(mDraw, brushBkColor));
        ASSERT(oldColor != CLR_INVALID);

        mBrushBkColor = brushBkColor;
    }
       
    if (penTextColor != mPenTextColor) {
        ColorType const oldColor = ColorType(Win::SetTextColor(mDraw, penTextColor));
        ASSERT(oldColor != CLR_INVALID);

        int const penStyle = PS_SOLID;
        int const penWidth = 0; // means a pen one pixel wide
        HPEN const pen = Win::CreatePen(penStyle, penWidth, penTextColor);
        ASSERT(pen != NULL);
        HGDIOBJ const old = Win::SelectObject(mDraw, pen);
        ASSERT(old != NULL);
        BOOL const success = Win::DeleteObject(old);
        ASSERT(success != 0);
        
        mPenTextColor = penTextColor;
    }
}

#endif // defined(_WINDOWS)
