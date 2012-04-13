// File:     direction.cpp
// Location: src
// Purpose:  implement Direction class
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

#include "direction.hpp"
#include "project.hpp"   // ASSERT


// lifecycle

Direction::Direction(void) {
	mValue = UNKNOWN;
}

Direction::Direction(DirectionType value) {
	mValue = value;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.
Direction Direction::operator++(int) {
	ASSERT(IsValid());

	if (mValue < LAST) {
		int const value = mValue;
	    mValue = DirectionType(value + 1);
	} else {
		mValue = UNKNOWN;
	}

	return mValue;
}


// misc methods

Direction Direction::Axis(void) const {
	ASSERT(IsValid());

    DirectionType value = UNKNOWN;
	switch (mValue) {
		case NORTH:
		case SOUTH:
			value = NORTH;
			break;
		case NORTHEAST:
	    case SOUTHWEST:
			value = NORTHEAST;
			break;
		case EAST:
	    case WEST:
			value = EAST;
			break;
		case NORTHWEST:
		case SOUTHEAST:
			value = SOUTHEAST;
			break;
		default:
			FAIL();
	}
	Direction const result(value);

	ASSERT(result.IsAxis());
	return result;
}

Direction Direction::OrthogonalAxis() const {
	ASSERT(IsValid());

    DirectionType value(UNKNOWN);
    switch (mValue) {
        case NORTH:
        case SOUTH:
            value = EAST;
            break;
        case NORTHEAST:
        case SOUTHWEST:
            value = SOUTHEAST;
            break;
        case EAST:
        case WEST:
            value = NORTH;
            break;
        case NORTHWEST:
        case SOUTHEAST:
            value = NORTHEAST;
            break;
		default:
			FAIL();
    }
	Direction const result(value);
    
	ASSERT(result.IsAxis());
	return result;
}

Direction Direction::Opposite() const {
	ASSERT(IsValid());

    DirectionType value(UNKNOWN);
    switch (mValue) {
        case NORTH:
			value = SOUTH;
			break;
        case NORTHEAST:
			value = SOUTHWEST;
			break;
        case EAST:
			value = WEST;
			break;
        case SOUTHEAST:
            value = NORTHWEST;
            break;
        case SOUTH:
            value = NORTH;
            break;
        case SOUTHWEST:
            value = NORTHEAST;
            break;
        case WEST:
            value = EAST;
            break;
        case NORTHWEST:
			value = SOUTHEAST;
			break;
		default:
			FAIL();
    }
	Direction const result(value);
    
	ASSERT(result.IsValid());
	return result;
}

void Direction::SetFirst(void) {
	mValue = FIRST;
}

/*
On a triangular grid, a cell's neighbor along a diagonal
direction will actually lie in a different direction, depending
on whether the cell is an even one or an odd one.
*/   
Direction Direction::TriangleNeighbor(bool odd) const {
	ASSERT(IsValid());

    DirectionType value = UNKNOWN;
	switch (mValue) {
	    case NORTH:
	    case SOUTH:
	    case EAST:
	    case WEST:
			value = mValue;
			break;
		case NORTHEAST:
			value = odd ? NORTH : EAST;
			break;
	    case SOUTHEAST:
			value = odd ? EAST : SOUTH;
			break;
	    case NORTHWEST:
			value = odd ? NORTH : WEST;
			break;
		case SOUTHWEST:
			value = odd ? WEST : SOUTH;
			break;
		default:
			FAIL();
	}
	Direction const result(value);

	ASSERT(result.IsValid());
    return result;
}


// inquiry methods

bool Direction::IsAxis(void) const {
	bool const result = (mValue >= FIRST && mValue <= LAST_POSITIVE); 

	return result;
}

bool Direction::IsDiagonal(void) const {
	bool const result = (mValue == NORTHEAST 
		              || mValue == NORTHWEST 
					  || mValue == SOUTHEAST 
					  || mValue == SOUTHWEST);

	return result;
}

bool Direction::IsEast(void) const {
	bool const result = (mValue == EAST);

	return result;
}

bool Direction::IsHorizontal(void) const {
	bool const result = (mValue == EAST || mValue == WEST);

	return result;
}

bool Direction::IsNorth(void) const {
	bool const result = (mValue == NORTH);

	return result;
}

bool Direction::IsNortheast(void) const {
	bool const result = (mValue == NORTHEAST);

	return result;
}

bool Direction::IsNorthwest(void) const {
	bool const result = (mValue == NORTHWEST);

	return result;
}

bool Direction::IsSouth(void) const {
	bool const result = (mValue == SOUTH);

	return result;
}

bool Direction::IsSoutheast(void) const {
	bool const result = (mValue == SOUTHEAST);

	return result;
}

bool Direction::IsSouthwest(void) const {
	bool const result = (mValue == SOUTHWEST);

	return result;
}

bool Direction::IsValid(void) const {
	bool const result = (mValue >= FIRST && mValue <= LAST);

	return result;
}

bool Direction::IsVertical(void) const {
	bool const result = (mValue == NORTH || mValue == SOUTH);

	return result;
}

bool Direction::IsWest(void) const {
	bool const result = (mValue == WEST);

	return result;
}
