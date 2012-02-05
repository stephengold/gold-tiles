#ifndef VIEWMENU_HPP_INCLUDED
#define VIEWMENU_HPP_INCLUDED

// File:    viewmenu.hpp
// Purpose: ViewMenu class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
