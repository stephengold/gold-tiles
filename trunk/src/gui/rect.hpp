#ifndef RECT_HPP_INCLUDED
#define RECT_HPP_INCLUDED

// File:    rect.hpp
// Purpose: Rect class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
    bool     Contains(POINT const &) const;

private:
	RECT mBounds;
};

#endif
#endif