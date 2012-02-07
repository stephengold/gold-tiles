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

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

class Rect {
public:
	// lifecycle
    Rect(void) { ASSERT(false); };
    //Rect(Rect const &);  compiler-generated copy constructor is OK
    Rect(int topY, int leftX, unsigned width, unsigned height);
    //~Rect(void);

	// operators
	//Rect &operator=(Rect const &);  compiler-generated assignment method is OK

	// misc
    Rect     CenterSquare(void) const;

	// access
    int      BottomY(void) const;
    unsigned Height(void) const;
    int      LeftX(void) const;
    operator RECT(void) const;
    int      RightX(void) const;
    int      TopY(void) const;
    unsigned Width(void) const;

	// inquiry
    bool     Contains(int x, int y) const;

private:
	RECT mBounds;
};

#endif
#endif