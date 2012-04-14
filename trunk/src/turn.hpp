#ifndef TURN_HPP_INCLUDED
#define TURN_HPP_INCLUDED

// File:     turn.hpp
// Location: src
// Purpose:  declare Turn class
// Author:   Stephen Gold sgold@sonic.net
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

/*
A Turn object represents a turn played on a specific hand.

The Move class composes a Move with the set of tiles drawn
from the stock bag after the move, along with other information
about the play.
*/

#include "move.hpp"  // HASA Move
#include "tiles.hpp" // HASA Tiles

class Turn {
public:
	// lifecycle
	Turn(Tiles const&, String const& handName);
	Turn(Move const&, String const& handName, unsigned mustPlay);
	// no default constructor
	// Turn(Turn const&);  compiler-generated copy constructor is OK
	// ~Turn(void);  compiler-generated destructor is OK

	// public operators
	// Turn const& operator=(Turn const&);  compiler-generated assignment operator is OK
	bool operator==(Turn const&) const;
    operator Move(void) const;
    operator String(void) const;
    
    // misc methods
	Tiles    Draw(void) const;
	String   HandName(void) const;
	unsigned MustPlay(void) const;
	unsigned Points(void) const;
    void     SetDraw(Tiles const&);
    void     SetPoints(unsigned);

private:
	// private data
	Tiles    mDraw;      // move first, then draw
	String   mHandName;
	Move     mMove;      // move first, then draw
	unsigned mMustPlay;  // set for first turn, otherwise zero
	unsigned mPoints;    // from start of turn
};
#endif // !defined(TURN_HPP_INCLUDED)