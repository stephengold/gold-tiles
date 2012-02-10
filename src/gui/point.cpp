// File:    point.cpp
// Purpose: Point class
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
#include "point.hpp"

Point::Point(LogicalXType x, LogicalYType y) {
    mX = x;
    mY = y;
}

Point::Point(POINT const &pt) {
    mX = pt.x;
    mY = pt.y;
}

Point::Point(POINTS const &pt) {
    mX = pt.x;
    mY = pt.y;
}

// operators

Point::operator POINT(void) const {
    POINT result;
    result.x = mX;
    result.y = mY;
    
    return result;
}

Point::operator POINTS(void) const {
    POINTS result;

	ASSERT(mX <= SHRT_MAX);
	ASSERT(mX >= SHRT_MIN);
    result.x = (SHORT)mX;

	ASSERT(mY <= SHRT_MAX);
	ASSERT(mY >= SHRT_MIN);
    result.y = (SHORT)mY;
    
    return result;
}

// misc methods

void Point::Offset(long dx, long dy) {
    mX += dx;
    mY += dy;     
}

LogicalXType Point::X(void) const {
    LogicalYType result = mX;
    
    return result;
}

LogicalYType Point::Y(void) const {
    LogicalYType result = mY;
    
    return result;
}

#endif
