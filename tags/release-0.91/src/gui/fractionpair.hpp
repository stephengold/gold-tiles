#ifndef FRACTIONPAIR_HPP_INCLUDED
#define FRACTIONPAIR_HPP_INCLUDED

// File:     fractionpair.hpp
// Location: src/gui
// Purpose:  declare FractionPair class
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
A FractionPair object represents a point in a unit square centered
on (0.5, 0.5).
*/

#include "fraction.hpp"


class FractionPair {
public:
    // lifecycle
    // no default constructor
    FractionPair(double x, double y);
    // FractionPair(FractionPair const&); implicitly defined copy constructor
    // ~FractionPair(void); implicitly defined destructor

    // public operators
    // FractionPair& operator=(FractionPair const&); implicitly defined assignment method

    // misc public methods
    float X(void) const;
    float Y(void) const;

private:
    // private data
    Fraction mX;
    Fraction mY;
};
#endif // !defined(FRACTIONPAIR_HPP_INCLUDED)
