// File:    menubar.cpp
// Purpose: MenuBar class
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

#include "gui/menubar.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"
#include "partial.hpp"

// lifecycle

MenuBar::MenuBar(CREATESTRUCT const &rCreateStruct, Partial const &rPartial)
:
    mrPartial(rPartial),
    Menu(rCreateStruct.hMenu),
    mFileMenu(*this, 0),
    mPlayMenu(*this, 1),
    mViewMenu(*this, 2),
    mHelpMenu(*this, 3)
{
    GameStyleType game_style = mrPartial.GameStyle();
    mAutocenterFlag = (game_style == GAME_STYLE_CHALLENGE);
    mAutopauseFlag = (game_style == GAME_STYLE_CHALLENGE);
	mShowClocksFlag = (game_style == GAME_STYLE_CHALLENGE);
   	mShowGridFlag = false;
	mShowScoresFlag = true;
	mShowTilesFlag = false;
    mTileSizeItem = IDM_LARGE_TILES;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.

// misc methods

void MenuBar::GameOver(void) {
    mShowClocksFlag = true;
    mShowScoresFlag = true;
    mShowTilesFlag = true;
}

void MenuBar::NewGame(void) {
    GameStyleType game_style = mrPartial.GameStyle();
    
	if (game_style == GAME_STYLE_CHALLENGE) {
        mAutocenterFlag = true;
        mAutopauseFlag = true;
	    mShowClocksFlag = true;
	} else {
        mAutocenterFlag = false;
        mAutopauseFlag = false;
	    mShowClocksFlag = false;
	}
}

void MenuBar::SetTileSize(IdType menuItem) {
    mTileSizeItem = menuItem;
}

void MenuBar::ToggleAutocenter(void) {
    mAutocenterFlag = !mAutocenterFlag;
}

void MenuBar::ToggleAutopause(void) {
    mAutopauseFlag = !mAutopauseFlag;
}

void MenuBar::ToggleClocks(void) {
    mShowClocksFlag = !mShowClocksFlag;
}

void MenuBar::ToggleGrid(void) {
    mShowGridFlag = !mShowGridFlag;
}

void MenuBar::TogglePeeking(void) {
    mShowTilesFlag = !mShowTilesFlag;
}

void MenuBar::ToggleScores(void) {
    mShowScoresFlag = !mShowScoresFlag;
}

void MenuBar::Update(void) {
	bool have_game = mrPartial.HaveGame();
	bool is_over = mrPartial.IsGameOver();
    bool is_pass = mrPartial.IsPass();
	bool is_paused = mrPartial.IsGamePaused();
    GameStyleType game_style = mrPartial.GameStyle();

	// "File" menu
	mFileMenu.EnableItems(have_game);
	mFileMenu.Enable(true);

	// "Play" menu
    mPlayMenu.Autopause(mAutopauseFlag);
    mPlayMenu.Pause(is_paused);
    
	mPlayMenu.EnableItems(game_style, is_over, is_paused, is_pass);
	mPlayMenu.Enable(have_game);

	// "View" menu
    mViewMenu.TileSize(mTileSizeItem);
    mViewMenu.ShowClocks(mShowClocksFlag);
    mViewMenu.ShowGrid(mShowGridFlag);
    mViewMenu.ShowScores(mShowScoresFlag);
    mViewMenu.ShowTiles(mShowTilesFlag);
    mViewMenu.Autocenter(mAutocenterFlag);
    
	mViewMenu.EnableItems(game_style, is_over, is_paused, is_pass);
	mViewMenu.Enable(!is_paused);
	
	mHelpMenu.Enable(true);
}

// inquiry methods

bool MenuBar::AreClocksVisible(void) const {
    return mShowClocksFlag;
}
    
bool MenuBar::AreScoresVisible(void) const {
    return mShowScoresFlag;
}

bool MenuBar::IsAutocenter(void) const {
    return mAutocenterFlag;
}

bool MenuBar::IsAutopause(void) const {
    return mAutopauseFlag;
}
    
bool MenuBar::IsGridVisible(void) const {
    return mShowGridFlag;
}

bool MenuBar::IsPeeking(void) const {
    return mShowTilesFlag;
}


