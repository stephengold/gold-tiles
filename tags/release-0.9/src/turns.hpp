#ifndef TURNS_HPP_INCLUDED
#define TURNS_HPP_INCLUDED

// File:     turns.hpp
// Location: src
// Purpose:  declare Turns class
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
A Turns object represents a linear sequence of zero or more turns of the game,
used to record game history.
*/

#include <list>      // ISA std::list
#include "turn.hpp"  // HASA Turn


class Turns: public std::list<Turn> {
public:
    // public types
    typedef std::list<Turn>::const_iterator ConstIterator;
    typedef std::list<Turn>::const_reverse_iterator ConstReverseIterator;
    typedef std::list<Turn>::iterator Iterator;

    // public lifecycle
    // Turns(void);  compiler-generated default constructor is OK
    // Turns(Turns const&);  compiler-generated copy constructor is OK
    // ~Turns(void);  compiler-generated destructor is OK

    // misc public methods
    unsigned Count(void) const;
    unsigned Index(Iterator const&) const;
    unsigned IndexLastPlay(void) const;
};
#endif // !defined(TURNS_HPP_INCLUDED)
