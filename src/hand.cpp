// File:     hand.cpp
// Location: src
// Purpose:  implement Hand class
// Author:   Stephen Gold sgold@sonic.net
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
#include "network.hpp"
#include "partial.hpp"


// lifecycle

Hand::Hand(
	String const& rHandName,
	HandOpt const& rOptions,
	Socket const& rClientSocket)
:
    mName(rHandName),
	mOptions(rOptions)
{
	mClockRunningFlag = false;
	mMilliseconds = 0L;
	if (rClientSocket.IsValid() && Address(rOptions) == rClientSocket.Peer()) {
		mSocket = rClientSocket;
	}
	// mStartTime gets initialized in StartClock()
	Restart();
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment method is fine.

bool Hand::operator==(Hand const& rOther) const {
	bool const result = (Name() == rOther.Name());

	return result;
}

Hand::operator Address(void) const {
	Address const result = mOptions;

	return result;
}

Hand::operator HandOpt(void) const {
	return mOptions;
}

Hand::operator Socket(void) const {
	return mSocket;
}

Hand::operator Tiles(void) const {
    return mTiles;
}


// misc methods

void Hand::AddScore(ScoreType points) {
    ASSERT(!IsClockRunning());

	ScoreType const new_score = mScore + points;
	ASSERT(new_score >= mScore); // check for wraparound
	mScore = new_score;
}

void Hand::AddTiles(Tiles const& rTiles) {
    mTiles.Merge(rTiles);
}

Move Hand::ChooseMove(unsigned mustPlay) const {
    ASSERT(IsClockRunning());
    ASSERT(IsLocalUser());
    
    DescribeTiles();
	Move result;
    result.GetUserChoice(mTiles, mustPlay);
    
	return result;
}

void Hand::ConnectToServer(Game& rGame) {
	ASSERT(IsRemote());
	ASSERT(rGame.AmClient());

    Address const server_address = *this;
	if (!rGame.IsConnectedToServer(server_address)) {
		Network::ConnectToServer(server_address, rGame);
	}
}

void Hand::DescribeName(void) const {
    std::cout << Name();
}

void Hand::DescribeScore(void) const {
	DescribeName();
    std::cout << " has " << ::plural(mScore, "point") << "." << std::endl;
}

void Hand::DescribeTiles(void) const {
	DescribeName();
	unsigned const count = mTiles.Count();
	std::cout << " holds " << ::plural(count, "tile") << ": " 
		      << mTiles.Description() << "." << std::endl;
}

Move Hand::GetAutomaticMove(Game const& rGame) const {
	double skip_probability = SkipProbability();
	if (rGame.MustPlay() > 0) {
		skip_probability = 0.0;
	}
    Partial partial(&rGame, HINT_NONE, skip_probability);
    partial.Suggest();
    Move result = partial.GetMove(false);

    DescribeName();
	std::cout << " " << result.Description() << "." << std::endl;

	return result;
}

Move Hand::GetRemoteMove(void) {
    Address const address = *this;
	String const move_string = mSocket.GetLine();
	Move const result = Move(move_string, true);

    DescribeName();
	std::cout << " " << result.Description() << "." << std::endl;

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

Tiles Hand::PullRandomTiles(unsigned tileCount, Tiles& rBag) {
    Tiles result;
    result.PullRandomTiles(tileCount, rBag);

	unsigned const count = result.Count();
	if (count > 0) {
	    std::cout << Name() << " drew " << plural(count, "tile") 
		     << " from the stock bag." << std::endl;
	}

    mTiles.Merge(result);

	return result;
}

void Hand::RemoveTile(Tile const& rTile) {
	mTiles.Remove(rTile);
}

void Hand::RemoveTiles(Tiles const& rTiles) {
	mTiles.Purge(rTiles);
}

void Hand::Resign(Tiles& rBag) {
    ASSERT(!IsClockRunning());
	ASSERT(!HasResigned());

	rBag.Merge(mTiles);
	mTiles.MakeEmpty();
	mResignedFlag = true;

	ASSERT(HasResigned());
}

void Hand::Restart(void) {
	ASSERT(!IsClockRunning());

	mResignedFlag = false;
	mScore = 0;
	mTiles.MakeEmpty();
}

// total points this hand has scored
ScoreType Hand::Score(void) const {
    return mScore;
}

// how much time this hand has spent, in seconds
unsigned Hand::Seconds(void) const {
	MsecIntervalType const msecs = Milliseconds();
    unsigned const result = unsigned(msecs/MSECS_PER_SECOND);

	return result;
}

void Hand::SetSocket(Socket const& rSocket) {
    ASSERT(!mSocket.IsValid());
    ASSERT(rSocket.IsValid());

	mSocket = rSocket;

    ASSERT(rSocket == mSocket);
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

void Hand::SubtractScore(ScoreType points) {
	ASSERT(!HasResigned());
    ASSERT(!IsClockRunning());

	ScoreType const new_score = mScore - points;
	ASSERT(new_score <= mScore); // check for wraparound
	mScore = new_score;
}

void Hand::Unresign(Tiles& rBag, Tiles const& rHand) {
	ASSERT(HasResigned());
    ASSERT(!IsClockRunning());

	rBag.Purge(rHand);
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

bool Hand::IsClockRunning(void) const {
	return mClockRunningFlag;
}

bool Hand::IsEmpty(void) const {
     bool const result = mTiles.IsEmpty();
     
     return result;
}

bool Hand::IsLocalUser(void) const {
    bool const result = mOptions.IsLocalUser();

	return result;
}

bool Hand::IsRemote(void) const {
	bool const result = mOptions.IsRemote();

	return result;
}
