#ifndef POINT_HPP_INCLUDED
#define POINT_HPP_INCLUDED

// File:    point.hpp
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

/*
A Point object represents the location of a pixel in the client area 
of a window.

The Point class is encapsulates a logical X coordinate and a logical Y coordinate. 
*/

#include <Windows.h>  // POINT

typedef long LogicalXType, LogicalYType;

class Point {
public:
	// lifecycle
    Point(LogicalXType, LogicalYType);
    Point(POINT const &);
    Point(POINTS const &);
	// no default constructor
    //Point(Point const &);  compiler-generated copy constructor is OK
    //~Point(void);

	// operators
	//Point &operator=(Point const &);  compiler-generated assignment operator is OK
	bool operator==(Point const &) const;
    operator POINT(void) const;
    operator POINTS(void) const;

	// misc public methods
    void         Offset(long dx, long dy);   // add offsets
    LogicalXType X(void) const;
    LogicalYType Y(void) const;

private:
	LogicalXType mX;
	LogicalYType mY;
};

#endif
