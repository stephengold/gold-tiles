// File:     fraction.cpp
// Location: src
// Purpose:  implement Fraction class
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

#include <cstdlib>      // ::srand()
#include "fraction.hpp"
#include "project.hpp"  // ASSERT


// lifecycle

Fraction::Fraction(double x) {
    ASSERT(x >= 0.0);
    ASSERT(x <= 1.0);

    mFloat = float(x);
}

// The implicitly defined copy constructor is OK.
// The implicitly defined destructor is OK.


// operators

Fraction::operator float(void) const {
    float const result = mFloat;

    return result;
}


// misc methods

void Fraction::Invert(void) {
    mFloat = 1.0f - mFloat;
}

bool Fraction::RandomBool(void) const {
    bool result = false;

    if (mFloat == 1.0) {
        result = true;

    } else if (mFloat > 0.0) {
        double const r = double(::rand())/RAND_MAX;
        ASSERT(r >= 0.0);
        ASSERT(r <= 1.0);
        result = (r < mFloat);
    }

    return result;
}

/* static */ void Fraction::ReseedGenerator(unsigned seed) {
    ::srand(seed);
}
