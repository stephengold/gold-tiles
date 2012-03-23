// File:    viewmenu.cpp
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

#include "gui/resource.hpp"
#include "gui/viewmenu.hpp"
#ifdef _WINDOWS
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT

ViewMenu::ViewMenu(void):
    SubMenu(tr("&View")),
    mSmallTiles(Qt(), "&Small Tiles"),
    mMediumTiles(Qt(), "&Medium Tiles"),
    mLargeTiles(Qt(), "&Large Tiles"),
    mRecenter(Qt(), "&Re-center\tC"),
    mAttributes(Qt(), "Tile &Attributes..."),
    mHints(Qt(), "&Hints..."),
    mShowClocks(Qt(), "Show &Clocks"),
    mShowGrid(Qt(), "Show &Grid"),
    mShowScores(Qt(), "Show Sc&ores"),
    mShowTiles(Qt(), "&Peek"),
    mAnimation(Qt(), "A&nimation")
{
    QKeySequence const c(tr("C"));
    mRecenter.SetShortcut(c);

    Add(mSmallTiles);
    Add(mMediumTiles);
    Add(mLargeTiles);
    AddSeparator();
    Add(mRecenter);
    Add(mAttributes);
    Add(mHints);
    AddSeparator();
    Add(mShowClocks);
    Add(mShowGrid);
    Add(mShowScores);
    Add(mShowTiles);
    Add(mAnimation);
}

#elif defined(_WINDOWS)

ViewMenu::ViewMenu(Menu const &rRootMenu, unsigned position):
	SubMenu(rRootMenu, position),
    mSmallTiles(rRootMenu, IDM_SMALL_TILES),
    mMediumTiles(rRootMenu, IDM_MEDIUM_TILES),
    mLargeTiles(rRootMenu, IDM_LARGE_TILES),
    mRecenter(rRootMenu, IDM_RECENTER),
    mAttributes(rRootMenu, IDM_ATTRIBUTES),
    mHints(rRootMenu, IDM_HINTS),
    mShowClocks(rRootMenu, IDM_SHOW_CLOCKS),
    mShowGrid(rRootMenu, IDM_SHOW_GRID),
    mShowScores(rRootMenu, IDM_SHOW_SCORES),
    mShowTiles(rRootMenu, IDM_PEEK),
    mAnimation(rRootMenu, IDM_ANIMATION)
{}

#endif // defined(_WINDOWS)

// The compiler-generated destructor is OK.


// misc methods

void ViewMenu::EnableItems(GameStyleType gameStyle, bool isOver, bool isLocalPlayer) {
    mSmallTiles.Enable(true);
    mMediumTiles.Enable(true);
    mLargeTiles.Enable(true);
    mRecenter.Enable(true);
    mAttributes.Enable(true);
    mHints.Enable(!isOver && isLocalPlayer);
    mShowClocks.Enable(true);
    mShowGrid.Enable(true);
    mShowScores.Enable(true);
    mShowTiles.Enable(isOver || gameStyle == GAME_STYLE_DEBUG);
    mAnimation.Enable(false); // TODO
}

void ViewMenu::ShowClocks(bool shown) {
	mShowClocks.Check(shown);
}
void ViewMenu::ShowGrid(bool shown) {
	mShowGrid.Check(shown);
}
void ViewMenu::ShowScores(bool shown) {
	mShowScores.Check(shown);
}
void ViewMenu::ShowTiles(bool shown) {
	mShowTiles.Check(shown);
}

#ifdef _WINDOWS

void ViewMenu::TileSize(IdType itemId) {
	UncheckAllSizes();
	
	switch (itemId) {
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
			FAIL();
	}
}
#endif // defined(_WINDOWS)

void ViewMenu::UncheckAllSizes(void) {
	mSmallTiles.Check(false);
    mMediumTiles.Check(false);
    mLargeTiles.Check(false);
}

