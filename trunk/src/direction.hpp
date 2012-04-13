#ifndef DIRECTION_HPP_INCLUDED
#define DIRECTION_HPP_INCLUDED

// File:     direction.hpp
// Location: src
// Purpose:  declare Direction class
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
A Direction object represents one of the eight cardinal directions on the
game board.

The Direction class encapsulates an enumeration of the eight directions.
*/

class Direction {
public:
	// public lifecycle
	Direction(void);
    // Direction(Direction const&);  compiler-generated copy constructor is OK
    // ~Direction(void);  compiler-generated destructor is OK

	// public operators
    // Direction &operator=(Direction const&);  compiler-generated assignment operator is OK
	operator int(void) const;
	Direction operator++(int);

	// misc public methods
	Direction Axis(void) const;
	Direction Opposite(void) const;
	Direction OrthogonalAxis(void) const;
	void      SetFirst(void);
	Direction TriangleNeighbor(bool odd) const;

	// public inquiry methods
	bool IsAxis(void) const;
	bool IsDiagonal(void) const;
	bool IsEast(void) const;
	bool IsHorizontal(void) const;
	bool IsNorth(void) const;
	bool IsNortheast(void) const;
	bool IsNorthwest(void) const;
	bool IsSouth(void) const;
	bool IsSoutheast(void) const;
	bool IsSouthwest(void) const;
	bool IsValid(void) const;
	bool IsVertical(void) const;
	bool IsWest(void) const;

private:
	// private types
	enum DirectionType {
        // axes (positive directions)
        NORTH = 0, FIRST = 0,
	    NORTHEAST = 1,
        EAST = 2,
	    SOUTHEAST = 3, LAST_POSITIVE = 3,
	    // negative directions
        SOUTH = 4, FIRST_NEGATIVE = 4,
	    SOUTHWEST = 5,
        WEST = 6,
	    NORTHWEST = 7, LAST = 7,
		// invalid directions
	    UNKNOWN = 66
    };

	// private data
	DirectionType mValue;

	// private constructor
	Direction(DirectionType);
};
#endif // !defined(DIRECTION_HPP_INCLUDED)
