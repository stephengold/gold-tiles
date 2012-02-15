// File:    hand.cpp
// Purpose: Hand class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of theGNU General Public License

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

#include <iostream>
#include "move.hpp"
#include "hand.hpp"

// lifecycle

Hand::Hand(void) {
	mScore = 0;
}

Hand::Hand(const String &name):
    mName(name)
{
	mScore = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

Hand::operator Tiles(void) const {
    return mTiles;
}
// The compiler-generated assignment method is fine.


// misc methods

void Hand::AddScore(unsigned points) {
	mScore += points;

	D(std::cout << Name() << " scored " << plural(points, "point") << "." << std::endl);
}

Move Hand::ChooseMove(void) const {
    DisplayTiles();
	Move result;
    result.GetUserChoice(mTiles);
    
	return result;
}

void Hand::DisplayTiles(void) const {
	std::cout << Name() << " is holding: " << String(mTiles) << "." << std::endl;
}

void Hand::DisplayName(void) const {
    std::cout << Name();
}

void Hand::DisplayScore(void) const {
    std::cout << Name() << " has " << plural(mScore, "point") << "." << std::endl;
}

unsigned Hand::DrawTiles(unsigned tileCount, Tiles &rBag) {
	D(std::cout << plural(rBag.Count(), "tile") << " in the stock bag." << std::endl);
	D(std::cout << Name() << " wants to draw " << plural(tileCount, "tile") << "." << std::endl);

    unsigned draw_count = mTiles.DrawTiles(tileCount, rBag);

	std::cout << Name() << " drew " << plural(draw_count, "tile") 
		 << " from the stock bag." << std::endl;
	D(std::cout << plural(rBag.Count(), "tile") << " remain." << std::endl);

	return draw_count;
}

// find the longest run in the hand's hand
Tiles Hand::LongestRun(void) const {
	Tiles result = mTiles.LongestRun();
            
	return result;
}

String Hand::Name(void) const {
    return mName;
}

void Hand::RemoveTile(Tile const &rTile) {
	mTiles.RemoveTile(rTile);
}

void Hand::RemoveTiles(Tiles const &rTiles) {
	mTiles.RemoveTiles(rTiles);
}

unsigned Hand::Score(void) const {
    return mScore;
}


// inquiry methods

bool Hand::IsEmpty(void) const {
     bool result = mTiles.IsEmpty();
     
     return result;
}
