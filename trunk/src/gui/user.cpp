// File:     user.cpp
// Location: src/gui
// Purpose:  implement User class
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

#include "gui/gameview.hpp"  // TILE_SIZE_DEFAULT
#include "gui/user.hpp"
#include "project.hpp"       // ASSERT


// static data

User::Map User::msMap;


// lifecycle

User::User(String const& rName):
mName(rName),
    mStartCellPosition(0, 0)
{
    mAutopause = false;
    mBoardTileSize = GameView::TILE_SIZE_DEFAULT;
    mHandTileSize = GameView::TILE_SIZE_DEFAULT;
    mPeek = false;
    mShowClocks = false;
    mShowGrid = false;
    mShowScores = false;
}

// The implicitly defined destructor is OK.


// operators

User::operator DisplayModes(void) const {
    return mDisplayModes;
}

User::operator Point(void) const {
    return mStartCellPosition;
}


// misc methods

TileSizeType User::BoardTileSize(void) const {
    return mBoardTileSize;
}

TileSizeType User::HandTileSize(void) const {
    return mHandTileSize;
}

/* static */  User& User::rLookup(String const& rName) {
    ConstIterator const i_user = msMap.find(rName);

    User* p_result = NULL;
    if (i_user != msMap.end()) {
        p_result = i_user->second;
    } else {
        p_result = new User(rName);
        ASSERT(p_result != NULL);

        Pair const new_mapping(rName, p_result); 
        InsertResult const ins_result = msMap.insert(new_mapping);
        bool const success = ins_result.second;
        ASSERT(success);
    }

    ASSERT(p_result != NULL);
    return *p_result;
}

void User::SetAutopause(bool value) {
    mAutopause = value;
}

void User::SetBoardTileSize(TileSizeType size) {
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    mBoardTileSize = size;
}

void User::SetDisplayModes(DisplayModes const& rDisplayModes) {
    mDisplayModes = rDisplayModes;
}

void User::SetHandTileSize(TileSizeType size) {
    ASSERT(size >= GameView::TILE_SIZE_MIN);
    ASSERT(size <= GameView::TILE_SIZE_MAX);

    mHandTileSize = size;
}

void User::SetPeek(bool value) {
    mPeek = value;
}

void User::SetShowClocks(bool value) {
    mShowClocks = value;
}

void User::SetShowGrid(bool value) {
    mShowGrid = value;
}

void User::SetShowScores(bool value) {
    mShowScores = value;
}

void User::SetStartCellPosition(Point const& rPoint) {
    mStartCellPosition = rPoint;
}


// inquiry methods

bool User::Autopause(void) const {
    return mAutopause;
}

bool User::Peek(void) const {
    return mPeek;
}

bool User::ShowClocks(void) const {
    return mShowClocks;
}

bool User::ShowGrid(void) const {
    return mShowGrid;
}

bool User::ShowScores(void) const {
    return mShowScores;
}
