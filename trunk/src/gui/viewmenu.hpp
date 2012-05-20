#ifndef VIEWMENU_HPP_INCLUDED
#define VIEWMENU_HPP_INCLUDED

// File:     viewmenu.hpp
// Location: src/gui
// Purpose:  declare ViewMenu class
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

/*
A ViewMenu object represents the "view" SubMenu in the menu bar.

The ViewMenu class extends the SubMenu class.
*/

#include "game.hpp"           // USES GameStyleType
#include "gui/menuitem.hpp"   // HASA MenuItem
#include "gui/sizemenu.hpp"   // HASA SizeMenu
#include "gui/gameview.hpp"   // USES TileSizeType


class ViewMenu: public SubMenu {
public:
    // public lifecycle
#ifdef _QT
    ViewMenu(void);
#elif defined(_WINDOWS)
    // no default constructor
    ViewMenu(Menu const&, unsigned position);
#endif  // defined(_WINDOWS)
    // ~ViewMenu(void);  implicitly defined destructor

    // misc public methods
    void Animation(bool);
    void BoardTileSize(TileSizeType);
    void EnableItems(Partial const&, ThinkModeType);
    void HandTileSize(TileSizeType);
    void ShowClocks(bool);
    void ShowGrid(bool);
    void ShowHints(bool);
    void ShowScores(bool);
    void ShowTiles(bool);

private:
    SizeMenu mBoardSize;
    SizeMenu mTileSize;
    MenuItem mRecenter;
    MenuItem mAttrs;
    MenuItem mHints;
    MenuItem mShowClocks;
    MenuItem mShowGrid;
    MenuItem mShowScores;
    MenuItem mShowTiles;
    MenuItem mAnimation;

    // private lifecycle
    ViewMenu(ViewMenu const&);   // not copyable

    // private operators
    ViewMenu& operator=(ViewMenu const&);  // not assignable
};
#endif  // !defined(VIEWMENU_HPP_INCLUDED)
