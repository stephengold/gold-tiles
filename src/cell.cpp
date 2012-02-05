// File:    cell.cpp
// Purpose: Implement Cell class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include "cell.hpp"
#include "string.hpp"

// constructors, assignment, and destructor

// default constructor:  refer to the origin
Cell::Cell(void) {
	mColumn = 0;
	mRow = 0;
}

Cell::Cell(int row, int column) {
	mColumn = column;
	mRow = row;
}

// The compiler-generated copy constructor is fine.

// construct the Cell "count" spaces in direction "dir" from "base"
Cell::Cell(Cell const &rBase, DirectionType direction, int count) {
	switch (direction) {
		case DIRECTION_NORTH:
			mRow = rBase.mRow + count;
            mColumn = rBase.mColumn;
            break;
        case DIRECTION_SOUTH:
			mRow = rBase.mRow - count;
            mColumn = rBase.mColumn;
            break;
		case DIRECTION_EAST:
			mRow = rBase.mRow;
            mColumn = rBase.mColumn + count;
            break;
        case DIRECTION_WEST:
			mRow = rBase.mRow;
            mColumn = rBase.mColumn - count;
            break;
        default:
            ASSERT(false);
            break;
    }
}

// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

Cell::operator String(void) const {
    String result;
    result = "(";
	result += String(mRow);
	result += ", ";
	result += String(mColumn);
	result += ")";

    return result;
}

// methods

int Cell::Column(void) const {
	int result = mColumn;

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

bool Cell::IsOrigin(void) const {
    bool result = (mRow == 0) && (mColumn == 0);

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

bool Cell::operator==(const Cell &rOther) const {
	bool same_row = (mRow == rOther.mRow);
    bool result = (same_row && mColumn == rOther.mColumn);
    
    return result;
}

int Cell::Row(void) const {
	int result = mRow;

	return result;
}
