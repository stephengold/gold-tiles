#ifndef VIEWMENU_HPP_INCLUDED
#define VIEWMENU_HPP_INCLUDED

// File:    viewmenu.hpp
// Purpose: ViewMenu class
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
A ViewMenu object represents the "view" SubMenu in the menu bar.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/menuitem.hpp"
#include "gui/submenu.hpp"

class ViewMenu: public SubMenu {
public:
	// lifecycle
	ViewMenu(void) { ASSERT(false); }
    ViewMenu(HMENU menu, UINT position);

	// misc
    void TileSize(UINT);

	// access
    void Animation(bool);
	void ShowClocks(bool);
	void ShowGrid(bool);
	void ShowHints(bool);
	void ShowScores(bool);
	void ShowTiles(bool);

private:
    MenuItem mRecenter, mAnimation, mAttributes;
    MenuItem mShowClocks, mShowGrid, mShowHints, mShowScores, mShowTiles;
    MenuItem mSmallTiles, mMediumTiles, mLargeTiles;

	// misc
	void UncheckAllSizes(void);
};

#endif
#endif
