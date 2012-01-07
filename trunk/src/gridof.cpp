// File:    gridof.cpp
// Purpose: GridRef class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp"

GridRef::GridRef(int n, int e) {
	this->easting = e;
	this->northing = n;
}

int GridRef::getE(void) const {
	return this->easting;
}

int GridRef::getN(void) const {
	return this->northing;
}

GridRef GridRef::toDirection(unsigned dir) const {
	int e = this->easting;
	int n = this->northing;

	switch (dir) {
		case 0: // north
			return GridRef(n + 1, e);
		case 1: // east
			return GridRef(n, e + 1);
        case 2: // south
            return GridRef(n - 1, e);
        case 3: // west
            return GridRef(n, e - 1);
    }
}
