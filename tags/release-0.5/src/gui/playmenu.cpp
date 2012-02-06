// File:    playmenu.cpp
// Purpose: PlayMenu class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/playmenu.hpp"

PlayMenu::PlayMenu(HMENU menu, UINT position):	
	SubMenu(menu, position),
    mPause(menu, IDM_PAUSE),
    mAccept(menu, IDM_ACCEPT),
    mPlay(menu, IDM_PLAY_PLAY),
    mPass(menu, IDM_PASS),
    mResign(menu, IDM_RESIGN),
    mRestart(menu, IDM_RESTART),
    mUndo(menu, IDM_UNDO),
    mRedo(menu, IDM_REDO),
    mAutopause(menu, IDM_AUTOPAUSE)
 {}

void PlayMenu::Autopause(bool active) {
	mAutopause.Check(active);
}
void PlayMenu::Pause(bool paused) {
	mPause.Check(paused);
}
void PlayMenu::EnableItems(bool paused, bool play) {
    mPause.Enable(true);
    mAccept.Enable(false);
    mPlay.Enable(!paused && play);
    mPass.Enable(!paused && !play);
    mResign.Enable(false);
    mRestart.Enable(false);
    mUndo.Enable(false);
    mRedo.Enable(false);
    mAutopause.Enable(!paused);
}

#endif
