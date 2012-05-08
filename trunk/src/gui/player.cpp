// File:     player.cpp
// Location: src/gui
// Purpose:  implement Player class
// Author:   Stephen Gold sgold@sonic.net
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

#include "gui/gameview.hpp" // TILE_SIZE_DEFAULT
#include "gui/player.hpp"
#include "project.hpp"      // ASSERT


// static data

Player::Map Player::msMap;


// lifecycle

Player::Player(String const& rName):
mName(rName),
    mStartCellPosition(0, 0)
{
    mAutopause = false;
    mPeek = false;
    mShowClocks = false;
    mShowGrid = false;
    mShowScores = false;
    mTileSize = GameView::TILE_SIZE_DEFAULT;
}

// The implicitly defined destructor is OK.


// operators

Player::operator DisplayModes(void) const {
    return mDisplayModes;
}

Player::operator Point(void) const {
    return mStartCellPosition;
}


// misc methods

/* static */ Player& Player::rLookup(String const& rName) {
    ConstIterator const i_player = msMap.find(rName);

    Player* p_result = NULL;
    if (i_player != msMap.end()) {
        p_result = i_player->second;
    } else {
        p_result = new Player(rName);
        ASSERT(p_result != NULL);

        Pair const new_mapping(rName, p_result); 
        InsertResult const ins_result = msMap.insert(new_mapping);
        bool const success = ins_result.second;
        ASSERT(success);
    }

    ASSERT(p_result != NULL);
    return *p_result;
}

void Player::SetAutopause(bool value) {
    mAutopause = value;
}

void Player::SetDisplayModes(DisplayModes const& rDisplayModes) {
    mDisplayModes = rDisplayModes;
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

void Player::SetStartCellPosition(Point const& rPoint) {
    mStartCellPosition = rPoint;
}

void Player::SetTileSize(unsigned size) {
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    mTileSize = size;
}

unsigned Player::TileSize(void) const {
    return mTileSize;
}


// inquiry methods

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
