#ifndef HANDS_HPP_INCLUDED
#define HANDS_HPP_INCLUDED

// File:     hands.hpp
// Location: src
// Purpose:  declare Hands class
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
A Hands object represents a circular sequence of zero or more hands of tiles.
*/

#include <vector>    // ISA std::vector
#include "hand.hpp"  // HASA Hand


class Hands: public std::vector<Hand> {
public:
    // public types
    typedef std::vector<Hand>::const_iterator ConstIterator;
    typedef std::vector<Hand>::iterator       Iterator;

    // public lifecycle
    // Hands(void);  implicitly defined default constructor
    // Hands(Hands const&);  implicitly defined copy constructor
    // ~Hands(void);  implicitly defined destructor

    // public operators
    // Hands& operator=(Hands const&);  implicitly defined assignment operator 

    // misc public methods
    void      Append(Hand const&);
    SizeType  Count(void) const;
    ConstIterator 
              Find(String const&) const;
    Iterator  Find(String const&);
    ScoreType MaxScore(void) const;
    void      Next(ConstIterator&) const;
    void      Next(Iterator&);
    void      NextWorking(Iterator&);
    void      Previous(ConstIterator&) const;
    void      Previous(Iterator&);

    // public inquiry method
    bool HasAnyGoneOut(void) const;
    bool HaveAllResigned(void) const;
    bool IsEmpty(void) const;
};
#endif // !defined(HANDS_HPP_INCLUDED)
