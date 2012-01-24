// File:    baseboard.cpp
// Purpose: BaseBoard class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include <string>
#include "baseboard.hpp"

// private methods

BaseBoard::const_iterator BaseBoard::find(int n, int e) const {
    GridRef ref(n, e);
    const_iterator result = _cells.find(ref);

    return result;
}

// constructors, assignment, and destructor

BaseBoard::BaseBoard(void) {
    _maxN = 0;
    _maxS = 0;
    _maxE = 0;
    _maxW = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

void BaseBoard::display(void) const {
     cout << endl << this->toString() << endl;
}

Tile const *BaseBoard::getCell(GridRef const &ref) const {
    const_iterator it = _cells.find(ref);

    Tile const *result = NULL;
    if (it != _cells.end()) {
        result = &(it->second);
    }

    return result;
}

int BaseBoard::getMaxN(void) const {
    return _maxN;
}
int BaseBoard::getMaxS(void) const {
    return _maxS;
}
int BaseBoard::getMaxE(void) const {
    return _maxE;
}
int BaseBoard::getMaxW(void) const {
    return _maxW;
}

void BaseBoard::playOnCell(GridRef const &ref, Tile const &tile) {
    D(cout << "Play " << tile.toString() << " at " << ref.toString() << endl);

    ASSERT(getCell(ref) == NULL);

    int n = ref.getN();
    if (n > (int)_maxN) {
        _maxN = n;
    }
    if (n < -(int)_maxS) {
        _maxS = -n;
    }

    int e = ref.getE();
    if (e > (int)_maxE) {
        _maxE = e;
    }
    if (e < -(int)_maxW) {
        _maxW = -e;
    }
    _cells[ref] = tile;
}

string BaseBoard::toString(void) const {
    D(cout << "maxN=" << _maxN << " maxS=" << _maxS << " maxE=" << _maxE << " maxW=" << _maxW
        << " in Board::toString()" << endl);

    string result = string(5, ' ');
    for (int column = -_maxW; column <= _maxE; column++) {
       string cTag = itoa(column);
       result += string(5 - cTag.size(), ' ');
       result += cTag;
    }
    result += "\n";
    for (int row = _maxN; row >= -(int)_maxS; row++) {
        string rTag = itoa(row);
        result += string(5 - rTag.size(), ' ');
        result += rTag;
	    for (int column = -_maxW; column <= _maxE; column++) {
            const_iterator it = find(row, column);
			if (it == _cells.end()) {
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
