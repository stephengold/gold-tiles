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

// get length of longest run in the player's hand
unsigned Player::bestRunLength(void) const {
	unsigned best = 0;
    
    for (unsigned di = 0; di < Tile::getNumAttributes(); di++) {
        for (unsigned ai = 1; ai <= Tile::getMaxAttribute(di); ai++) {
            unsigned count = 0;
            set<Tile>::const_iterator ti;
            for (ti = hand.begin(); ti != hand.end(); ti++) {
                if (ti->hasAttribute(di, ai)) {
                    count++;
                }
            }
            if (count > best) {
                best = count;
            }
        }
    }
	
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
	cout << name << " drew " << tileCount << " tile(s).\n";
	cout << size << " tile(s) remain.\n";
}

string Player::getName(void) const {
    return name;
}

bool Player::handIsEmpty(void) const {
     return hand.empty();
}

void Player::printName(void) const {
    cout << name;
}

void Player::printHand(void) const {
    printName();
	cout << " has";
	set<Tile>::const_iterator i;
	for (i = hand.begin(); i != hand.end(); i++) {
        i->display();
    }
	cout << ".\n";
}

void Player::printScore(void) const {
	printName();
    cout << " has " << score << " point";
	if (score != 1) {
		cout << "s";
	}
	cout << ".\n";
	system("PAUSE");
}
