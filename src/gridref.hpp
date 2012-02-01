#ifndef GRIDREF_HPP_INCLUDED
#define GRIDREF_HPP_INCLUDED

// File:    gridref.hpp
// Purpose: GridRef class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A GridRef object identifies a location in a 2-D grid.
 GridRefs can be accessed by row-column or northing-easting,
 with row number increasing northward and column number
 increasing eastward.  Both coordinates can be negative.
 GridRefs are used as keys by the BaseBoard, Board, and
 Locus classes.
*/

#include <string>
#include "project.hpp"

enum Direction {
     North = 0, FirstDir = 0,
     South = 1,
     East = 2,
     West = 3, LastDir = 3,
     NumDir = 4
};

class GridRef {
	int _northing, _easting;

	public:
        GridRef(void);
		GridRef(int n, int e);
		GridRef(GridRef const &, unsigned dir, int count);

        void display(void) const;
		int getN(void) const;
		int getE(void) const;
		int getRow(void) const;
		int getColumn(void) const;
        bool getUserChoice(string const &);
		bool isOrigin(void) const;

		bool operator<(GridRef const &) const;
		bool operator==(GridRef const &) const;

		string toString(void) const;
};
#endif
