#ifndef CELL_HPP_INCLUDED
#define CELL_HPP_INCLUDED

// File:    cell.hpp
// Purpose: Cell class
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
A Cell object identifies a location in a 2-D grid.
Cells can be accessed by row+column or northing+easting,
with row number increasing northward and column number
increasing eastward.  Either coordinate can be negative.
Cells are used as keys by the BaseBoard and Board classes.

The Cell class encapsulates a pair of indices:  one index
for the row and another for the column.
*/

#include "project.hpp"

enum DirectionType {
     DIRECTION_NORTH = 0, DIRECTION_FIRST = 0,
     DIRECTION_SOUTH = 1,
     DIRECTION_EAST = 2,
     DIRECTION_WEST = 3, DIRECTION_LAST = 3
};

class Cell {
public:
	// lifecycle
    Cell(void);
    Cell(int row, int column);
	Cell(Cell const &, DirectionType, int count);
    // Cell(Cell const &);  compiler-generated copy constructor is OK
    // ~Cell(void);  compiler-generated destructor is OK

	// operators
	bool operator!=(Cell const &) const;
	bool operator<(Cell const &) const;
	// Cell &operator=(Cell const &);  compiler-generated assignment method is OK
	bool operator==(Cell const &) const;
	operator String(void) const;

	// misc
    bool GetUserChoice(String const &);

	// access
	int  Column(void) const;
	int  Row(void) const;

	// inquiry
	bool IsOrigin(void) const;

private:
	int mColumn, mRow;
};
#endif
