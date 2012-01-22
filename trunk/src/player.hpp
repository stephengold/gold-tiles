#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

// File:    player.hpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <string>
#include "tiles.hpp"

class Player {
	Tiles _hand;
	string _name;
	unsigned _score;

	public:
        Player(void);
		Player(string const &);

		void addScore(unsigned);
		Play choosePlay(void) const;
		void display(void) const;
		void displayHand(void) const;
		void displayName(void) const;
		void displayScore(void) const;
		unsigned drawTiles(unsigned, Tiles &);
		string getName(void) const;
		bool handIsEmpty(void) const;
		Tiles longestRun(void) const;
		void removeTile(Tile const &);
		void removeTiles(Tiles const &);
		string toString(void) const;
};
#endif
