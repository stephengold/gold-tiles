// File:    player.cpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <cstdlib>
#include <iostream>
#include "player.hpp"

Player::Player(void) {
    hand = set<Tile>();
	name = "";
	score = 0;
}

Player::Player(const string &ns) {
    hand = set<Tile>();
	name = ns;
	score = 0;
}

Player::Player(const Player &p) {
	hand = p.hand;
	name = p.name;
	score = p.score;
}

void Player::addScore(unsigned points) {
	this->score += points;
}

unsigned Player::bestRunLength(void) const {
	unsigned best = 0;
    // TODO
	
	return best;
}

set<Tile> Player::chooseTiles(void) const {
	set<Tile> s;

	//TODO

	return s;
}

void Player::deleteTile(Tile & tile) {
	this->hand.erase(tile);
}

void Player::drawTiles(unsigned tileCount, set<Tile> &bag) {
    unsigned size = bag.size();
    for (unsigned di = 0; di < tileCount; di++) {
		unsigned r = rand() % size;

		set<Tile>::iterator tIndex = bag.begin();
		for (unsigned ti = 0; ti < r; ti++) {
            tIndex++;
        }

		hand.insert(*tIndex);
		bag.erase(tIndex);
		--size;
	}
}

bool Player::handIsEmpty(void) const {
     return hand.empty();
}

void Player::printHand(void) const {
	cout << name << " has";
	set<Tile>::const_iterator i;
	for (i = hand.begin(); i != hand.end(); i++) {
        i->display();
    }
	cout << ".\n";
	system("PAUSE");
}

void Player::printScore(void) const {
	cout << name << " has " << score << " point";
	if (score != 1) {
		cout << "s";
	}
	cout << ".\n";
	system("PAUSE");
}
