// File:    rect.cpp
// Purpose: Rect class
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
#include <windows.h>
#include "fractionpair.hpp"
#include "rect.hpp"

Rect::Rect(RECT const &rStruct) {
    mTop = rStruct.top;
    mLeft = rStruct.left;
    mRight = rStruct.right;
    mBottom = rStruct.bottom;
}

Rect::Rect(Point const &rUlc, Point const &rBrc) {
    mTop = rUlc.Y();
    mLeft = rUlc.X();
    mRight = rBrc.X();
    mBottom = rBrc.Y();

    ASSERT(Brc() == rBrc);
}

Rect::Rect(Point const &rUlc, PCntType width, PCntType height) {
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
    PCntType width,
    PCntType height)
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
    float width = float(Width());
    ASSERT(width > 0.5);

    float height = float(Height());
    ASSERT(height > 0.5);

    float result = width/height;
    
    return result;
}

LogicalYType Rect::BottomY(void) const {
    LogicalYType result = mBottom;
    
    return result;
}

Point Rect::Brc(void) const {
    Point result(mRight, mBottom);
    
    return result;
}

// construct the largest Rect with a given aspect ratio
// that is centered and contained within this Rect

Rect Rect::CenterRect(float aspectRatio) const {
    ASSERT(aspectRatio > 0.01);
    ASSERT(aspectRatio < 100.0);
    
    PCntType width = Width();
    PCntType height = Height();
    LogicalXType left = LeftX();
    LogicalYType top = TopY();
    
    if (width > height*aspectRatio) {
        left += (unsigned)(width - height*aspectRatio)/2;
        width = height;
    } else if (height > width/aspectRatio) {
        top += (unsigned)(height - width/aspectRatio)/2;
        height = width;
    } 
    Rect result(top, left, width, height);
    
    ASSERT(Contains(result));
    return result;
}

// construct the largest square that is centered and contained
// within this Rect
Rect Rect::CenterSquare(void) const {
    PCntType width = Width();
    PCntType height = Height();
    LogicalXType left = LeftX();
    LogicalYType top = TopY();
    
    if (width > height) {
        left += (width - height)/2;
        width = height;
    } else if (height > width) {
        top += (height - width)/2;
        height = width;
    } 
    Rect result(top, left, width, height);
    
    ASSERT(Contains(result));
    return result;
}

LogicalXType Rect::CenterX(void) const {
	LogicalXType result = LeftX() + Width()/2;

	return result;
}

LogicalYType Rect::CenterY(void) const {
	LogicalYType result = TopY() + Height()/2;

	return result;
}

PCntType Rect::Height(void) const {
    ASSERT(BottomY() >= TopY());
    PCntType result = BottomY() - TopY();
    
    return result;
}

Point Rect::Interpolate(FractionPair const &rPair, bool invertFlag) const {
    double height = double(Height() - 1);
    double width = double(Width() - 1);

    PCntType dx = PCntType(0.5 + rPair.X()*width);
    ASSERT(dx < Width());
    LogicalXType x = LeftX() + dx;
    
	double y_fraction = rPair.Y();
	if (!invertFlag) {
		y_fraction = 1.0 - y_fraction;
	}
    PCntType dy = PCntType(0.5 + y_fraction*height);
    ASSERT(dy < Height());
    LogicalYType y = TopY() + dy;  

    ASSERT(Contains(x, y));
        
    Point result(x, y);
    
    return result;
}

LogicalXType Rect::LeftX(void) const {
    LogicalXType result = mLeft;
    
    return result;
}

LogicalXType Rect::RightX(void) const {
    LogicalXType result = mRight;
    
    return result;
}

LogicalYType Rect::TopY(void) const {
    LogicalYType result = mTop;
    
    return result;
}

Point Rect::Ulc(void) const {
    Point result(mLeft, mTop);
    
    return result;
}

PCntType Rect::Width(void) const {
    ASSERT(RightX() >= LeftX());
    PCntType result = RightX() - LeftX();
    
    return result;
}


// inquiry methods 

bool Rect::Contains(Point const &rPoint) const {
    LogicalXType x = rPoint.X();
    LogicalYType y = rPoint.Y();
    bool result = Contains(x, y);
    
    return result; 
}

bool Rect::Contains(LogicalXType x, LogicalYType y) const {
    PCntType dx = x - mLeft;
    PCntType dy = y - mTop;
    bool result = (dx < Width() && dy < Height());
    
    return result; 
}

bool Rect::Contains(Rect const &rOther) const {
    Point ulc = rOther.Ulc();
    bool result = Contains(ulc);
    
    if (result) {
        LogicalXType x_max = rOther.RightX() - 1;
        LogicalYType y_max = rOther.BottomY() - 1;
        if (!Contains(x_max, y_max)) {
            result = false;
        }
    }
    
    return result; 
}

#endif
