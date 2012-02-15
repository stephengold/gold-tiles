// File:    cell.cpp
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

#include <iostream>
#include "cell.hpp"
#include "project.hpp"
#include "string.hpp"

// static data

GridType  Cell::msGrid     = GRID_HEX;  // TODO
IndexType Cell::msHeight   = HEIGHT_MAX;
IndexType Cell::msWidth    = WIDTH_MAX;
bool      Cell::msWrapFlag = false;

// lifecycle

// default constructor:  refer to the start cell
Cell::Cell(void) {
	mColumn = 0;
	mRow = 0;
	ASSERT(IsValid());
}

Cell::Cell(IndexType row, IndexType column) {
	mColumn = column;
	mRow = row;
}

// The compiler-generated copy constructor is fine.

// construct the next valid cell (not necessarily a neighbor) in direction "dir" from "base"
Cell::Cell(Cell const &rBase, DirectionType direction, IndexType count) {
	ASSERT(rBase.IsValid());

	IndexType row_offset, column_offset;
	NextCellOffsets(direction, row_offset, column_offset);

	mRow = rBase.mRow + count*row_offset;
	if (msWrapFlag) {
        while (mRow >= msHeight/2) {
		    mRow -= msHeight;
	    }
        while (mRow < -msHeight/2) {
		    mRow += msHeight;
	    }
	}

	mColumn = rBase.mColumn + count*column_offset;
	if (msWrapFlag) {
        while (mColumn >= msWidth/2) {
		    mColumn -= msWidth;
	    }
        while (mRow < -msWidth/2) {
		    mColumn += msWidth;
	    }
	}

	ASSERT(IsValid());
}

// The compiler-generated destructor is fine.


// operators

bool Cell::operator!=(const Cell &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool same_row = (mRow == rOther.mRow);
    bool result = (!same_row || mColumn != rOther.mColumn);
    
    return result;
}

bool Cell::operator<(Cell const &rOther) const {
	bool result;
	bool same_row = (mRow == rOther.mRow);
     
    if (same_row) {
        result = (mColumn < rOther.mColumn);
    } else {
        result = (mRow < rOther.mRow);
    }
     
    return result;
}

// The compiler-generated assignment operator is fine.

bool Cell::operator==(const Cell &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool same_row = (mRow == rOther.mRow);
    bool result = (same_row && mColumn == rOther.mColumn);
    
    return result;
}

Cell::operator String(void) const {
	ASSERT(IsValid());

    String result;
    result = "(";
	result += String(mRow);
	result += ", ";
	result += String(mColumn);
	result += ")";

    return result;
}


// misc methods

IndexType Cell::Column(void) const {
	ASSERT(IsValid());

	return mColumn;
}

IndexType Cell::Distance(Cell const &rCell, DirectionType direction) const {
    ASSERT(rCell != *this);
    
    Cell diff(rCell.mRow - mRow, rCell.mColumn - mColumn);
    IndexType group = diff.Group(direction);
    
    IndexType rowOffset, columnOffset;
    NextCellOffsets(direction, rowOffset, columnOffset);
    Cell step(rowOffset, columnOffset);
    IndexType unit = step.Group(direction);
    ASSERT(unit != 0);
    
    IndexType result = ::abs(group/unit);
        
    ASSERT(result != 0);
    return result;
}
          
bool Cell::GetUserChoice(String const &alt) {
    String input;

    while (true) {
        std::cout << "Enter a row number";
		if (alt != "") {
			std::cout << " or '" << alt << "'";
		}
		std::cout << ": ";
        std::cin >> input;
		if (input == alt) {
			return true;
		}
        mRow = input;
        if (String(mRow) == input) {
            break;
        }
        std::cout << "'" << input << "' is invalid." << std::endl;
    }
    while (true) {
        std::cout << "Enter a column number: ";
        std::cin >> input;
        mColumn = input;
        if (String(mColumn) == input) {
            break;
        }
        std::cout << "'" << input << "' is invalid." << std::endl;
    }

	return false;
}

/* static */ GridType Cell::Grid(void) {
    return msGrid;
}

IndexType Cell::Group(DirectionType direction) const {
    IndexType row_offset, column_offset;
    NextCellOffsets(direction, row_offset, column_offset);
    IndexType result = row_offset*mRow + column_offset*mColumn;

    return result;        
}

void Cell::Next(DirectionType direction, IndexType count) {
    Cell next(*this, direction, count);
    *this = next;
}

/* static */ void Cell::NextCellOffsets(
	DirectionType direction,
	IndexType &rowOffset,
	IndexType &columnOffset)
{
	switch (direction) {
		case DIRECTION_NORTH:
			rowOffset = (msGrid == GRID_HEX) ? +2 : +1;
            columnOffset = 0;
            break;
        case DIRECTION_SOUTH:
			rowOffset = (msGrid == GRID_HEX) ? -2 : -1;
            columnOffset = 0;
            break;
		case DIRECTION_EAST:
			rowOffset = 0;
			columnOffset = (msGrid == GRID_HEX) ? +2 : +1;
            break;
		case DIRECTION_WEST:
			rowOffset = 0;
			columnOffset = (msGrid == GRID_HEX) ? -2 : -1;
            break;
		case DIRECTION_NORTHEAST:
			rowOffset = +1;
            columnOffset = +1;
            break;
		case DIRECTION_NORTHWEST:
			rowOffset = +1;
            columnOffset = -1;
            break;
		case DIRECTION_SOUTHEAST:
			rowOffset = -1;
            columnOffset = +1;
            break;
		case DIRECTION_SOUTHWEST:
			rowOffset = -1;
            columnOffset = -1;
            break;
        default:
            ASSERT(false);
            break;
    }
}

IndexType Cell::Row(void) const {
	ASSERT(IsValid());

	return mRow;
}

/* static */ void Cell::SetGrid(GridType grid) {
	switch (msGrid) {
		case GRID_HEX:
	    case GRID_TRIANGLE:
		case GRID_4WAY:
		case GRID_8WAY:
			break;
		default:
			ASSERT(false);
	}
    msGrid = grid;
}

/* static */ void Cell::SetTopology(
	bool wrapFlag, 
	IndexType width, 
	IndexType height)
{
	ASSERT(msHeight <= HEIGHT_MAX);
	ASSERT(msHeight >= HEIGHT_MIN);
	ASSERT(msHeight % 2 == 0);

	ASSERT(msWidth <= WIDTH_MAX);
	ASSERT(msWidth >= WIDTH_MIN);
	ASSERT(msWidth % 2 == 0);

	msHeight = height;
	msWidth = width;
    msWrapFlag = wrapFlag;
}

// inquiry methods

bool Cell::HasNeighbor(DirectionType direction) const {
	bool result = true;

	switch (msGrid) {
		case GRID_HEX:
			if (direction == DIRECTION_EAST 
			 || direction == DIRECTION_WEST)
			{
				result = false;
			}
			break;

	    case GRID_TRIANGLE:
			if (direction == DIRECTION_NORTH && (mRow % 2) == (mColumn % 2)
			 || direction == DIRECTION_SOUTH && (mRow % 2) == (mColumn % 2))
			{
				result = false;
			}
			// fall through

		case GRID_4WAY:
			if (direction == DIRECTION_NORTHEAST
			 || direction == DIRECTION_NORTHWEST
			 || direction == DIRECTION_SOUTHEAST
			 || direction == DIRECTION_SOUTHWEST)
			{
				result = false;
			}
			break;

		case GRID_8WAY:
			// has neighbors in all directions
			break;

		default:
			ASSERT(false);
	}

	// check for edges
	IndexType row, column;
	NextCellOffsets(direction, row, column);
	row += mRow;
	column += mColumn;
	if (!msWrapFlag) {
        if (row >= msHeight/2 || row < -msHeight/2) {
	        result = false;
		}
        if (column >= msWidth/2 || column < -msWidth/2) {
			result = false;
		}
	}

	return result;
}

bool Cell::IsStart(void) const {
	ASSERT(IsValid());
    bool result = (mRow == 0) && (mColumn == 0);

    return result;
}

bool Cell::IsValid(void) const {
	bool result = IsValid(mRow, mColumn);

	return result;
}

/* static */ bool Cell::IsValid(IndexType row, IndexType column) {
    bool result = true;

	switch (msGrid) {
		case GRID_HEX:
			if ((row & 0x1) != (column & 0x1)) {
				result = false;
			}
			break;
		case GRID_4WAY:
		case GRID_8WAY:
	    case GRID_TRIANGLE:
			// all coordinates are valid cells
			break;
		default:
			ASSERT(false);
	}

	if (row < -msHeight/2 || row >= msHeight/2) {
		result = false;
	}
	if (column < -msWidth/2 || column >= msWidth/2) {
		result = false;
	}

	return result;
}

// utility functions

bool is_scoring_direction(DirectionType direction) {
     bool result = false;
     switch (Cell::Grid()) {
         case GRID_TRIANGLE:
             result = (direction != DIRECTION_NORTH 
                    && direction != DIRECTION_SOUTH);
             break;
         case GRID_4WAY:
             result = (direction == DIRECTION_NORTH 
                    || direction == DIRECTION_EAST
                    || direction == DIRECTION_SOUTH
                    || direction == DIRECTION_WEST);
             break;
         case GRID_HEX:
             result = (direction != DIRECTION_EAST
                    && direction != DIRECTION_WEST);
             break;
         case GRID_8WAY:
             result = true;
             break;
         default:
             ASSERT(false);
     }
     
     return result;
}

DirectionType ortho_direction(DirectionType direction) {
    DirectionType result;
    
    switch(direction) {
        case DIRECTION_NORTH:
        case DIRECTION_SOUTH:
            result = DIRECTION_EAST;
            break;
        case DIRECTION_NORTHEAST:
        case DIRECTION_SOUTHWEST:
            result = DIRECTION_SOUTHEAST;
            break;
        case DIRECTION_EAST:
        case DIRECTION_WEST:
            result = DIRECTION_NORTH;
            break;
        case DIRECTION_NORTHWEST:
        case DIRECTION_SOUTHEAST:
            result = DIRECTION_NORTHEAST;
            break;
    }
    
    // always return a "positive" direction
    return result;
}
