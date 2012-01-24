// File:    player.cpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <cstdlib>
#include <iostream>
#include "play.hpp"
#include "player.hpp"

// constructors, assignment, and destructor

Player::Player(void) {
	_name = "";
	_score = 0;
}

Player::Player(const string &ns) {
	_name = ns;
	_score = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

void Player::addScore(unsigned points) {
	_score += points;

	D(cout << _name << " scored " << plural(points, "point") << "." << endl);
}

Play Player::choosePlay(void) const {
    displayHand();
	Play result;
    result.getUserChoice(_hand);
    
	return result;
}

void Player::displayHand(void) const {
	cout << _name << "'s hand contains: " << _hand.toString() << "." << endl;
}

void Player::displayName(void) const {
    cout << _name;
}

void Player::displayScore(void) const {
    cout << _name << " has " << plural(_score, "point") << "." << endl;
}

unsigned Player::drawTiles(unsigned tileCount, Tiles &bag) {
	D(cout << plural(bag.size(), "tile") << " in the stock bag." << endl);
	D(cout << _name << " wants to draw " << plural(tileCount, "tile") << "." << endl);

    unsigned numTilesDrawn = _hand.drawTiles(tileCount, bag);

	cout << _name << " drew " << plural(numTilesDrawn, "tile") << " from the stock bag." << endl;
	D(cout << plural(bag.size(), "tile") << " remain." << endl);

	return numTilesDrawn;
}

Tiles Player::getHand(void) const {
    Tiles result = _hand;

    return result;
}

string Player::getName(void) const {
    string result = _name;
    
    return result;
}

bool Player::handIsEmpty(void) const {
     bool result = _hand.empty();
     
     return result;
}

// find the longest run in the player's hand
Tiles Player::longestRun(void) const {
	Tiles result = _hand.getLongestRun();
            
	return result;
}

void Player::removeTile(Tile const &tile) {
	_hand.removeTile(tile);
}

void Player::removeTiles(Tiles const &tiles) {
    D(cout << "Originally " << plural(_hand.size(), "tile") << " in the hand." << endl);

	_hand.removeTiles(tiles);

	D(cout << plural(tiles.size(), "tile") << " removed from the hand." << endl);
    D(cout << plural(_hand.size(), "tile") << " remain in the hand." << endl);
}

string Player::toString(void) const {
    string result = _name;
    
    return result;
}
