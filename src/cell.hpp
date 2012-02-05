#ifndef CELL_HPP_INCLUDED
#define CELL_HPP_INCLUDED

// File:    cell.hpp
// Purpose: Cell class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Cell object identifies a location in a 2-D grid.
 Cells can be accessed by row+column or northing+easting,
 with row number increasing northward and column number
 increasing eastward.  Either coordinate can be negative.
 Cells are used as keys by the BaseBoard and Board classes.
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
