#ifndef AREA_HPP_INCLUDED
#define AREA_HPP_INCLUDED

// File:     area.hpp
// Location: src/gui
// Purpose:  declare Area class
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

/*
An Area object represents the dimensions (in pixels) of an on-screen rectangle.

The Area class encapsulates two PixelCntType members. 
*/

#ifdef _WINDOWS
# include "gui/win.hpp"  // Win::SIZE
using Win::SIZE;
#elif defined(_QT)
# include <QSize>
typedef QSize SIZE;
#endif // defined(_QT)

typedef unsigned long PixelCntType;

class Area {
public:
    // public lifecycle
    // no default constructor
    // Area(Area const&);  implicitly defined copy constructor
    Area(SIZE const&);
    Area(PixelCntType width, PixelCntType height);
    // ~Area(void);  implicitly defined destructor

    // public operators
    //Area& operator=(Area const&);  implicitly defined assignment operator

    // misc public methods
    void         Expand(PixelCntType);
    PixelCntType Height(void) const;
    PixelCntType Width(void) const;

    // public inquiry methods
    bool IsSquare(void) const;

private:
    PixelCntType mHeight;
    PixelCntType mWidth;
};
#endif // !defined(AREA_HPP_INCLUDED)
