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
    Address const hand_address = rOptions;
    if (rClientSocket.IsValid() && hand_address == rClientSocket.Peer()) {
        mSocket = rClientSocket;
    }
    // mStartTime gets initialized in StartClock()
    Restart();
}

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment method is fine.

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

bool Hand::ConnectToServer(Game& rGame) {
    ASSERT(IsRemote());
    ASSERT(rGame.AmClient());

    bool result = true;

    Address const server_address = *this;
    if (!rGame.IsConnectedToServer(server_address)) {
        result = Network::ConnectToServer(server_address, rGame);
    }

    return result;
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

void Hand::DisableServer(void) {
    mSocket.Invalidate();
}

Move Hand::GetAutomaticMove(Game const& rGame) const {
    Fraction skip_probability = SkipProbability();
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

// Return true if successful, false if canceled.
bool Hand::GetRemoteMove(Move& rMove) {
    ASSERT(IsRemote());

    if (!mSocket.IsValid()) {
        return false;
    }

    String move_string;
    bool const success = mSocket.GetLine(move_string);
    if (success) {
        rMove = Move(move_string, true);
#ifdef _CONSOLE
        DescribeName();
        std::cout << " " << rMove.Description() << "." << std::endl;
#endif // defined(_CONSOLE)
    }

    return success;
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
}

Fraction Hand::SkipProbability(void) const {
    Fraction const result = mOptions.SkipProbability();

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

bool Hand::IsDisconnected(void) const {
    bool const result = IsRemote() && !mSocket.IsValid();

    return result;
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
