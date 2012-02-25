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

#include "game.hpp"           // USES GameStyleType
#include "gui/menuitem.hpp"   // HASA MenuItem
#include "gui/submenu.hpp"    // ISA SubMenu

class ViewMenu: public SubMenu {
public:
	// public lifecycle
    ViewMenu(Menu const &, unsigned position);
	// no default constructor
	//~ViewMenu(void);  compiler-generated destructor is OK

	// misc public methods
    void Animation(bool);
    void Autocenter(bool);
	void EnableItems(GameStyleType, bool over);
	void ShowClocks(bool);
	void ShowGrid(bool);
	void ShowHints(bool);
	void ShowScores(bool);
	void ShowTiles(bool);
    void TileSize(IdType);

private:
    MenuItem mSmallTiles, mMediumTiles, mLargeTiles;
    MenuItem mRecenter, mAnimation, mAttributes, mHints;
    MenuItem mShowClocks, mShowGrid, mShowScores, mShowTiles;
    MenuItem mAutocenter;

	// private lifecycle
	ViewMenu (ViewMenu const &); // not copyable

	// private operators
	ViewMenu &operator=(ViewMenu const &); // not assignable

	// misc private methods
	void UncheckAllSizes(void);
};
#endif
