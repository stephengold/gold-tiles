#ifndef RECT_HPP_INCLUDED
#define RECT_HPP_INCLUDED

// File:    rect.hpp
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

/*
A Rect object represents a rectangular region in the client area of a window.
*/

#include "gui/point.hpp"
#include "project.hpp"    // FractionPair

typedef unsigned long PCntType; // pixel count

class Rect {
public:
	// lifecycle
    Rect(RECT const &);
    Rect(Point const &ulc, Point const &lrc);
    Rect(Point const &ulc, PCntType width, PCntType height);
    Rect(LogicalYType, LogicalXType, PCntType width, PCntType height);
	// no default constructor
    //Rect(Rect const &);  compiler-generated copy constructor is OK
    //~Rect(void);

	// operators
	//Rect &operator=(Rect const &);  compiler-generated assignment method is OK
    operator RECT(void) const;

	// misc public methods
	float        AspectRatio(void) const;
    LogicalYType BottomY(void) const;
    Point        Brc(void) const;  // bottom right corner coordinates
	Point        Center(void) const; // center pixel coordinates
    Rect         CenterRect(float aspectRatio) const;
    Rect         CenterSquare(void) const;
    LogicalXType CenterX(void) const;
    LogicalYType CenterY(void) const;
    PCntType     Height(void) const;
    Point        Interpolate(FractionPair const &, bool invert = false) const;
    LogicalXType LeftX(void) const;
    LogicalXType RightX(void) const;
    LogicalYType TopY(void) const;
    Point        Ulc(void) const;  // upper left corner coordinates
    PCntType     Width(void) const;

	// public inquiry methods
    bool Contains(Point const &) const;
    bool Contains(LogicalXType, LogicalYType) const;
    bool Contains(Rect const &) const;

private:
	LogicalXType mLeft, mRight;
    LogicalYType mBottom, mTop;
};

#endif

