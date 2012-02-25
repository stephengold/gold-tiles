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
A Move object represents a group of zero or more tiles and the
location(s) where they will be played.
*/

#include <set>           // ISA std::set
#include "tilecell.hpp"  // HASA TileCell

class Move: public std::set<TileCell> {
public:
    typedef std::set<TileCell>::const_iterator ConstIterator;
 	typedef std::set<TileCell>::iterator       Iterator;

	// public operators
    operator Cells(void) const;
    operator String(void) const;
    operator Tiles(void) const;

	// misc public methods
    void     Add(Tile const &);
    void     Add(Tile const &, Cell const &);
    unsigned Count(void) const;
    void     GetUserChoice(Tiles const &);
	void     MakeEmpty(void);
 
	// public inquiry methods
	bool InvolvesSwap(void) const;
	bool IsPass(void) const;
	bool IsPureSwap(void) const;
    bool RepeatsCell(void) const;
    bool RepeatsTile(void) const;
};

#endif
