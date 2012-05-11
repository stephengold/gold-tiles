// File:     area.cpp
// Location: src/gui
// Purpose:  implement Area class
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

#include "area.hpp"


// lifecycle

Area::Area(PixelCntType width, PixelCntType height) {
    mWidth = width;
    mHeight = height;
}


// misc methods

void Area::Expand(PixelCntType pixel_cnt) {
    mHeight += pixel_cnt;
    mWidth += pixel_cnt;
}

PixelCntType Area::Height(void) const {
    return mHeight;
}

PixelCntType Area::Width(void) const {
    return mWidth;
}


// inquiry methods

bool Area::IsSquare(void) const {
    bool const result = (mHeight == mWidth);

    return result;
}
