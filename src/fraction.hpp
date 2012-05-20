#ifndef FRACTION_HPP_INCLUDED
#define FRACTION_HPP_INCLUDED

// File:     fraction.hpp
// Location: src
// Purpose:  declare Fraction class
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
A Fraction object represents a number between 0 and 1 inclusive,
such as a probability.
*/

#include "project.hpp"   // 

typedef uint32_t SeedType;

class Fraction {
public:
    // lifecycle
    // no default constructor
    Fraction(double);
    // Fraction(Fraction const&); implicitly defined copy constructor
    // ~Fraction(void); implicitly defined destructor

    // public operators
    // Fraction& operator=(Fraction const&); implicitly defined assignment method
    operator float(void) const;

    // misc public methods
    void Invert(void);
    bool RandomBool(void) const;
    static void 
         ReseedGenerator(SeedType);

private:
    // private data
    float mFloat;
};
#endif // !defined(FRACTION_HPP_INCLUDED)
