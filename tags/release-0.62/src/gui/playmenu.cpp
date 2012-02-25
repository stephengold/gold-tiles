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
#include "gui/resource.hpp"
#include "gui/win_types.hpp"

// lifecycle

PlayMenu::PlayMenu(Menu const &rRootMenu, unsigned position):	
	SubMenu(rRootMenu, position),
    mPlay(rRootMenu, IDM_PLAY_PLAY),
    mTakeBack(rRootMenu, IDM_TAKE_BACK),
    mHint(rRootMenu, IDM_HINT),
    mPause(rRootMenu, IDM_PAUSE),
    mSwapAll(rRootMenu, IDM_SWAP_ALL),
    mPass(rRootMenu, IDM_PASS),
    mResign(rRootMenu, IDM_RESIGN),
    mRestart(rRootMenu, IDM_RESTART),
    mUndo(rRootMenu, IDM_UNDO),
    mRedo(rRootMenu, IDM_REDO),
    mAutopause(rRootMenu, IDM_AUTOPAUSE)
 {}

// misc methods

void PlayMenu::Autopause(bool active) {
	mAutopause.Check(active);
}

void PlayMenu::Pause(bool paused) {
	mPause.Check(paused);
}

void PlayMenu::EnableItems(
	GameStyleType gameStyle,
	bool isOver,
	bool isPaused,
	bool isPass,
	bool canSwapAll)
{
    mPlay.Enable(!isOver && !isPaused && !isPass);
    mTakeBack.Enable(!isOver && !isPaused && !isPass);
    mHint.Enable(false);    // TODO
    mPause.Enable(!isOver);
	mSwapAll.Enable(!isOver && !isPaused && isPass && canSwapAll);
    mPass.Enable(!isOver && !isPaused && isPass);
    mResign.Enable(false);  // TODO
    mRestart.Enable(false); // TODO
    mUndo.Enable(false);    // TODO
    mRedo.Enable(false);    // TODO
    mAutopause.Enable(gameStyle != GAME_STYLE_CHALLENGE && !isOver && !isPaused);
}

