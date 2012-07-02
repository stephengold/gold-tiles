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
#include "gui/user.hpp"
#ifdef _WINDOWS
# include "gui/resource.hpp"
# include "gui/win_types.hpp"
#endif  // defined(_WINDOWS)


// lifecycle

#ifdef _QT

MenuBar::MenuBar(Partial const& rPartial, QObject* pParent)
:   mHelpMenu(tr("&Help")),
   mrPartial(rPartial),
    mThinking(tr("&Thinking"))
{
    addMenu(mFileMenu.Qt());
    addMenu(mPlayMenu.Qt());
    addMenu(mViewMenu.Qt());

    MenuItem* p_cancel = new MenuItem(mViewMenu.Qt(), tr("&Cancel\tCtrl+C"));
    mThinking.Add(*p_cancel);
    addMenu(mThinking.Qt());

    MenuItem* p_rules = new MenuItem(mHelpMenu.Qt(), tr("&Rules...\tF1"));
    connect(p_rules->pAction(), SIGNAL(triggered()),
        pParent, SLOT(HandleMenuCommand(void)));
    p_rules->SetShortcut(tr("F1"));
    mHelpMenu.Add(*p_rules);

    MenuItem* p_warranty = new MenuItem(mHelpMenu.Qt(), tr("&Warranty..."));
    mHelpMenu.Add(*p_warranty);

    MenuItem* p_about = new MenuItem(mHelpMenu.Qt(), tr("&About Gold Tile..."));
    addMenu(mHelpMenu.Qt());
    mHelpMenu.Add(*p_about);

    GameStyleType const game_style = mrPartial.GameStyle();
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

#endif  // defined(_WINDOWS)

void MenuBar::Initialize(GameStyleType gameStyle) {
    bool const is_challenge = (gameStyle == GAME_STYLE_CHALLENGE);
    bool const is_debug = (gameStyle == GAME_STYLE_DEBUG);

    mAutopauseFlag = is_challenge;
    mBoardTileSize = GameView::TILE_SIZE_DEFAULT;
    mHandTileSize = GameView::TILE_SIZE_DEFAULT;
    mShowClocksFlag = is_challenge || is_debug;
    mShowGridFlag = true;
    mShowScoresFlag = true;
    mPeekFlag = is_debug;
}

// The implicitly defined destructor is OK.

// misc methods

TileSizeType MenuBar::BoardTileSize(void) const {
    return mBoardTileSize;
}

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
#endif  // defined(_WINDOWS)

TileSizeType MenuBar::HandTileSize(void) const {
    return mHandTileSize;
}

void MenuBar::LoadUserOptions(User const& rUser) {
    mAutopauseFlag = rUser.Autopause();
    mBoardTileSize = rUser.BoardTileSize();
    mHandTileSize = rUser.HandTileSize();
    mPeekFlag = rUser.Peek();
    mShowClocksFlag = rUser.ShowClocks();
    mShowGridFlag = rUser.ShowGrid();
    mShowScoresFlag = rUser.ShowScores();
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

void MenuBar::SaveUserOptions(User& rUser) const {
    rUser.SetAutopause(mAutopauseFlag);
    rUser.SetBoardTileSize(mBoardTileSize);
    rUser.SetHandTileSize(mHandTileSize);
    rUser.SetPeek(mPeekFlag);
    rUser.SetShowClocks(mShowClocksFlag);
    rUser.SetShowGrid(mShowGridFlag);
    rUser.SetShowScores(mShowScoresFlag);
}

void MenuBar::SetBoardTileSize(TileSizeType size) {
    mBoardTileSize = size;
}

void MenuBar::SetHandTileSize(TileSizeType size) {
    mHandTileSize = size;
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
    mViewMenu.BoardTileSize(mBoardTileSize);
    mViewMenu.HandTileSize(mHandTileSize);
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
