// File:     viewmenu.cpp
// Location: src/gui
// Purpose:  implement ViewMenu class
// Author:   Stephen Gold sgold@sonic.net
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
#include "partial.hpp"
#ifdef _WINDOWS
# include "gui/win_types.hpp"
#endif  // defined(_WINDOWS)


// lifecycle

#ifdef _QT

ViewMenu::ViewMenu(void):
SubMenu(tr("&View")),
    mRecenter(Qt(), "&Re-center\tC"),
    mAttrs(Qt(), "Tile &Attributes..."),
    mHints(Qt(), "&Hints..."),
    mShowClocks(Qt(), "Show &Clocks"),
    mShowGrid(Qt(), "Show &Grid"),
    mShowScores(Qt(), "Show Sc&ores"),
    mShowTiles(Qt(), "&Peek"),
    mAnimation(Qt(), "A&nimation")
{
    QKeySequence const c(tr("C"));
    mRecenter.SetShortcut(c);

    Add(mRecenter);
    Add(mAttrs);
    Add(mHints);
    AddSeparator();
    Add(mShowClocks);
    Add(mShowGrid);
    Add(mShowScores);
    Add(mShowTiles);
    Add(mAnimation);
}

#elif defined(_WINDOWS)

ViewMenu::ViewMenu(Menu const& rRootMenu, unsigned position):
SubMenu(rRootMenu, position),
    mBoardSize(rRootMenu, 0),
    mTileSize(rRootMenu, 1),
    mRecenter(rRootMenu, IDM_RECENTER),
    mAttrs(rRootMenu, IDM_ATTRIBUTES),
    mHints(rRootMenu, IDM_HINTS),
    mShowClocks(rRootMenu, IDM_SHOW_CLOCKS),
    mShowGrid(rRootMenu, IDM_SHOW_GRID),
    mShowScores(rRootMenu, IDM_SHOW_SCORES),
    mShowTiles(rRootMenu, IDM_PEEK),
    mAnimation(rRootMenu, IDM_ANIMATION)
{}

#endif  // defined(_WINDOWS)

// The implicitly defined destructor is OK.


// misc methods

void ViewMenu::BoardTileSize(TileSizeType size) {
    mBoardSize.SetSize(size);
}

void ViewMenu::EnableItems(Partial const& rPartial, ThinkModeType thinkMode) {
    bool const is_over = rPartial.IsGameOver();
    bool const is_local = rPartial.IsLocalUsersTurn() && (thinkMode != THINK_SUGGEST);
    GameStyleType const game_style = rPartial.GameStyle();

    mBoardSize.Enable(true);
    mTileSize.Enable(true);
    mRecenter.Enable(true);
    mAttrs.Enable(true);
    mHints.Enable(!is_over && is_local);
    mShowClocks.Enable(true);
    mShowGrid.Enable(true);
    mShowScores.Enable(true);
    mShowTiles.Enable(is_over || game_style == GAME_STYLE_DEBUG);
    mAnimation.Enable(false); // TODO
}

void ViewMenu::HandTileSize(TileSizeType size) {
    mTileSize.SetSize(size);
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

