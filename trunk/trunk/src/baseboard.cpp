// File:    baseboard.cpp
// Purpose: BaseBoard class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include <string>
#include "baseboard.hpp"

// private methods

BaseBoard::const_iterator BaseBoard::find(int n, int e) const {
    GridRef ref(n, e);
    const_iterator result = cells.find(ref);

    return result;
}

// constructors, assignment, and destructor

BaseBoard::BaseBoard(void) {
    maxN = 0;
    maxS = 0;
    maxE = 0;
    maxW = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

void BaseBoard::display(void) const {
     cout << endl << this->toString() << endl;
}

Tile const *BaseBoard::getCell(GridRef const &ref) const {
    const_iterator it = cells.find(ref);

    Tile const *result = NULL;
    if (it != cells.end()) {
        result = &(it->second);
    }

    return result;
}

void BaseBoard::playOnCell(GridRef const &ref, Tile const &tile) {
    D(cout << "Play " << tile.toString() << " at " << ref.toString() << endl);

    ASSERT(getCell(ref) == NULL);

    int n = ref.getN();
    if (n > (int)maxN) {
        maxN = n;
    }
    if (n < -(int)maxS) {
        maxS = -n;
    }

    int e = ref.getE();
    if (e > (int)maxE) {
        maxE = e;
    }
    if (e < -(int)maxW) {
        maxW = -e;
    }
    cells[ref] = tile;
}

string BaseBoard::toString(void) const {
    D(cout << "maxN=" << maxN << " maxS=" << maxS << " maxE=" << maxE << " maxW=" << maxW
        << " in Board::toString()" << endl);

    string result = string(5, ' ');
    for (int column = -maxW; column <= maxE; column++) {
       string cTag = itoa(column);
       result += string(5 - cTag.size(), ' ');
       result += cTag;
    }
    result += "\n";
    for (int row = -maxS; row <= maxN; row++) {
        string rTag = itoa(row);
        result += string(5 - rTag.size(), ' ');
        result += rTag;
	    for (int column = -maxW; column <= maxE; column++) {
            const_iterator it = find(row, column);
			if (it == cells.end()) {
				result += " .";
                result += Tile::toStringEmpty();
                result += ".";
			} else {
                Tile tile = it->second;
				result += " [";
                result += tile.toString();
                result += "]";
			}
		}
		result += "\n";
	}
	return result;
}
