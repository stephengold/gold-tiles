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
for the row (or northing) and another for the column (or easting).
*/

#include "indices.hpp"  // HASA IndexType
#include "string.hpp"   // USES String

enum DirectionType {
    // positive directions
    DIRECTION_NORTH = 0, DIRECTION_FIRST = 0,
	DIRECTION_NORTHEAST = 1,
    DIRECTION_EAST = 2,
	DIRECTION_SOUTHEAST = 3, DIRECTION_LAST_POSITIVE = 3,
	// negative directions
    DIRECTION_SOUTH = 4, DIRECTION_FIRST_NEGATIVE = 4,
	DIRECTION_SOUTHWEST = 5,
    DIRECTION_WEST = 6,
	DIRECTION_NORTHWEST = 7, DIRECTION_LAST = 7,
	DIRECTION_UNKNOWN = 66
};

enum GridType {
	GRID_TRIANGLE = 3,
	GRID_4WAY = 4,
	GRID_HEX = 6,
	GRID_8WAY = 8,
	GRID_DEFAULT = GRID_4WAY
};

class Cell {
public:
	// public constants
	static const IndexType HEIGHT_MAX = 0x40000000L;
	static const IndexType HEIGHT_MIN = 4;
	static const IndexType WIDTH_MAX  = 0x40000000L;
	static const IndexType WIDTH_MIN = 4;

	// public lifecycle
    Cell(void);
    Cell(IndexType row, IndexType column);
	Cell(Cell const &, DirectionType, IndexType count = 1);
    // Cell(Cell const &);  compiler-generated copy constructor is OK
    // ~Cell(void);  compiler-generated destructor is OK

	// public operators
	bool operator!=(Cell const &) const;
	bool operator<(Cell const &) const;
	// Cell &operator=(Cell const &);  compiler-generated assignment method is OK
	bool operator==(Cell const &) const;
	operator String(void) const;

	// misc public methods
	IndexType       Column(void) const;
	static void     GetTopology(bool &wrapFlag, IndexType &height, IndexType &width);
    bool            GetUserChoice(String const &);
    static GridType Grid(void);
	IndexType       Group(DirectionType direction) const;
	void            Next(DirectionType, IndexType count = 1);
	IndexType       Ortho(DirectionType direction) const;
	IndexType       Row(void) const;
	static void     SetGrid(GridType);
	static void     SetTopology(bool wrapFlag, IndexType height, IndexType width);

	// public inquiry methods
	bool        HasNeighbor(DirectionType) const;
	bool        IsOdd(void) const;
	bool        IsStart(void) const;
	bool        IsValid(void) const;
	static bool IsValid(IndexType row, IndexType column);

private:
	// private data
	IndexType         mColumn;
	static GridType  msGrid;
	static IndexType msHeight;   // must be even and <= HEIGHT_MAX
	IndexType         mRow;
	static IndexType msWidth;    // must be even and <= WIDTH_MAX
	static bool      msWrapFlag; // coordinates wrap around

    // misc private methods
	static void          NextCellOffsets(DirectionType, IndexType &rowOffset, 
		                        IndexType &columnOffset);
    static DirectionType OppositeDirection(DirectionType);
	static DirectionType OrthoAxis(DirectionType);
};
#endif // !defined(CELL_HPP_INCLUDED)
