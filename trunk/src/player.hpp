#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

// File:    player.hpp
// Purpose: Player class
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

#include "string.hpp"
#include "tiles.hpp"

class Player {
public:
	// lifecycle
    Player(void);
    Player(String const &);
    // Player(Player const &); The compiler-generated copy constructor is fine.
    //~Player(void); The compiler-generated destructor is fine.

	// operators
    // Player &operator=(Player const &); The compiler-generated assignment method is fine.
	operator Tiles(void) const;

	// misc
	void     AddScore(unsigned);
	Move     ChooseMove(void) const;
	void     Display(void) const;
	void     DisplayHand(void) const;
	void     DisplayName(void) const;
	void     DisplayScore(void) const;
	unsigned DrawTiles(unsigned, Tiles &);
	Tiles    LongestRun(void) const;
	void     RemoveTile(Tile const &);
	void     RemoveTiles(Tiles const &);

	// access
	String   Name(void) const;
	unsigned Score(void) const;

	// inquiry
	bool     IsEmptyHanded(void) const;

private:
	Tiles    mHand;
	String   mName;
	unsigned mScore;
};
#endif
