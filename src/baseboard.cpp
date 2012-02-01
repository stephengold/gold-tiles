// File:    baseboard.cpp
// Purpose: BaseBoard class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include <string>
#include "baseboard.hpp"

// constructors, assignment, and destructor

BaseBoard::BaseBoard(void) {
    _maxN = 0;
    _maxS = 0;
    _maxE = 0;
    _maxW = 0;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated assignment method is OK.
// The compiler-generated destructor is OK.

// public methods

// display the entire board to the console
void BaseBoard::display(void) const {
    cout << endl << this->toString() << endl;
}

// make a specific cell empty
void BaseBoard::emptyCell(GridRef const &square) {
    int row = square.getRow();
    int column = square.getColumn();
    iterator it = find(row, column);
    assert(it != _cells.end());
    _cells.erase(it);
}

// get iterators to specific cells
BaseBoard::const_iterator BaseBoard::find(int n, int e) const {
    GridRef ref(n, e);
    const_iterator result = _cells.find(ref);

    return result;
}
BaseBoard::iterator BaseBoard::find(int n, int e) {
    GridRef ref(n, e);
    iterator result = _cells.find(ref);

    return result;
}

// get a GridRef to the cell containing a specific Tile
bool BaseBoard::findTile(Tile const &tile, GridRef &square) const {
    for (int row = _maxN; row >= -(int)_maxS; row--) {
	    for (int column = -_maxW; column <= _maxE; column++) {
            const_iterator it = find(row, column);
			if (it != _cells.end() && it->second == tile) {
                square = it->first;
                return true;
            }
        }
    }
    return false;
}

// get a pointer to the Tile (if any) in a specific cell
Tile const *BaseBoard::getCell(GridRef const &square) const {
    Tile const *result = NULL;

    const_iterator it = _cells.find(square);
    if (it != _cells.end()) {
        result = &(it->second);
    }

    return result;
}

// get the limits of the board in each direction
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

// play a Tile on a specific cell
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

// convert the entire board to a string
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
    for (int row = _maxN; row >= -(int)_maxS; row--) {
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
