// File:    player.cpp
// Purpose: Player class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of theGNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "move.hpp"
#include "player.hpp"

// constructors, assignment, and destructor

Player::Player(void) {
	mScore = 0;
}

Player::Player(const String &name) {
	mName = name;
	mScore = 0;
}

// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

Player::operator Tiles(void) const {
    Tiles result = mHand;

    return result;
}


// methods

void Player::AddScore(unsigned points) {
	mScore += points;

	D(std::cout << Name() << " scored " << plural(points, "point") << "." << std::endl);
}

Move Player::ChooseMove(void) const {
    DisplayHand();
	Move result;
    result.GetUserChoice(mHand);
    
	return result;
}

void Player::DisplayHand(void) const {
	std::cout << Name() << " is holding: " << String(mHand) << "." << std::endl;
}

void Player::DisplayName(void) const {
    std::cout << Name();
}

void Player::DisplayScore(void) const {
    std::cout << Name() << " has " << plural(mScore, "point") << "." << std::endl;
}

unsigned Player::DrawTiles(unsigned tileCount, Tiles &rBag) {
	D(std::cout << plural(rBag.Count(), "tile") << " in the stock bag." << std::endl);
	D(std::cout << Name() << " wants to draw " << plural(tileCount, "tile") << "." << std::endl);

    unsigned draw_count = mHand.DrawTiles(tileCount, rBag);

	std::cout << Name() << " drew " << plural(draw_count, "tile") 
		 << " from the stock bag." << std::endl;
	D(std::cout << plural(rBag.Count(), "tile") << " remain." << std::endl);

	return draw_count;
}

bool Player::IsEmptyHanded(void) const {
     bool result = mHand.IsEmpty();
     
     return result;
}

// find the longest run in the player's hand
Tiles Player::LongestRun(void) const {
	Tiles result = mHand.LongestRun();
            
	return result;
}

String Player::Name(void) const {
    String result = mName;
    
    return result;
}

void Player::RemoveTile(Tile const &rTile) {
	mHand.RemoveTile(rTile);
}

void Player::RemoveTiles(Tiles const &rTiles) {
    D(std::cout << "Originally " << plural(mHand.Count(), "tile") << " in the hand." << std::endl);

	mHand.RemoveTiles(rTiles);

	D(std::cout << plural(rTiles.Count(), "tile") << " removed from the hand." << std::endl);
    D(std::cout << plural(mHand.Count(), "tile") << " remain in the hand." << std::endl);
}

unsigned Player::Score(void) const {
    unsigned result = mScore;
    
    return result;
}