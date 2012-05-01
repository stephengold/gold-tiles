#ifndef PLAYMENU_HPP_INCLUDED
#define PLAYMENU_HPP_INCLUDED

// File:     playmenu.hpp
// Location: src/gui
// Purpose:  declare PlayMenu class
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
A PlayMenu object represents the "Play" submenu in the menu bar.

The PlayMenu class extends the SubMenu class.
*/

#include "game.hpp"          // USES GameStyleType
#include "gui/menuitem.hpp"  // HASA MenuItem
#include "gui/submenu.hpp"   // ISA SubMenu


class PlayMenu: public SubMenu {
    Q_OBJECT

public:
    // public lifecycle
    // no default constructor
#ifdef _QT
    PlayMenu(void);
#elif defined(_WINDOWS)
    PlayMenu(Menu const&, unsigned position);
#endif // defined(_WINDOWS)
    // ~PlayMenu(void);  compiler-generated destructor is fine

    // misc public methods
    void Autopause(bool);
    void EnableItems(Partial const&, ThinkModeType);
    void Pause(bool);

private:
    // private data
    MenuItem mPlay;
    MenuItem mTakeBack;
    MenuItem mSuggest;
    MenuItem mPause;
    MenuItem mSwapAll;
    MenuItem mPass;
    MenuItem mResign;
    MenuItem mRestart;
    MenuItem mUndo;
    MenuItem mRedo;
    MenuItem mAutopause;

    // private lifecycle
    PlayMenu(PlayMenu const&); // not copyable

    // private operators
    PlayMenu& operator=(PlayMenu const&); // not assignable
};
#endif // !defined(PLAYMENU_HPP_INCLUDED)
