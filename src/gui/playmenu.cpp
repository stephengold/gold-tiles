// File:    playmenu.cpp
// Purpose: PlayMenu class
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

#include "gui/playmenu.hpp"
#include "partial.hpp"
#ifdef _WINDOWS
# include "gui/resource.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)

// lifecycle

#ifdef _QT

PlayMenu::PlayMenu(void):
    SubMenu(tr("&Play")),
    mPlay(Qt(), "&Play\tEnter"),
    mTakeBack(Qt(), "Take &back\tEsc"),
    mSuggest(Qt(), "Su&ggest\tG"),
    mPause(Qt(), "Pause"),
    mSwapAll(Qt(), "S&wap All"),
    mPass(Qt(), "Pa&ss"),
    mResign(Qt(), "&Resign"),
    mRestart(Qt(), "R&estart"),
    mUndo(Qt(), "&Undo\tZ"),
    mRedo(Qt(), "Re&do\tY"),
    mAutopause(Qt(), "&AutoPause")
{
    mPlay.SetShortcut(tr("Enter"));
    mTakeBack.SetShortcut(tr("Enter"));
    mSuggest.SetShortcut(tr("G"));
    mUndo.SetShortcut(tr("Z"));
    mRedo.SetShortcut(tr("Y"));

    Add(mPlay);
    Add(mTakeBack);
    Add(mSuggest);
    Add(mPause);
    Add(mSwapAll);
    Add(mPass);
    Add(mResign);
    AddSeparator();
    Add(mRestart);
    Add(mUndo);
    Add(mRedo);
    AddSeparator();
    Add(mAutopause);
}

#elif defined(_WINDOWS)

PlayMenu::PlayMenu(Menu const &rRootMenu, unsigned position):	
	SubMenu(rRootMenu, position),
    mPlay(rRootMenu, IDM_PLAY_PLAY),
    mTakeBack(rRootMenu, IDM_TAKE_BACK),
    mSuggest(rRootMenu, IDM_SUGGEST),
    mPause(rRootMenu, IDM_PAUSE),
    mSwapAll(rRootMenu, IDM_SWAP_ALL),
    mPass(rRootMenu, IDM_PASS),
    mResign(rRootMenu, IDM_RESIGN),
    mRestart(rRootMenu, IDM_RESTART),
    mUndo(rRootMenu, IDM_UNDO),
    mRedo(rRootMenu, IDM_REDO),
    mAutopause(rRootMenu, IDM_AUTOPAUSE)
 {}

#endif // defined(_WINDOWS)

// The compiler-generated destructor is OK.


// misc methods

void PlayMenu::Autopause(bool active) {
	mAutopause.Check(active);
}

void PlayMenu::Pause(bool paused) {
	mPause.Check(paused);
}

void PlayMenu::EnableItems(Partial const &rPartial, ThinkModeType thinkMode) {
	bool const is_local = rPartial.IsLocalUsersTurn() && (thinkMode != THINK_SUGGEST);
	bool const is_over = rPartial.IsGameOver();
    bool const is_pass = rPartial.IsPass();
	bool const is_paused = rPartial.IsGamePaused();
	bool const can_swap_all = rPartial.CanSwapAll();
	bool const can_redo = rPartial.CanRedo();
	bool const can_undo = rPartial.CanUndo();
    GameStyleType const game_style = rPartial.GameStyle();

	bool const is_challenge = (game_style == GAME_STYLE_CHALLENGE);
	bool const is_friendly = (game_style == GAME_STYLE_FRIENDLY);

    mPlay.Enable(is_local && !is_over && !is_paused && !is_pass);
    mTakeBack.Enable(is_local && !is_over && !is_paused && !is_pass);
    mSuggest.Enable(is_local && !is_over && !is_paused && !is_challenge);
    mPause.Enable(!is_over);
	mSwapAll.Enable(is_local && !is_over && !is_paused && is_pass && can_swap_all);
    mPass.Enable(is_local && !is_over && !is_paused && is_pass);
    mResign.Enable(is_local && !is_over && !is_paused);
    mRestart.Enable(!is_paused && !is_challenge && !is_friendly);
    mUndo.Enable(can_undo && !is_paused && !is_challenge && !is_friendly);
    mRedo.Enable(can_redo && !is_paused && !is_challenge && !is_friendly);
    mAutopause.Enable(is_local && !is_over && !is_paused && !is_challenge);
}
