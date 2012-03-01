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

#include "game.hpp"          // USES GameStyleType
#include "gui/menuitem.hpp"  // HASA MenuItem
#include "gui/submenu.hpp"   // ISA SubMenu

class PlayMenu: public SubMenu {
public:
	// public lifecycle
    PlayMenu(Menu const &, unsigned position);
	// no default constructor
	// ~PlayMenu(void);  compiler-generated destructor is fine

	// misc public methods
	void Autopause(bool);
	void EnableItems(GameStyleType, bool over, bool paused, bool pass, bool swapAll);
	void Pause(bool);

private:
	// private data
    MenuItem mPlay, mTakeBack, mSuggest, mPause, mSwapAll, mPass, mResign;
    MenuItem mRestart, mUndo, mRedo, mAutopause;

	// private lifecycle
	PlayMenu(PlayMenu const &); // not copyable

	// private operators
	PlayMenu &operator=(PlayMenu const &); // not assignable
};
#endif

