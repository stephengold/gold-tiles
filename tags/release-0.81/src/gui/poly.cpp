// File:     poly.cpp
// Location: src/gui
// Purpose:  implement Poly class
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

#ifdef _WINDOWS
#include <vector>
#include "gui/poly.hpp"
#include "gui/rect.hpp"
#include "gui/win_types.hpp"

void Poly::Add(double x, double y) {
    ASSERT(x >= 0.0);
    ASSERT(y >= 0.0);
    ASSERT(x <= 1.0);
    ASSERT(y <= 1.0);

    FractionPair const pair(x, y);
    push_back(pair);
}

unsigned Poly::Count(void) const {
	unsigned const result = size();

	return result;
}

void Poly::GetPoints(
    POINT points[], 
    unsigned numPoints, 
    Rect const& rBounds,
	bool invertFlag) const
{
    ASSERT(numPoints <= Count());
    
    ConstIterator it = begin();
    for (unsigned i = 0; i < numPoints; i++) {
        ASSERT(it < end());
        FractionPair const pair = *it;
        Point const point = rBounds.Interpolate(pair, invertFlag);
        points[i] = POINT(point);
        it++;
    }
}

#endif // defined(_WINDOWS)
