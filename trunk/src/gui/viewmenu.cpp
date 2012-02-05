// File:    viewmenu.cpp
// Purpose: implement the ViewMenu class.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/viewmenu.hpp"

ViewMenu::ViewMenu(HMENU menu, UINT position):
	SubMenu(menu, position),
    mSmallTiles(menu, IDM_SMALL_TILES),
    mMediumTiles(menu, IDM_MEDIUM_TILES),
    mLargeTiles(menu, IDM_LARGE_TILES),
    mRecenter(menu, IDM_RECENTER),
    mAttributes(menu, IDM_ATTRIBUTES),
    mShowClocks(menu, IDM_SHOW_CLOCKS),
    mShowGrid(menu, IDM_SHOW_GRID),
    mShowHints(menu, IDM_SHOW_HINTS),
    mShowScores(menu, IDM_SHOW_SCORES),
    mShowTiles(menu, IDM_SHOW_TILES),
    mAnimation(menu, IDM_ANIMATION)
{}

void ViewMenu::ShowClocks(bool shown) {
	mShowClocks.Check(shown);
}
void ViewMenu::ShowGrid(bool shown) {
	mShowGrid.Check(shown);
}
void ViewMenu::ShowHints(bool shown) {
	mShowHints.Check(shown);
}
void ViewMenu::ShowScores(bool shown) {
	mShowScores.Check(shown);
}
void ViewMenu::ShowTiles(bool shown) {
	mShowTiles.Check(shown);
}
void ViewMenu::TileSize(UINT position) {
	UncheckAllSizes();
	switch (position) {
 	    case IDM_SMALL_TILES:
	        mSmallTiles.Check(true);
			break;
		case IDM_MEDIUM_TILES:
            mMediumTiles.Check(true);
			break;
		case IDM_LARGE_TILES:
        	mLargeTiles.Check(true);
			break;
		default:
			assert(false);
	}
}
void ViewMenu::UncheckAllSizes(void) {
	mSmallTiles.Check(false);
    mMediumTiles.Check(false);
    mLargeTiles.Check(false);
}
#endif
