#ifndef POLY_HPP_INCLUDED
#define POLY_HPP_INCLUDED

// File:     poly.hpp
// Location: src/gui
// Purpose:  declare Poly class
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
A Poly object represents a closed polygon which can be scaled to
fit inside any rectangle.

The Poly class is implemented as a list of FractionPair objects.
*/

#include <vector>               // ISA std::vector
#include "gui/fractionpair.hpp" // HASA FractionPair
#include "gui/point.hpp"        // USES POINT
#include "project.hpp"          // USES Rect


class Poly: public std::vector<FractionPair> {
public:
    // public types
    typedef std::vector<FractionPair>::const_iterator ConstIterator;
    typedef std::vector<FractionPair>::iterator       Iterator;

    // public lifecycle
    // Poly(void);  implicitly defined default constructor
    // Poly(Poly const&);  implicitly defined copy constructor
    // ~Poly(void);  implicitly defined destructor

    // public operators
    // Poly& operator=(Poly const&);  implicitly defined assignment operator 

    // misc public methods
    void     Add(double x, double y);
    SizeType Count(void) const;
    void     GetPoints(POINT points[], SizeType numPoints,
                 Rect const& bounds, bool invert) const;
};
#endif // !defined(POLY_HPP_INCLUDED)
