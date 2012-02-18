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
#include <time.h>
#include "hand.hpp"
#include "move.hpp"


// lifecycle

Hand::Hand(const String &name, unsigned seconds):
    mName(name)
{
	mRunning = false;
	mScore = 0;
	mSeconds = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment method is fine.

Hand::operator Tiles(void) const {
    return mTiles;
}


// misc methods

void Hand::AddScore(unsigned points) {
	unsigned new_score = mScore + points;
	ASSERT(new_score >= mScore); // check for wrap
	mScore = new_score;
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

unsigned Hand::Seconds(void) const {
	unsigned result = mSeconds;

    if (mRunning) {
	    time_t now = ::time(NULL);
		ASSERT(now >= mStartTime);
        result += unsigned(now - mStartTime);
	}

	return result;
}

void Hand::StartClock(void) {
	ASSERT(!mRunning);
	mStartTime = ::time(NULL);
	mRunning = true;
}

unsigned Hand::StopClock(void) {
	ASSERT(mRunning);
	mSeconds = Seconds();
	mRunning = false;

	return mSeconds;
}


// inquiry methods

bool Hand::IsEmpty(void) const {
     bool result = mTiles.IsEmpty();
     
     return result;
}

bool Hand::IsRunning(void) const {
	return mRunning;
}
