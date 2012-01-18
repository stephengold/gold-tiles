// File:    gridref.cpp
// Purpose: GridRef class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include <sstream>
#include "gridref.hpp"

// constructors, assignment, and destructor

// default constructor:  refer to the origin
GridRef::GridRef(void) {
	northing = 0;
	easting = 0;
}

GridRef::GridRef(int n, int e) {
	northing = n;
	easting = e;
}

// The compiler-generated copy constructor is fine.

// refer to the square "count" spaces in direction "dir" from "base"
GridRef::GridRef(GridRef const &base, unsigned dir, int count) {
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

// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

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

// public methods

void GridRef::getUserChoice(void) {
    string input;
    while (true) {
        cout << "Enter a row number:" << endl;
        cin >> input;
        northing = atoi(input);
        if (itoa(northing) == input) {
            break;
        }
        cout << "'" << input << "' is invalid." << endl;
    }
    while (true) {
        cout << "Enter a column number:" << endl;
        cin >> input;
        easting = atoi(input);
        if (itoa(easting) == input) {
            break;
        }
        cout << "'" << input << "' is invalid." << endl;
    }
}

bool GridRef::isOrigin(void) const {
    bool result = (northing == 0) && (easting == 0);

    return result;
}

bool GridRef::operator<(GridRef const &other) const {
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
