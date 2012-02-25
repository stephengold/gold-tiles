// File:    player.cpp
// Purpose: Player class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

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

#include "gui/player.hpp"
#include "gui/resource.hpp"
#include "project.hpp"    // ASSERT

Player::Map Player::msMap;

// lifecycle

Player::Player(String const &rName):
    mName(rName)
{
	mAutocenter = false;
	mAutopause = false;
	mPeek = false;
	mShowClocks = false;
	mShowGrid = false;
	mShowScores = false;
	mTileSize = IDM_LARGE_TILES;
}

// The compiler-generated destructor is OK.


// misc methods

/* static */ Player *Player::Lookup(String const &rName) {
    ConstIterator i_player = msMap.find(rName);

	Player *result = NULL;
    if (i_player != msMap.end()) {
        result = i_player->second;
    } else {
		result = new Player(rName);
	    ASSERT(result != NULL);

		Pair new_mapping(rName, result); 
	    InsertResult ins_result = msMap.insert(new_mapping);
	    bool success = ins_result.second;
	    ASSERT(success);
	}

	ASSERT(result != NULL);
	return result;
}

void Player::SetAutocenter(bool value) {
	mAutocenter = value;
}

void Player::SetAutopause(bool value) {
	mAutopause = value;
}

void Player::SetPeek(bool value) {
	mPeek = value;
}

void Player::SetShowClocks(bool value) {
    mShowClocks = value;
}

void Player::SetShowGrid(bool value) {
    mShowGrid = value;
}

void Player::SetShowScores(bool value) {
    mShowScores = value;
}

void Player::SetTileSize(IdType value) {
	ASSERT(value == IDM_LARGE_TILES
		 || value == IDM_MEDIUM_TILES
		 || value == IDM_SMALL_TILES);

	mTileSize = value;
}

IdType Player::TileSize(void) const {
	return mTileSize;
}


// inquiry methods
bool Player::Autocenter(void) const {
	return mAutocenter;
}

bool Player::Autopause(void) const {
    return mAutopause;
}

bool Player::Peek(void) const {
    return mPeek;
}

bool Player::ShowClocks(void) const {
	return mShowClocks;
}

bool Player::ShowGrid(void) const {
	return mShowGrid;
}

bool Player::ShowScores(void) const {
	return mShowScores;
}
