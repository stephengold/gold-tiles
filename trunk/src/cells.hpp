#ifndef CELLS_HPP_INCLUDED
#define CELLS_HPP_INCLUDED

// File:    cells.hpp
// Purpose: Cells class
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
 A Cells object represents zero or more cells in a 2-D grid.
*/

#include <set>
#include "cell.hpp"

class Cells: public std::set<Cell> {
public:
	typedef std::set<Cell>::const_iterator ConstIteratorType;
	typedef std::set<Cell>::iterator IteratorType;

	// operators
    operator String(void) const;

	// misc public methods
	void     Add(Cell const &);
	unsigned Count(void) const;
	void     MakeEmpty(void);

	// public inquiry methods
	bool AreAllInSameRow(void) const;
    bool AreAllInSameColumn(void) const;
    bool Contains(Cell const &) const;
	bool IsEmpty(void) const;
};

#endif
