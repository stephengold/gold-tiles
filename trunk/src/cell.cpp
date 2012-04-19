// File:     cell.cpp
// Location: src
// Purpose:  implement Cell class
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

#include <iostream>    // cin
#include "direction.hpp"
#include "gameopt.hpp"
#include "project.hpp" // ASSERT

// static data

GridType  Cell::msGrid     = GRID_4WAY;
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
Cell::Cell(Cell const& rBase, Direction const& rDirection, IndexType count) {
	ASSERT(rBase.IsValid());
	ASSERT(rDirection.IsValid());
	ASSERT(count == 1 || count == -1);

	Direction direction = rDirection;
	if (count < 0) {
		direction = direction.Opposite();
		count = -count;
	}

	if (Grid() == GRID_TRIANGLE) {
		bool const odd = rBase.IsOdd();
		direction = direction.TriangleNeighbor(odd);
	}
    IndexType row_offset;
	IndexType column_offset;
    NextCellOffsets(direction, row_offset, column_offset);
    mRow = rBase.mRow + count*row_offset;
    mColumn = rBase.mColumn + count*column_offset;

	if (msWrapFlag) {
        Wrap();
	}
}

// The compiler-generated destructor is fine.


// operators

bool Cell::operator!=(Cell const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool const same_row = (mRow == rOther.mRow);
    bool const result = (!same_row || mColumn != rOther.mColumn);
    
    return result;
}

bool Cell::operator<(Cell const& rOther) const {
	bool result = false;
	bool const same_row = (mRow == rOther.mRow);
     
    if (same_row) {
        result = (mColumn < rOther.mColumn);
    } else {
        result = (mRow < rOther.mRow);
    }
     
    return result;
}

// The compiler-generated assignment operator is fine.

bool Cell::operator==(Cell const& rOther) const {
	bool const same_row = (mRow == rOther.mRow);
    bool const result = (same_row && mColumn == rOther.mColumn);
    
    return result;
}

Cell::operator String(void) const {
	ASSERT(IsValid());

    String result("(");
	result += String(mRow) + "," + String(mColumn);
	result += ")";

    return result;
}


// misc methods

IndexType Cell::Column(void) const {
	return mColumn;
}

bool Cell::GetUserChoice(String const& rAlternate) {
    String input;

    for (;;) {
        std::cout << "Enter a row number";
		if (rAlternate != "") {
			std::cout << " or '" << rAlternate << "'";
		}
		std::cout << ": ";
        std::cin >> input;
		if (input == rAlternate) {
			return true;
		}
        mRow = input;
        if (String(mRow) == input) {
            break;
        }
        std::cout << "'" << input << "' is invalid." << std::endl;
    }
    for (;;) {
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

/* static */ void Cell::LimitPlay(unsigned& rCellCnt) {
	ASSERT(rCellCnt > 0);

	IndexType const cells_needed = rCellCnt;
	IndexType most_found = 1;

	Direction axis;
    for (axis.SetFirst(); axis.IsAxis(); axis++) {
	    if (Cell::IsScoringAxis(axis)) {
	        IndexType cells_found = 1;

			Cell current; // start cell
			ASSERT(current.IsValid());

			// look in the negative direction; stop at first invalid cell
            while (cells_found < cells_needed) {
                Cell const previous(current, axis, -1);
                if (!previous.IsValid()) {
			        break;
		        }
		        ++cells_found;
				current = previous;
	        }
    
			current = Cell(); // start cell
			ASSERT(current.IsValid());

			// look in the positive direction; stop at first invalid cell
			while (cells_found < cells_needed) {
                Cell const next(current, axis);
                if (!next.IsValid()) {
			        break;
		        }
				++cells_found;
				current = next;
			}

			if (cells_found >= cells_needed) {
				return;
			}
			if (cells_found > most_found) {
				most_found = cells_found;
			}
		}
	}

	ASSERT(most_found < cells_needed);
	rCellCnt = most_found;
}

void Cell::Next(Direction const& rDirection, IndexType count) {
	ASSERT(count == 1 || count == -1);

    Cell const next(*this, rDirection, count);
    *this = next;
}

/* static */ void Cell::NextCellOffsets(
	Direction const& rDirection,
	IndexType& rRowOffset,
	IndexType& rColumnOffset)
{
	if (rDirection.IsNorth()) {
		rRowOffset = (msGrid == GRID_HEX) ? +2 : +1;
        rColumnOffset = 0;
	} else if (rDirection.IsSouth()) {
		rRowOffset = (msGrid == GRID_HEX) ? -2 : -1;
        rColumnOffset = 0;
	} else if (rDirection.IsEast()) {
		rRowOffset = 0;
		rColumnOffset = (msGrid == GRID_HEX) ? +2 : +1;
	} else if (rDirection.IsWest()) {
		rRowOffset = 0;
		rColumnOffset = (msGrid == GRID_HEX) ? -2 : -1;
	} else if (rDirection.IsNortheast()) {
		rRowOffset = +1;
        rColumnOffset = +1;
	} else if (rDirection.IsNorthwest()) {
		rRowOffset = +1;
        rColumnOffset = -1;
	} else if (rDirection.IsSoutheast()) {
		rRowOffset = -1;
        rColumnOffset = +1;
	} else if (rDirection.IsSouthwest()) {
		rRowOffset = -1;
        rColumnOffset = -1;
	} else {
        FAIL();
    }
}

IndexType Cell::Row(void) const {
	return mRow;
}

// number of rows above (or below) used cells which might be usable
/* static */ int Cell::RowFringe(void) {
	int const result = (msGrid == GRID_HEX) ? 2 : 1;

	return result;
}

/* static */ String Cell::ScoringAxes(void) {
	String result;

	switch (msGrid) {
		case GRID_TRIANGLE:
			result = "row or diagonal";
			break;
		case GRID_4WAY:
			result = "row or column";
			break;
		case GRID_HEX:
			result = "column or diagonal";
			break;
		case GRID_8WAY:
		    result = "row, column, or diagonal";
			break;
		default:
			FAIL();
	}

	return result;
}

/* static */ void Cell::SetStatic(GameOpt const& rGameOpt) {
    GridType const grid = rGameOpt;
	switch (grid) {
		case GRID_HEX:
	    case GRID_TRIANGLE:
		case GRID_4WAY:
		case GRID_8WAY:
			break;
		default:
			FAIL();
	}
    msGrid = grid;

	IndexType const height = rGameOpt.BoardHeight();
	ASSERT(height <= HEIGHT_MAX);
	ASSERT(height >= HEIGHT_MIN);
	ASSERT(!::is_odd(height));
	msHeight = height;

	IndexType const width = rGameOpt.BoardWidth(); 
	ASSERT(width <= WIDTH_MAX);
	ASSERT(width >= WIDTH_MIN);
	ASSERT(!::is_odd(width));
	msWidth = width;

    msWrapFlag = rGameOpt.DoesBoardWrap();
}

void Cell::Wrap(void) {
	ASSERT(msWrapFlag);

    if (mColumn >= 0) {
		IndexType const num_wraps = (mColumn + msWidth/2) / msWidth;
		ASSERT(num_wraps >= 0);
		mColumn -= num_wraps * msWidth;
    } else {
		IndexType const num_wraps = (msWidth/2 - mColumn - 1) / msWidth;
		ASSERT(num_wraps >= 0);
		mColumn += num_wraps * msWidth;
	}

	if (mRow >= 0) {
		IndexType const num_wraps = (mRow + msHeight/2) / msHeight;
		ASSERT(num_wraps >= 0);
		mRow -= num_wraps * msHeight;
    } else {
		IndexType const num_wraps = (msHeight/2 - mRow - 1) / msHeight;
		ASSERT(num_wraps >= 0);
		mRow += num_wraps * msHeight;
	}

	ASSERT(mColumn >= -msWidth/2);
	ASSERT(mColumn < msWidth/2);
	ASSERT(mRow >= -msHeight/2);
	ASSERT(mRow < msHeight/2);
}


// inquiry methods

/* static */ bool Cell::DoesBoardWrap(void) {
	return msWrapFlag;
}

bool Cell::HasNeighbor(Direction const& rDirection) const {
	ASSERT(rDirection.IsValid());

	bool result = true;
	switch (msGrid) {
		case GRID_HEX:
			result = !(rDirection.IsHorizontal());
			break;

	    case GRID_TRIANGLE:
			if (rDirection.IsNorth() && !IsOdd()
			 || rDirection.IsSouth() && IsOdd()
			 || rDirection.IsDiagonal())
			{
				result = false;
			} else {
				result = true;
			}
			break;

		case GRID_4WAY:
			result = !(rDirection.IsDiagonal());
			break;

		case GRID_8WAY:
			result = true; // has neighbors in all eight directions
			break;

		default:
			FAIL();
	}

	// check for edges
	IndexType column;
	IndexType row;
	NextCellOffsets(rDirection, row, column);
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

bool Cell::IsOdd(void) const {
    bool const result = ::is_odd(mRow + mColumn);

	return result;
}

/* static */ bool Cell::IsScoringAxis(Direction const& rAxis) {
	ASSERT(rAxis.IsAxis());

    bool result = false;
    switch (Cell::Grid()) {
        case GRID_TRIANGLE:
            result = !(rAxis.IsVertical()); 
            break;
        case GRID_4WAY:
            result = !(rAxis.IsDiagonal());
            break;
        case GRID_HEX:
            result = !(rAxis.IsHorizontal());
            break;
        case GRID_8WAY:
            result = true; // all four axes are for scoring
            break;
        default:
            FAIL();
    }
     
    return result;
}

bool Cell::IsStart(void) const {
	ASSERT(IsValid());
    bool const result = (mRow == 0) && (mColumn == 0);

    return result;
}

bool Cell::IsValid(void) const {
    bool result = true;

	if (msGrid == GRID_HEX && IsOdd()) {
		result = false;
	}

	if (mRow < -msHeight/2 || mRow >= msHeight/2) {
		result = false;
	}
	if (mColumn < -msWidth/2 || mColumn >= msWidth/2) {
		result = false;
	}

	return result;
}
