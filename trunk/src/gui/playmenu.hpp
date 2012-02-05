#ifndef PLAYMENU_HPP_INCLUDED
#define PLAYMENU_HPP_INCLUDED

// File:    playmenu.hpp
// Purpose: PlayMenu for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A PlayMenu object represents the "play" submenu in the menu bar.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/menuitem.hpp"
#include "gui/submenu.hpp"

class PlayMenu: public SubMenu {
public:
	// lifecycle
	PlayMenu(void) { ASSERT(false); }
    PlayMenu(HMENU menu, UINT position);

	// misc
	void Autopause(bool);
	void EnableItems(bool paused, bool play);
	void Pause(bool);

public:
    MenuItem mPause, mAccept, mPlay, mPass, mResign;
    MenuItem mRestart, mUndo, mRedo, mAutopause;
};

#endif
#endif
