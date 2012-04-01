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
#include "turn.hpp"


// lifecycle

Hand::Hand(const String &rHandName, HandOpt const &rOptions):
    mName(rHandName),
	mOptions(rOptions)
{
	mClockRunningFlag = false;
	Restart();
}

void Hand::Restart(void) {
	ASSERT(!IsClockRunning());

	mMilliseconds = 0L;
	mResignedFlag = false;
	mScore = 0;
	// mStartTime gets initialized in StartClock()
	mTiles.MakeEmpty();
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment method is fine.

bool Hand::operator ==(Hand const &rOther) const {
	bool const result = (Name() == rOther.Name());

	return result;
}

Hand::operator HandOpt(void) const {
	return mOptions;
}

Hand::operator Tiles(void) const {
    return mTiles;
}


// misc methods

void Hand::AddScore(unsigned points) {
    ASSERT(!IsClockRunning());

	unsigned const new_score = mScore + points;
	ASSERT(new_score >= mScore); // check for wraparound
	mScore = new_score;
}

void Hand::AddTiles(Tiles const &rTiles) {
    mTiles.AddTiles(rTiles);
}

Move Hand::ChooseMove(void) const {
    ASSERT(IsClockRunning());
    ASSERT(IsLocalUser());
    
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

Tiles Hand::DrawTiles(unsigned tileCount, Tiles &rBag) {
    Tiles result;
    result.DrawTiles(tileCount, rBag);

	unsigned const count = result.Count();
	if (count > 0) {
	    std::cout << Name() << " drew " << plural(count, "tile") 
		     << " from the stock bag." << std::endl;
	}

    mTiles.AddTiles(result);

	return result;
}

// find the longest run of compatible tiles in the hand
Tiles Hand::LongestRun(void) const {
	Tiles const result = mTiles.LongestRun();
            
	return result;
}

// read the hand's elapsed time clock
// and if the clock is running, update mTurnMilliseconds
MsecIntervalType Hand::Milliseconds(void) const {
	MsecIntervalType result = mMilliseconds;

    if (mClockRunningFlag) {
	    MsecIntervalType const now = ::milliseconds();
		ASSERT(now >= mStartTime);
		MsecIntervalType const turn_msec = now - mStartTime;
        result += turn_msec;
		ASSERT(result >= mMilliseconds);
	}

	return result;
}

String Hand::Name(void) const {
    return mName;
}

String Hand::PlayerName(void) const {
    String result = mOptions.PlayerName();

	return result;
}

void Hand::RemoveTile(Tile const &rTile) {
	mTiles.RemoveTile(rTile);
}

void Hand::RemoveTiles(Tiles const &rTiles) {
	mTiles.RemoveTiles(rTiles);
}

void Hand::Resign(Tiles &rBag) {
    ASSERT(!IsClockRunning());
	ASSERT(!HasResigned());

	rBag.AddTiles(mTiles);
	mTiles.MakeEmpty();
	mResignedFlag = true;

	ASSERT(HasResigned());
}

// total points this hand has scored
unsigned Hand::Score(void) const {
    return mScore;
}

// how much time this hand has spent, in seconds
unsigned Hand::Seconds(void) const {
	MsecIntervalType const msecs = Milliseconds();
    unsigned const result = unsigned(msecs/MSECS_PER_SECOND);

	return result;
}

double Hand::SkipProbability(void) const {
	double result = mOptions.SkipProbability();

	return result;
}

void Hand::StartClock(void) {
	ASSERT(!mClockRunningFlag);

	mStartTime = ::milliseconds();
	mClockRunningFlag = true;
}

unsigned Hand::StopClock(void) {
	ASSERT(IsClockRunning());

	mMilliseconds = Milliseconds();
	mClockRunningFlag = false;

	// return the number of elapsed seconds
	unsigned const result = unsigned(mMilliseconds/MSECS_PER_SECOND);

	return result;
}

void Hand::SubtractScore(unsigned points) {
	ASSERT(!HasResigned());
    ASSERT(!IsClockRunning());

	unsigned const new_score = mScore - points;
	ASSERT(new_score <= mScore); // check for wraparound
	mScore = new_score;
}

void Hand::Unresign(Tiles &rBag, Tiles const &rHand) {
	ASSERT(HasResigned());
    ASSERT(!IsClockRunning());

	rBag.RemoveTiles(rHand);
	mTiles = rHand;
	mResignedFlag = false;

	ASSERT(!HasResigned());
}


// inquiry methods

bool Hand::HasGoneOut(void) const {
    bool const result = IsEmpty() && !HasResigned();
	
	return result;
}

bool Hand::HasResigned(void) const {
    return mResignedFlag;
}

bool Hand::IsAutomatic(void) const {
    bool const result = mOptions.IsAutomatic();

	return result;
}

bool Hand::IsEmpty(void) const {
     bool const result = mTiles.IsEmpty();
     
     return result;
}

bool Hand::IsClockRunning(void) const {
	return mClockRunningFlag;
}

bool Hand::IsLocalUser(void) const {
    bool const result = mOptions.IsLocalUser();

	return result;
}

