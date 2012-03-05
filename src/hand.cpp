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
#include "hand.hpp"
#include "move.hpp"


// lifecycle

Hand::Hand(const String &name, const String &playerName, bool autoFlag):
    mName(name),
	mPlayerName(playerName)
{
	mAutomatic = autoFlag;
	mClockRunning = false;
	mMilliseconds = 0L;
	mResigned = false;
	mScore = 0;
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
	ASSERT(new_score >= mScore); // check for wraparound
	mScore = new_score;
}

Move Hand::ChooseMove(void) const {
    DisplayTiles();
	Move result;
    result.GetUserChoice(mTiles);
    
	return result;
}

void Hand::DisplayName(void) const {
    std::cout << Name();
}

void Hand::DisplayScore(void) const {
    std::cout << Name() << " has " << plural(mScore, "point") << "." << std::endl;
}

void Hand::DisplayTiles(void) const {
	std::cout << Name() << " is holding: " << String(mTiles) << "." << std::endl;
}

unsigned Hand::DrawTiles(unsigned tileCount, Tiles &rBag) {
    unsigned draw_count = mTiles.DrawTiles(tileCount, rBag);

	std::cout << Name() << " drew " << plural(draw_count, "tile") 
		 << " from the stock bag." << std::endl;

	return draw_count;
}

// find the longest run of compatible tiles in the hand
Tiles Hand::LongestRun(void) const {
	Tiles result = mTiles.LongestRun();
            
	return result;
}

// read the hand's elapsed time clock, in milliseconds
long Hand::Milliseconds(void) const {
	long result = mMilliseconds;

    if (mClockRunning) {
	    long now = ::milliseconds();
		ASSERT(now >= mStartTime);  // check for wraparound
        result += unsigned(now - mStartTime);
	}

	return result;
}

String Hand::Name(void) const {
    return mName;
}

String Hand::PlayerName(void) const {
    return mPlayerName;
}

void Hand::RemoveTile(Tile const &rTile) {
	mTiles.RemoveTile(rTile);
}

void Hand::RemoveTiles(Tiles const &rTiles) {
	mTiles.RemoveTiles(rTiles);
}

void Hand::Resign(Tiles &rBag) {
	ASSERT(!HasResigned());

	rBag.AddTiles(mTiles);
	mTiles.MakeEmpty();
	mResigned = true;

	ASSERT(HasResigned());
}

unsigned Hand::Score(void) const {
    return mScore;
}

unsigned Hand::Seconds(void) const {
	long msecs = Milliseconds();
    unsigned result = unsigned(msecs/MSECS_PER_SECOND);
	return result;
}

void Hand::StartClock(void) {
	ASSERT(!mClockRunning);
	mStartTime = ::milliseconds();
	mClockRunning = true;
}

unsigned Hand::StopClock(void) {
	ASSERT(mClockRunning);
	mMilliseconds = Milliseconds();
	mClockRunning = false;

	// return the number of elapsed seconds
	unsigned result = unsigned(mMilliseconds/MSECS_PER_SECOND);

	return result;
}


// inquiry methods

bool Hand::HasGoneOut(void) const {
    bool result = IsEmpty() && !HasResigned();
	
	return result;
}

bool Hand::HasResigned(void) const {
    return mResigned;
}

bool Hand::IsAutomatic(void) const {
    return mAutomatic;
}

bool Hand::IsEmpty(void) const {
     bool result = mTiles.IsEmpty();
     
     return result;
}

bool Hand::IsClockRunning(void) const {
	return mClockRunning;
}

bool Hand::IsLocalPlayer(void) const {
    bool result = !mAutomatic;

	return result;
}

