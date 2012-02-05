// File:    poly.cpp
// Purpose: Poly and FractionPair classes for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/rect.hpp"
#include "fractionpair.hpp"

void Poly::Add(double x, double y) {
    ASSERT(x >= 0.0);
    ASSERT(y >= 0.0);
    ASSERT(x <= 1.0);
    ASSERT(y <= 1.0);

    FractionPair pair((float)x, (float)y);
    push_back(pair);
}

unsigned Poly::Count(void) const {
	unsigned result = size();

	return result;
}

void Poly::GetPoints(
    POINT points[], 
    unsigned numPoints, 
    Rect const &rBounds) const
{
    ASSERT(numPoints <= Count());
    
    unsigned height = rBounds.Height();
    unsigned width = rBounds.Width();
    int bottomY = rBounds.BottomY();
    int leftX = rBounds.LeftX();
    vector<FractionPair>::const_iterator it = begin();
    for (unsigned i = 0; i < numPoints; i++) {
        ASSERT(it < end());
        long dx = (long)(0.5 + it->X()*width);
        points[i].x = leftX + dx;
        long dy = (long)(0.5 + it->Y()*height);
        points[i].y = bottomY - dy;
        it++;
    }
}

#endif