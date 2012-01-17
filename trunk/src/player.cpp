// File:    player.cpp
// Purpose: Player class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <cstdlib>
#include <iostream>
#include "player.hpp"

// constructors

Player::Player(void) : hand() {
	name = "";
	score = 0;
}

Player::Player(const string &ns) : hand() {
	name = ns;
	score = 0;
}

// member functions

void Player::addScore(unsigned points) {
	score += points;

	cout << name << " scored " << plural(points, "point") << "." << endl;
	pause();
}

Tiles Player::chooseTiles(void) const {
	Tiles s;

	cout << "Enter tile to play or 'end':" << endl;
	string str;
	cin >> str;
	if (str != "end") {
        Tile t(str);
        s.insert(t);
    }

	return s;
}

void Player::drawTiles(unsigned tileCount, Tiles &bag) {
    unsigned numTilesRemaining = bag.size();
    unsigned numTilesDrawn;
    
    for (numTilesDrawn = 0; numTilesDrawn < tileCount; ++numTilesDrawn) {
        if (numTilesRemaining == 0) {
            break;
        }
		unsigned r = rand() % numTilesRemaining;

        // find the "r"th tile in the bag
		Tiles::iterator tile = bag.begin();
		for (unsigned ti = 0; ti < r; ti++) {
            tile++;
        }

		hand.insert(*tile);
		bag.erase(tile);
		--numTilesRemaining;
	}
	
	cout << name << " drew " << plural(numTilesDrawn, "tile") << "." << endl
        << plural(numTilesRemaining, "tile") << " remain." << endl;
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
#if 0    
    cout << plural(count, "unique tile") << ":" << endl
        << " " << toString(unique) << "." << endl;
#endif    
    Tiles::const_iterator tile;
    for (tile = unique.begin(); tile != unique.end(); tile++) {
        Tile t = *tile;
        for (unsigned ind = 0; ind < t.getNumAttributes(); ind++) {
            unsigned value = t.getAttribute(ind);
#if 0
            cout << "Tile " << t.toString() << " attribute #" << (ind + 1) << " is " 
                << attributeToString(ind, value) << "." << endl;
            pause();
#endif
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
    cout << name << " has a run of " << n << " " << raString << plural(n) << "." << endl;
            
	return result;
}

void Player::printHand(void) const {
    unsigned count = hand.size();
    
	cout << name << "'s hand contains " << plural(count, "tile") << ": " << endl << " "
        << hand.toString() << "." << endl;
}

void Player::printName(void) const {
    cout << name;
}

void Player::printScore(void) const {
    cout << name << " has " << plural(score, "point") << "." << endl;
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
#if 0
    unsigned nRemain = hand.size();
    cout << "Originally there were " << plural(nOrig, "tile") << " in the hand." << endl;
    cout << plural(nRemove, "tile") << " removed from the hand." << endl;
    cout << plural(nRemain, "tile") << " remain in the hand." << endl;
#endif
}
