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
	// lifecycle
    Hand(String const &name, String const &playerName, unsigned seconds);
	// no default constructor
    // Hand(Hand const &); The compiler-generated copy constructor is fine.
    //~Hand(void); The compiler-generated destructor is fine.

	// operators
    // Hand &operator=(Hand const &); The compiler-generated assignment method is fine.
	operator Tiles(void) const;

	// misc public methods
	void     AddScore(unsigned);
	Move     ChooseMove(void) const;
	void     Display(void) const;
	void     DisplayName(void) const;
	void     DisplayScore(void) const;
	void     DisplayTiles(void) const;
	unsigned DrawTiles(unsigned, Tiles &);
	Tiles    LongestRun(void) const;
	String   Name(void) const;
	String   PlayerName(void) const;
	void     RemoveTile(Tile const &);
	void     RemoveTiles(Tiles const &);
	unsigned Score(void) const;
	unsigned Seconds(void) const;
	void     StartClock(void);
	unsigned StopClock(void);

	// public inquiry methods
	bool IsEmpty(void) const;
	bool IsRunning(void) const;

private:
	String   mName;    // the "name" of this hand
	String   mPlayerName; 
	bool     mRunning; // is the clock running?
	unsigned mScore;
	unsigned mSeconds;
	time_t   mStartTime;
	Tiles    mTiles;
};
#endif
