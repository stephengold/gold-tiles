// File:     menubar.cpp
// Location: src/gui
// Purpose:  implement MenuBar class
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

#include "gui/gameview.hpp"
#include "gui/menubar.hpp"
#include "gui/player.hpp"
#ifdef _WINDOWS
# include "gui/resource.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT

MenuBar::MenuBar(Partial const& rPartial):
   mrPartial(rPartial),
    mThinking(tr("&Thinking")),
    mHelpMenu(tr("&Help"))
{
    addMenu(mFileMenu.Qt());
    addMenu(mPlayMenu.Qt());
    addMenu(mViewMenu.Qt());
    addMenu(mThinking.Qt());

    MenuItem rules(mHelpMenu.Qt(), tr("&Rules"));
    mHelpMenu.Add(rules);
    addMenu(mHelpMenu.Qt());

    GameStyleType game_style = mrPartial.GameStyle();
    Initialize(game_style);
}

#elif defined(_WINDOWS)

MenuBar::MenuBar(CREATESTRUCT const& rCreateStruct, Partial const& rPartial):
mrPartial(rPartial),
    mMenu(rCreateStruct.hMenu),
    mFileMenu(mMenu, 1),
    mPlayMenu(mMenu, 2),
    mViewMenu(mMenu, 3),
    mThinking(mMenu, 4),
    mHelpMenu(mMenu, 5)
{
    GameStyleType game_style = mrPartial.GameStyle();
    Initialize(game_style);
}

#endif // defined(_WINDOWS)

void MenuBar::Initialize(GameStyleType gameStyle) {
    bool const is_challenge = (gameStyle == GAME_STYLE_CHALLENGE);
    bool const is_debug = (gameStyle == GAME_STYLE_DEBUG);

    mAutopauseFlag = is_challenge;
    mShowClocksFlag = is_challenge || is_debug;
    mShowGridFlag = is_debug;
    mShowScoresFlag = true;
    mPeekFlag = is_debug;
    mTileSize = GameView::TILE_SIZE_DEFAULT;
}

// The implicitly defined destructor is OK.

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

void MenuBar::LoadPlayerOptions(Player const& rPlayer) {
    mAutopauseFlag = rPlayer.Autopause();
    mPeekFlag = rPlayer.Peek();
    mShowClocksFlag = rPlayer.ShowClocks();
    mShowGridFlag = rPlayer.ShowGrid();
    mShowScoresFlag = rPlayer.ShowScores();
    mTileSize = rPlayer.TileSize();
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

void MenuBar::SavePlayerOptions(Player& rPlayer) const {
    rPlayer.SetAutopause(mAutopauseFlag);
    rPlayer.SetPeek(mPeekFlag);
    rPlayer.SetShowClocks(mShowClocksFlag);
    rPlayer.SetShowGrid(mShowGridFlag);
    rPlayer.SetShowScores(mShowScoresFlag);
    rPlayer.SetTileSize(mTileSize);
}

void MenuBar::SetTileSize(unsigned size) {
    mTileSize = size;
}

unsigned MenuBar::TileSize(void) const {
    return mTileSize;
}

void MenuBar::Update(ThinkModeType thinkMode) {
    bool const have_game = mrPartial.HasGame();
    bool const is_local = mrPartial.IsLocalUsersTurn() && (thinkMode != THINK_SUGGEST);
    bool const is_over = mrPartial.IsGameOver();
    bool const is_paused = mrPartial.IsGamePaused();
    bool const can_redo = mrPartial.CanRedo();

    // "File" menu
    mFileMenu.EnableItems(have_game);
    mFileMenu.Enable(is_local || is_over || can_redo || !have_game);

    // "Play" menu
    mPlayMenu.Autopause(mAutopauseFlag);
    mPlayMenu.Pause(is_paused);

    mPlayMenu.EnableItems(mrPartial, thinkMode);
    mPlayMenu.Enable(have_game && (is_local || is_over || can_redo));

    // "View" menu
    mViewMenu.TileSize(mTileSize);
    mViewMenu.ShowClocks(mShowClocksFlag);
    mViewMenu.ShowGrid(mShowGridFlag);
    mViewMenu.ShowScores(mShowScoresFlag);
    mViewMenu.ShowTiles(mPeekFlag);

    mViewMenu.EnableItems(mrPartial, thinkMode);
    mViewMenu.Enable(!is_paused);

    // "Thinking" menu
    mThinking.Enable(thinkMode != THINK_IDLE || thinkMode == THINK_CANCEL);

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
