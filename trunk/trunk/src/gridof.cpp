// File:    gridof.cpp
// Purpose: GridRef class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp"
#include <sstream>

// constructors

// refer to origin
GridRef::GridRef(void) {
	northing = 0;
	easting = 0;
}

GridRef::GridRef(int n, int e) {
	northing = n;
	easting = e;
}

// refers to the square "count" spaces in direction "dir" from "base" 
GridRef::GridRef(const GridRef &base, unsigned dir, int count) {
	switch (dir) {
		case North:
			northing = base.northing + count;
            easting = base.easting;
            break;
        case South:
			northing = base.northing - count;
            easting = base.easting;
            break;
		case East:
			northing = base.northing;
            easting = base.easting + count;
            break;
        case West:
			northing = base.northing;
            easting = base.easting - count;
            break;
        default:
            ASSERT(false);
            break;
    }
}

// accessors
int GridRef::getN(void) const {
	return northing;
}

int GridRef::getE(void) const {
	return easting;
}

int GridRef::getRow(void) const {
	return northing;
}

int GridRef::getColumn(void) const {
	return easting;
}

// compare and assign operators

bool GridRef::operator<(const GridRef &other) const {
     bool result;
     
     if (northing == other.northing) {
         result = (easting < other.easting);
     } else {
         result = (northing < other.northing);
     }
     
     return result;
}

bool GridRef::operator==(const GridRef &other) const {
    bool result = (northing == other.northing && easting == other.easting);
    
    return result;
}

string GridRef::toString(void) const {
    ostringstream sout;

    sout << "(" << northing << ", " << easting << ")";
    string result = sout.str();

    return result;
}

// set<GridRef> utilities

bool areAllInSameRow(const set<GridRef> &squares) {
    bool result = true;
    
    if (squares.size() > 1) {
        set<GridRef>::const_iterator s = squares.begin();
        int row = s->getRow();
        for ( ; s != squares.end(); s++) {
            if (s->getRow() != row) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

bool areAllInSameColumn(const set<GridRef> &squares) {
    bool result = true;
    
    if (squares.size() > 1) {
        set<GridRef>::const_iterator s = squares.begin();
        int column = s->getColumn();
        for ( ; s != squares.end(); s++) {
            if (s->getColumn() != column) {
                result = false;
                break;
            }     
        }
    }

    return result;
}
