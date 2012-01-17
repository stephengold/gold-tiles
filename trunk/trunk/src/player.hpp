#ifndef PLAYER_HPP
#define PLAYER_HPP

// File:    player.hpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <set>
#include <string>
#include "tile.hpp"
#include "tiles.hpp"

using namespace std;

class Player {
	Tiles hand;
	string name;
	unsigned score;

	public:
        Player(void);
		Player(string const &);

		void addScore(unsigned);
		Tiles chooseTiles(void) const;
		void drawTiles(unsigned, Tiles &);
		string getName(void) const;
		bool handIsEmpty(void) const;
		Tiles longestRun(void) const;
		void printHand(void) const;
		void printName(void) const;
		void printScore(void) const;
		void removeTile(Tile const &);
		void removeTiles(Tiles const &);
};
#endif
