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

// lifecycle

PlayMenu::PlayMenu(HMENU menu, UINT position):	
	SubMenu(menu, position),
    mPlay(menu, IDM_PLAY_PLAY),
    mTakeBack(menu, IDM_TAKE_BACK),
    mHint(menu, IDM_HINT),
    mPause(menu, IDM_PAUSE),
    mPass(menu, IDM_PASS),
    mResign(menu, IDM_RESIGN),
    mRestart(menu, IDM_RESTART),
    mUndo(menu, IDM_UNDO),
    mRedo(menu, IDM_REDO),
    mAutopause(menu, IDM_AUTOPAUSE)
 {}

// misc methods

void PlayMenu::Autopause(bool active) {
	mAutopause.Check(active);
}

void PlayMenu::Pause(bool paused) {
	mPause.Check(paused);
}

void PlayMenu::EnableItems(bool paused, bool play) {
    mPlay.Enable(!paused && play);
    mTakeBack.Enable(!paused && play);
    mHint.Enable(false);
    mPause.Enable(true);
    mPass.Enable(!paused && !play);
    mResign.Enable(false);
    mRestart.Enable(false);
    mUndo.Enable(false);
    mRedo.Enable(false);
    mAutopause.Enable(!paused);
}

