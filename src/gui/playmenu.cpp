// File:    playmenu.cpp
// Purpose: implement the PlayMenu class.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
