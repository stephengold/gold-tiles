// File:    poly.cpp
// Purpose: Poly and FractionPair classes for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/graphics.hpp"
#include "gui/poly.hpp"

FractionPair::FractionPair(float x, float y) {
    ASSERT(x >= 0.0);
    ASSERT(y >= 0.0);
    ASSERT(x <= 1.0);
    ASSERT(y <= 1.0);
    
    _x = x;
    _y = y;
}

float FractionPair::getX(void) const {
    float result = _x;
    
    return result;
}

float FractionPair::getY(void) const {
    float result = _y;
    
    return result;
}

void Poly::add(float x, float y) {
    ASSERT(x >= 0.0);
    ASSERT(y >= 0.0);
    ASSERT(x <= 1.0);
    ASSERT(y <= 1.0);

    FractionPair pair(x, y);
    push_back(pair);
}

void Poly::getPoints(
    POINT points[], 
    unsigned numPoints, 
    Rect const &bounds) const
{
    ASSERT(numPoints <= size());
    
    unsigned height = bounds.getHeight();
    unsigned width = bounds.getWidth();
    int bottomY = bounds.getBottomY();
    int leftX = bounds.getLeftX();
    vector<FractionPair>::const_iterator it = begin();
    for (unsigned i = 0; i < numPoints; i++) {
        ASSERT(it < end());
        long dx = (long)(0.5 + it->getX()*width);
        points[i].x = leftX + dx;
        long dy = (long)(0.5 + it->getY()*height);
        points[i].y = bottomY - dy;
        it++;
    }
}

#endif

