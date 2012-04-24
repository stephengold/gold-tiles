// File:     rect.cpp
// Location: src/gui
// Purpose:  implement Rect class
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

#ifdef _WINDOWS
#include "fractionpair.hpp"
#include "gui/win.hpp"
#include "gui/win_types.hpp"
#include "rect.hpp"

// lifecycle

Rect::Rect(RECT const& rStruct) {
    mTop = rStruct.top;
    mLeft = rStruct.left;
    mRight = rStruct.right;
    mBottom = rStruct.bottom;
}

Rect::Rect(Point const& rUlc, Point const& rBrc) {
    mTop = rUlc.Y();
    mLeft = rUlc.X();
    mRight = rBrc.X();
    mBottom = rBrc.Y();

    ASSERT(Brc() == rBrc);
}

Rect::Rect(Point const& rUlc, PixelCntType width, PixelCntType height) {
    mTop = rUlc.Y();
    mLeft = rUlc.X();
    mRight = mLeft + width;
    mBottom = mTop + height;

    ASSERT(Width() == width);
    ASSERT(Height() == height);
}

Rect::Rect(
    LogicalYType topY,
    LogicalXType leftX,
    PixelCntType width,
    PixelCntType height)
{
    ASSERT(width < 10000);
    ASSERT(height < 10000);

    mTop = topY;
    mLeft = leftX;
    mRight = mLeft + width;
    mBottom = mTop + height;

    ASSERT(Width() == width);
    ASSERT(Height() == height);
}

// operators

Rect::operator RECT(void) const {
    RECT result;
    result.top = mTop;
    result.bottom = mBottom;
    result.left = mLeft;
    result.right = mRight;
    
    return result;
}

// misc methods

float Rect::AspectRatio(void) const {
    float const width = float(Width());
    ASSERT(width > 0.5);

    float const height = float(Height());
    ASSERT(height > 0.5);

    float const result = width/height;
    
    return result;
}

LogicalYType Rect::BottomY(void) const {
    return mBottom;
}

Point Rect::Brc(void) const {
    Point const result(mRight, mBottom);
    
    return result;
}

Point Rect::Center(void) const {
    LogicalXType const x = CenterX();
    LogicalYType const y = CenterY();
    Point const result(x, y);
    
    return result;
}
 
// construct the largest Rect with a given aspect ratio
// that is centered and contained within this Rect

Rect Rect::CenterRect(float aspectRatio) const {
    ASSERT(aspectRatio > 0.01);
    ASSERT(aspectRatio < 100.0);
    
    PixelCntType width = Width();
    PixelCntType height = Height();
    LogicalXType left = LeftX();
    LogicalYType top = TopY();
    
    if (width > height*aspectRatio) {
        left += (unsigned)(width - height*aspectRatio)/2;
        width = height;
    } else if (height > width/aspectRatio) {
        top += (unsigned)(height - width/aspectRatio)/2;
        height = width;
    } 
    Rect const result(top, left, width, height);
    
    ASSERT(Contains(result));
    return result;
}

// construct the largest square that is centered and contained
// within this Rect
Rect Rect::CenterSquare(void) const {
    PixelCntType width = Width();
    PixelCntType height = Height();
    LogicalXType left = LeftX();
    LogicalYType top = TopY();
    
    if (width > height) {
        left += (width - height)/2;
        width = height;
    } else if (height > width) {
        top += (height - width)/2;
        height = width;
    } 
    Rect const result(top, left, width, height);
    
    ASSERT(Contains(result));
    return result;
}

LogicalXType Rect::CenterX(void) const {
	LogicalXType const result = LeftX() + Width()/2;

	return result;
}

LogicalYType Rect::CenterY(void) const {
	LogicalYType const result = TopY() + Height()/2;

	return result;
}

PixelCntType Rect::Height(void) const {
    ASSERT(BottomY() >= TopY());
    PixelCntType const result = BottomY() - TopY();
    
    return result;
}

Point Rect::Interpolate(FractionPair const& rPair, bool invertFlag) const {
    double const height = double(Height() - 1);
    double const width = double(Width() - 1);

    PixelCntType const dx = PixelCntType(0.5 + rPair.X()*width);
    ASSERT(dx < Width());
    LogicalXType const x = LeftX() + dx;
    
	double y_fraction = rPair.Y();
	if (!invertFlag) {
		y_fraction = 1.0 - y_fraction;
	}
    PixelCntType const dy = PixelCntType(0.5 + y_fraction*height);
    ASSERT(dy < Height());
    LogicalYType const y = TopY() + dy;  

    ASSERT(Contains(x, y));
        
    Point const result(x, y);
    
    return result;
}

LogicalXType Rect::LeftX(void) const {
    return mLeft;
}

LogicalXType Rect::RightX(void) const {
    return mRight;
}

LogicalYType Rect::TopY(void) const {
    return mTop;
}

Point Rect::Ulc(void) const {
    Point const result(mLeft, mTop);
    
    return result;
}

PixelCntType Rect::Width(void) const {
    ASSERT(RightX() >= LeftX());
    PixelCntType const result = RightX() - LeftX();
    
    return result;
}


// inquiry methods 

bool Rect::Contains(Point const& rPoint) const {
    LogicalXType const x = rPoint.X();
    LogicalYType const y = rPoint.Y();
    bool const result = Contains(x, y);
    
    return result; 
}

bool Rect::Contains(LogicalXType x, LogicalYType y) const {
    PixelCntType const dx = x - mLeft;
    PixelCntType const dy = y - mTop;
    bool const result = (dx < Width() && dy < Height());
    
    return result; 
}

bool Rect::Contains(Rect const& rOther) const {
    Point const ulc = rOther.Ulc();
    bool result = Contains(ulc);
    
    if (result) {
        LogicalXType const x_max = rOther.RightX() - 1;
        LogicalYType const y_max = rOther.BottomY() - 1;
        if (!Contains(x_max, y_max)) {
            result = false;
        }
    }
    
    return result; 
}
#endif // defined(_WINDOWS)
