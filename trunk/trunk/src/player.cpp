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
	cout << "Select tile(s) to play:" << endl;
	Play result;
    result.getUserChoice(hand);
    
	return result;
}

void Player::displayHand(void) const {
    unsigned count = hand.size();

	cout << plural(count, "tile") << " in " << name << "'s hand:" << endl
        << " " << hand.toString() << "." << endl;
}

void Player::displayName(void) const {
    cout << name;
}

void Player::displayScore(void) const {
    cout << name << " has " << plural(score, "point") << "." << endl;
}

void Player::drawTiles(unsigned tileCount, Tiles &bag) {
    unsigned numTilesRemaining = bag.size();
	D(cout << plural(numTilesRemaining, "tile") << " in the stock bag." << endl);
	D(cout << name << " wants " << plural(tileCount, "tile") << "." << endl);

    unsigned numTilesDrawn;
    for (numTilesDrawn = 0; numTilesDrawn < tileCount; ++numTilesDrawn) {
        if (numTilesRemaining == 0) {
            break;
        }
        Tile tile = bag.drawRandomTile();
        D(cout << name << " drew " << tile.toString() << "." << endl);
		hand.insert(tile);
		--numTilesRemaining;
	}
	D(cout << name << " drew " << plural(numTilesDrawn, "tile") << "." << endl);
	D(cout << plural(numTilesRemaining, "tile") << " remain." << endl);
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
	Tiles result;
    
    Tiles unique = hand.uniqueTiles();
    unsigned count = unique.size();
    string raString;
    D(cout << plural(count, "unique tile") << ":" << endl
        << " " << unique.toString() << "." << endl);
    Tiles::const_iterator tile;
    for (tile = unique.begin(); tile != unique.end(); tile++) {
        Tile t = *tile;
        for (AIndex ind = 0; ind < t.getNumAttributes(); ind++) {
            AValue value = t.getAttribute(ind);
            D(cout << "Tile " << t.toString() << " attribute #" << (ind + 1) << " is " 
                << attributeToString(ind, value) << "." << endl);
            Tiles run;
            Tiles::const_iterator t2;
            for (t2 = tile; t2 != unique.end(); t2++) {
                if (t2->hasAttribute(ind, value)) {
                    run.insert(*t2);
                }
            }
            if (run.size() > result.size()) {
                result = run;
                raString = attributeToString(ind, value);
            }
        }
    }
    
    unsigned n = result.size();
    D(cout << name << " has a run of " << n << " " << raString << plural(n) << "." << endl);
            
	return result;
}

void Player::removeTile(Tile const &tile) {
    set<Tile>::iterator position = hand.find(tile);
    ASSERT(position != hand.end());
	hand.erase(tile);
}

void Player::removeTiles(Tiles const &tSet) {
    unsigned nOrig = hand.size();
    unsigned nRemove = tSet.size();

	set<Tile>::const_iterator tile;
	for (tile = tSet.begin(); tile != tSet.end(); tile++) {
        removeTile(*tile);
    }
    D(cout << "Originally there were " << plural(nOrig, "tile") << " in the hand." << endl);
    D(cout << plural(nRemove, "tile") << " removed from the hand." << endl);
    D(cout << plural(hand.size(), "tile") << " remain in the hand." << endl);
}
