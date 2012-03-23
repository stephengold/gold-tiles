#ifndef HAND_HPP_INCLUDED
#define HAND_HPP_INCLUDED

// File:    hand.hpp
// Purpose: Hand class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

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

#include "string.hpp"  // HASA String
#include "tiles.hpp"   // HASA Tiles

class Hand {
public:
	// public lifecycle
    Hand(String const &name, String const &playerName, bool autom);
	void Restart(void);
	// no default constructor
    // Hand(Hand const &); The compiler-generated copy constructor is fine.
    //~Hand(void); The compiler-generated destructor is fine.

	// public operators
    // Hand &operator=(Hand const &); The compiler-generated assignment method is fine.
	bool operator==(Hand const &) const;
	operator Tiles(void) const;

	// misc public methods
	void     AddScore(unsigned);
	void     AddTiles(Tiles const &);
	Move     ChooseMove(void) const;
	void     Display(void) const;
	void     DisplayName(void) const;
	void     DisplayScore(void) const;
	void     DisplayTiles(void) const;
	Tiles    DrawTiles(unsigned, Tiles &bag);
	Tiles    LongestRun(void) const;
	MsecIntervalType
		     Milliseconds(void) const;
	String   Name(void) const;
	String   PlayerName(void) const;
	void     RemoveTile(Tile const &);
	void     RemoveTiles(Tiles const &);
	void     Resign(Tiles &bag);
	unsigned Score(void) const;
	unsigned Seconds(void) const;
	void     StartClock(void);
	unsigned StopClock(void);
	void     SubtractScore(unsigned);
	void     Unresign(Tiles &bag, Tiles const &hand);

	// public inquiry methods
	bool HasGoneOut(void) const;
	bool HasResigned(void) const;
	bool IsAutomatic(void) const;
	bool IsClockRunning(void) const;
	bool IsLocalPlayer(void) const;

private:
	// private data
	bool      mAutomatic;
	bool      mClockRunning;
	MsecIntervalType
		      mMilliseconds;   // total time charged to the hand
	String    mName;           // the "name" of the hand
	String    mPlayerName;
	bool      mResigned;
	unsigned  mScore;
	MsecIntervalType
		      mStartTime;
	Tiles     mTiles;

	// private inquiry methods
	bool IsEmpty(void) const;
};
#endif
