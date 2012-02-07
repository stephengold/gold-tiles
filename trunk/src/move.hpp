#ifndef MOVE_HPP_INCLUDED
#define MOVE_HPP_INCLUDED

// File:    move.hpp
// Purpose: Move class
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
A Move object represents zero or more tiles and the
locations where they will be played.
*/

#include <set>
#include "tilecell.hpp"

class Move: public std::set<TileCell> {
public:
    typedef std::set<TileCell>::const_iterator ConstIteratorType;
 	typedef std::set<TileCell>::iterator       IteratorType;

	// operators
    operator Cells(void) const;
    operator String(void) const;
    operator Tiles(void) const;

	// misc
    void     Add(Tile const &);
    void     Add(Tile const &, Cell const &);
    unsigned Count(void) const;
    void     GetUserChoice(Tiles const &);
	void     MakeEmpty(void);
 
	// inquiry
	bool     InvolvesSwap(void) const;
	bool     IsPass(void) const;
	bool     IsPureSwap(void) const;
    bool     RepeatsCell(void) const;
    bool     RepeatsTile(void) const;
};

#endif
