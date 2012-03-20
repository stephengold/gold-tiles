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

#include "partial.hpp"      // order-dependent #include
#include "gui/menubar.hpp"
#include "gui/player.hpp"
#ifdef _WINDOWS
# include "gui/resource.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT

MenuBar::MenuBar(Partial const &rPartial):
    mrPartial(rPartial),
    mHelpMenu(tr("&Help"))
{
    addMenu(mFileMenu.Qt());
    addMenu(mPlayMenu.Qt());
    addMenu(mViewMenu.Qt());

    MenuItem rules(mHelpMenu.Qt(), tr("&Rules"));
    mHelpMenu.Add(rules);
    addMenu(mHelpMenu.Qt());

    GameStyleType game_style = mrPartial.GameStyle();
	Initialize(game_style);
}

#elif defined(_WINDOWS)

MenuBar::MenuBar(CREATESTRUCT const &rCreateStruct, Partial const &rPartial):
    mrPartial(rPartial),
    mMenu(rCreateStruct.hMenu),
    mFileMenu(mMenu, 0),
    mPlayMenu(mMenu, 1),
    mViewMenu(mMenu, 2),
    mHelpMenu(mMenu, 3)
{
    GameStyleType game_style = mrPartial.GameStyle();
	Initialize(game_style);
}

#endif // defined(_WINDOWS)

void MenuBar::Initialize(GameStyleType game_style) {
	bool is_challenge = (game_style == GAME_STYLE_CHALLENGE);
	bool is_debug = (game_style == GAME_STYLE_DEBUG);

	mAutopauseFlag = is_challenge;
	mShowClocksFlag = is_challenge || is_debug;
   	mShowGridFlag = is_debug;
	mShowScoresFlag = is_debug;
	mPeekFlag = is_debug;
    mTileSizeItem = IDM_LARGE_TILES;
}

// The compiler-generated destructor is OK.

// misc methods

void MenuBar::GameOver(void) {
    mPeekFlag = true;
    mShowClocksFlag = true;
    mShowScoresFlag = true;
}

#ifdef _WINDOWS
void MenuBar::HandleMenuCommand(IdType command) {
	switch (command) {
		case IDM_AUTOPAUSE:
            mAutopauseFlag = !mAutopauseFlag;
            break;
        case IDM_SHOW_CLOCKS:
            mShowClocksFlag = !mShowClocksFlag;
            break;
        case IDM_SHOW_GRID:
            mShowGridFlag = !mShowGridFlag;
            break;
        case IDM_SHOW_SCORES:
			mShowScoresFlag = !mShowScoresFlag;
            break;
        case IDM_PEEK:
			ASSERT(mrPartial.IsGameOver() || mrPartial.GameStyle() == GAME_STYLE_DEBUG);
            mPeekFlag = !mPeekFlag;
            break;
		default:
			FAIL();
	}
}
#endif // defined(_WINDOWS)

void MenuBar::LoadPlayerOptions(Player const &rPlayer) {
	mAutopauseFlag = rPlayer.Autopause();
    mPeekFlag = rPlayer.Peek();
	mShowClocksFlag = rPlayer.ShowClocks();
    mShowGridFlag = rPlayer.ShowGrid();
	mShowScoresFlag = rPlayer.ShowScores();
	mTileSizeItem = rPlayer.TileSize();
}

void MenuBar::NewGame(GameStyleType oldStyle) {
    GameStyleType new_style = mrPartial.GameStyle();
    
	if (oldStyle != new_style) {
		Initialize(new_style);
	}

	if (new_style != GAME_STYLE_DEBUG) {
		mPeekFlag = false;
	}
}

void MenuBar::SavePlayerOptions(Player &rPlayer) const {
	rPlayer.SetAutopause(mAutopauseFlag);
    rPlayer.SetPeek(mPeekFlag);
	rPlayer.SetShowClocks(mShowClocksFlag);
    rPlayer.SetShowGrid(mShowGridFlag);
	rPlayer.SetShowScores(mShowScoresFlag);
	rPlayer.SetTileSize(mTileSizeItem);
}

void MenuBar::SetTileSize(IdType menuItem) {
	mTileSizeItem = menuItem;
}

IdType MenuBar::TileSize(void) const {
	return mTileSizeItem;
}

void MenuBar::Update(void) {
	bool have_game = mrPartial.HasGame();
	bool is_over = mrPartial.IsGameOver();
	bool is_local = mrPartial.IsLocalPlayer();
    bool is_pass = mrPartial.IsPass();
	bool is_paused = mrPartial.IsGamePaused();
	bool can_swap_all = mrPartial.CanSwapAll();
	bool can_undo = mrPartial.CanUndo();
	bool can_redo = mrPartial.CanRedo();
    GameStyleType game_style = mrPartial.GameStyle();

	// "File" menu
	mFileMenu.EnableItems(have_game);
	mFileMenu.Enable(is_local || is_over || can_redo || !have_game);

	// "Play" menu
    mPlayMenu.Autopause(mAutopauseFlag);
    mPlayMenu.Pause(is_paused);
    
	mPlayMenu.EnableItems(game_style, is_over, is_paused, is_pass, 
		  can_swap_all, can_undo, can_redo, is_local);
	mPlayMenu.Enable(have_game && (is_local || is_over || can_redo));

	// "View" menu
#ifdef _WINDOWS
    mViewMenu.TileSize(mTileSizeItem);
#endif // defined(_WINDOWS)
    mViewMenu.ShowClocks(mShowClocksFlag);
    mViewMenu.ShowGrid(mShowGridFlag);
    mViewMenu.ShowScores(mShowScoresFlag);
    mViewMenu.ShowTiles(mPeekFlag);
    
	mViewMenu.EnableItems(game_style, is_over, is_local);
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

bool MenuBar::IsAutopause(void) const {
    return mAutopauseFlag;
}
    
bool MenuBar::IsGridVisible(void) const {
    return mShowGridFlag;
}

bool MenuBar::IsPeeking(void) const {
    return mPeekFlag;
}
