#ifndef CELLS_HPP_INCLUDED
#define CELLS_HPP_INCLUDED

// File:    cells.hpp
// Purpose: Cells class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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

	// misc
	void Add(Cell const &);

	// access
	unsigned Count(void) const;

	// inquiry
	bool AreAllInSameRow(void) const;
    bool AreAllInSameColumn(void) const;
    bool Contains(Cell const &) const;
	bool IsEmpty(void) const;
};

#endif