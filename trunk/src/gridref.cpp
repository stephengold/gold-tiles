// File:    gridref.cpp
// Purpose: GridRef class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include <sstream>
#include "gridref.hpp"

// constructors, assignment, and destructor

// default constructor:  refer to the origin
GridRef::GridRef(void) {
	_northing = 0;
	_easting = 0;
}

GridRef::GridRef(int n, int e) {
	_northing = n;
	_easting = e;
}

// The compiler-generated copy constructor is fine.

// refer to the square "count" spaces in direction "dir" from "base"
GridRef::GridRef(GridRef const &base, unsigned dir, int count) {
	switch (dir) {
		case North:
			_northing = base._northing + count;
            _easting = base._easting;
            break;
        case South:
			_northing = base._northing - count;
            _easting = base._easting;
            break;
		case East:
			_northing = base._northing;
            _easting = base._easting + count;
            break;
        case West:
			_northing = base._northing;
            _easting = base._easting - count;
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
	return _northing;
}

int GridRef::getE(void) const {
	return _easting;
}

int GridRef::getRow(void) const {
	return _northing;
}

int GridRef::getColumn(void) const {
	return _easting;
}

// public methods

bool GridRef::getUserChoice(string const &alt) {
    string input;

    while (true) {
        cout << "Enter a row number";
		if (alt != "") {
			cout << " or '" << alt << "'";
		}
		cout << ": ";
        cin >> input;
		if (input == alt) {
			return true;
		}
        _northing = atoi(input);
        if (itoa(_northing) == input) {
            break;
        }
        cout << "'" << input << "' is invalid." << endl;
    }
    while (true) {
        cout << "Enter a column number: ";
        cin >> input;
        _easting = atoi(input);
        if (itoa(_easting) == input) {
            break;
        }
        cout << "'" << input << "' is invalid." << endl;
    }
	return false;
}

bool GridRef::isOrigin(void) const {
    bool result = (_northing == 0) && (_easting == 0);

    return result;
}

bool GridRef::operator<(GridRef const &other) const {
     bool result;
     
     if (_northing == other._northing) {
         result = (_easting < other._easting);
     } else {
         result = (_northing < other._northing);
     }
     
     return result;
}

bool GridRef::operator==(const GridRef &other) const {
    bool result = (_northing == other._northing && _easting == other._easting);
    
    return result;
}

string GridRef::toString(void) const {
    ostringstream sout;
    sout << "(" << _northing << ", " << _easting << ")";
    string result = sout.str();

    return result;
}
