#ifndef PLAYMENU_HPP_INCLUDED
#define PLAYMENU_HPP_INCLUDED

// File:    playmenu.hpp
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

/*
A PlayMenu object represents the "play" submenu in the menu bar.
*/

#include "game.hpp"
#include "gui/menuitem.hpp"
#include "gui/submenu.hpp"
#include "gui/win.hpp"

class PlayMenu: public SubMenu {
public:
	// lifecycle
    PlayMenu(Menu const &, unsigned position);
	// no default constructor
	// ~PlayMenu(void);  compiler-generated destructor is fine

	// public operators
	PlayMenu &operator=(PlayMenu const &) { FAIL(); }

	// misc public methods
	void Autopause(bool);
	void EnableItems(GameStyleType, bool over, bool paused, bool pass, bool swapAll);
	void Pause(bool);

public:
    MenuItem mPlay, mTakeBack, mHint, mPause, mSwapAll, mPass, mResign;
    MenuItem mRestart, mUndo, mRedo, mAutopause;
};
#endif

