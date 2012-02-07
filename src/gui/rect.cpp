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
#include "rect.hpp"

Rect::Rect(int topY, int leftX, unsigned width, unsigned height) {
    mBounds.top = topY;
    mBounds.left = leftX;
    mBounds.right = leftX + width;
    mBounds.bottom = topY + height;
}

Rect::operator RECT(void) const {
    RECT result = mBounds;
    
    return result;
}

// methods

int Rect::BottomY(void) const {
    int result = mBounds.bottom;
    
    return result;
}

Rect Rect::CenterSquare(void) const {
    unsigned width = Width();
    unsigned height = Height();
    int left = LeftX();
    int top = TopY();
    
    if (width > height) {
        left += (width - height)/2;
        width = height;
    } else if (height > width) {
        top += (height - width)/2;
        height = width;
    } 
    Rect result(top, left, width, height);
    
    return result;
}

bool Rect::Contains(int x, int y) const {
    unsigned dx = x - mBounds.left;
    unsigned dy = y - mBounds.top;
    bool result = (dx < Width() && dy < Height());
    
    return result; 
}

unsigned Rect::Height(void) const {
    ASSERT(BottomY() >= TopY());
    unsigned result = BottomY() - TopY();
    
    return result;
}

int Rect::LeftX(void) const {
    int result = mBounds.left;
    
    return result;
}

int Rect::RightX(void) const {
    int result = mBounds.right;
    
    return result;
}

int Rect::TopY(void) const {
    int result = mBounds.top;
    
    return result;
}

unsigned Rect::Width(void) const {
    ASSERT(RightX() >= LeftX());
    unsigned result = RightX() - LeftX();
    
    return result;
}


#endif