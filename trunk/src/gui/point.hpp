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
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

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
    operator POINT(void) const;
    operator POINTS(void) const;

	// misc public methods
    LogicalXType X(void) const;
    LogicalYType Y(void) const;
    void         Offset(long dx, long dy);   

private:
	LogicalXType mX;
	LogicalYType mY;
};

#endif
#endif
