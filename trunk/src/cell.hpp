#ifndef CELL_HPP_INCLUDED
#define CELL_HPP_INCLUDED

// File:     cell.hpp
// Location: src
// Purpose:  declare Cell class
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
    Cell(void);   // the start cell
    Cell(IndexType row, IndexType column);
	Cell(Cell const&, Direction const&, IndexType count = 1);
    // Cell(Cell const&);  compiler-generated copy constructor is OK
    // ~Cell(void);  compiler-generated destructor is OK

	// public operators
	bool operator!=(Cell const&) const;
	bool operator<(Cell const&) const;
	// Cell& operator=(Cell const&);  compiler-generated assignment method is OK
	bool operator==(Cell const&) const;
	operator String(void) const;

	// misc public methods
	IndexType       Column(void) const;
    bool            GetUserChoice(String const&);
    static GridType Grid(void);
	IndexType       Group(Direction const&) const;
	static void     LimitPlay(unsigned&);
	void            Next(Direction const&, IndexType count = 1);
	IndexType       Ortho(Direction const&) const;
	IndexType       Row(void) const;
	static String   ScoringAxes(void);
	static void     SetStatic(GameOpt const&);

	// public inquiry methods
	bool        HasNeighbor(Direction const&) const;
	bool        IsOdd(void) const;
	bool        IsStart(void) const;
	static bool IsScoringAxis(Direction const&);
	bool        IsValid(void) const;

private:
	// private data
	IndexType         mColumn;
	static GridType  msGrid;
	static IndexType msHeight;   // must be even and <= HEIGHT_MAX
	IndexType         mRow;
	static IndexType msWidth;    // must be even and <= WIDTH_MAX
	static bool      msWrapFlag; // coordinates wrap around

    // misc private methods
	static void NextCellOffsets(Direction const&, IndexType& rowOffset, 
		            IndexType& columnOffset);
};
#endif // !defined(CELL_HPP_INCLUDED)
