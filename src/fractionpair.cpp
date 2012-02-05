// File:    fractionpair.cpp
// Purpose: FractionPair class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "fractionpair.hpp"
#include "project.hpp"

FractionPair::FractionPair(float x, float y) {
    ASSERT(x >= 0.0);
    ASSERT(y >= 0.0);
    ASSERT(x <= 1.0);
    ASSERT(y <= 1.0);
    
    mX = x;
    mY = y;
}

float FractionPair::X(void) const {
    float result = mX;
    
    return result;
}

float FractionPair::Y(void) const {
    float result = mY;
    
    return result;
}
