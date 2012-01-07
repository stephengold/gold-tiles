#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <set>
#include <string>
#include "tile.hpp"

using namespace std;

class Player {
	set<Tile> hand;
	string name;
	unsigned score;

	public:
		Player(string &);
		void addScore(unsigned);
		unsigned bestRunLength(void) const;
		set<Tile> chooseTiles(void) const;
		void deleteTile(Tile &);
		void drawTiles(unsigned, set<Tile> &);
		bool handIsEmpty(void) const;
		void printScore(void) const;
};
#endif
