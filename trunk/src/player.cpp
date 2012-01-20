// File:    player.cpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <cstdlib>
#include <iostream>
#include "play.hpp"
#include "player.hpp"

// constructors, assignment, and destructor

Player::Player(void) : hand() {
	name = "";
	score = 0;
}

Player::Player(const string &ns) : hand() {
	name = ns;
	score = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

void Player::addScore(unsigned points) {
	score += points;

	D(cout << name << " scored " << plural(points, "point") << "." << endl);
}

Play Player::choosePlay(void) const {
    displayHand();
	Play result;
    result.getUserChoice(hand);
    
	return result;
}

void Player::displayHand(void) const {
    unsigned count = hand.size();

	cout << "The " << plural(count, "tile") << " in " << name << "'s hand are: " 
		<< hand.toString() << "." << endl;
}

void Player::displayName(void) const {
    cout << name;
}

void Player::displayScore(void) const {
    cout << name << " has " << plural(score, "point") << "." << endl;
}

unsigned Player::drawTiles(unsigned tileCount, Tiles &bag) {
	D(cout << plural(bag.size(), "tile") << " in the stock bag." << endl);
	D(cout << name << " wants to draw " << plural(tileCount, "tile") << "." << endl);

    unsigned numTilesDrawn = hand.drawTiles(tileCount, bag);

	cout << name << " drew " << plural(numTilesDrawn, "tile") << " from the stock bag." << endl;
	D(cout << plural(bag.size(), "tile") << " remain." << endl);

	return numTilesDrawn;
}

string Player::getName(void) const {
    string result = name;
    
    return result;
}

bool Player::handIsEmpty(void) const {
     bool result = hand.empty();
     
     return result;
}

// find the longest run in the player's hand
Tiles Player::longestRun(void) const {
	Tiles result = hand.getLongestRun();
            
	return result;
}

void Player::removeTile(Tile const &tile) {
	hand.removeTile(tile);
}

void Player::removeTiles(Tiles const &tiles) {
    D(cout << "Originally " << plural(hand.size(), "tile") << " in the hand." << endl);

	hand.removeTiles(tiles);

	D(cout << plural(tiles.size(), "tile") << " removed from the hand." << endl);
    D(cout << plural(hand.size(), "tile") << " remain in the hand." << endl);
}

string Player::toString(void) const {
    string result = name;
    
    return result;
}
