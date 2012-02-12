#ifndef PLAYERS_HPP_INCLUDED
#define PLAYERS_HPP_INCLUDED

// File:    hands.hpp
// Purpose: Hands class
// Author:  Stephen Gold sgold@sonic.net
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
A Hands object represents a group of zero or more hands.
*/

#include <vector>
#include "hand.hpp"

class Hands: public std::vector<Hand> {
public:
    typedef std::vector<Hand>::const_iterator ConstIteratorType;
    typedef std::vector<Hand>::iterator IteratorType;

	// misc methods
    unsigned Count(void) const;
    void     Next(ConstIteratorType &) const;
    void     Next(IteratorType &);
};

#endif
