// File:    tilesquare.cpp
// Purpose: TileSquare class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "TileSquare.hpp"

// constructors, assignment, and destructor

TileSquare::TileSquare(void) {
	_swapFlag = true;
}

TileSquare::TileSquare(Tile const &t):
    _tile(t)
{
	_swapFlag = true;
}

TileSquare::TileSquare(Tile const &t, GridRef const &s):
    _tile(t), _square(s)
{
	_swapFlag = false;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// accessors

Tile TileSquare::getTile(void) const {
    return _tile;
}

GridRef TileSquare::getSquare(void) const {
	ASSERT(!_swapFlag);
    return _square;
}

bool TileSquare::isSwap(void) const {
	return _swapFlag;
}

// public methods

string TileSquare::getUserChoice(Tiles const &availableTiles, Strings const &alts) {
    string result = _tile.getUserChoice(availableTiles, alts);
	if (result == _tile.toString()) {
        _swapFlag = _square.getUserChoice("swap");
	}
	return result;
}

bool TileSquare::operator<(TileSquare const &other) const {
     bool result;
     
     if (_tile == other._tile) {
		 if (_swapFlag == other._swapFlag) {
			 if (_swapFlag == true) {
				 result = false;
			 } else {
				 result = (_square < other._square);
			 }
		 } else {
             result = (_swapFlag < other._swapFlag);
		 }
     } else {
         result = (_tile < other._tile);
     }
     
     return result;
}

string TileSquare::toString(void) const {
	string result = _tile.toString();
	if (_swapFlag) {
		result += "@bag";
	} else {
		result += "@";
		result += _square.toString();
	}

	return result;
}
