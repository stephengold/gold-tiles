// File:    player.cpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <cstdlib>
#include <iostream>
#include "player.hpp"

Player::Player(string &name) {
	this->name = name;
	this->score = 0;
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
		for (unsigned ri = 0; ri < r; ri++) {
            tIndex++;
        }
		Tile tile = *tIndex;

		--size;
		hand.insert(tile);
		bag.erase(tIndex);
	}
}

bool Player::handIsEmpty(void) const {
     return this->hand.empty();
}

void Player::printScore() const {
	unsigned score = this->score;

	cout << this->name << " has " << score << " point";
	if (score != 1) {
		cout << "s";
	}
	cout << ".\n";
}
