#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

// File:    player.hpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
	operator Tiles(void) const;

	// inquiry
	bool     IsEmptyHanded(void) const;

private:
	Tiles    mHand;
	String   mName;
	unsigned mScore;
};
#endif
