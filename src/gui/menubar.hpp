#ifndef MENUBAR_HPP_INCLUDED
#define MENUBAR_HPP_INCLUDED

// File:     menubar.hpp
// Location: src/gui
// Purpose:  declare MenuBar class
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
A MenuBar object represents the menu bar of the top window.

In the Qt version, the MenuBar class extends the QMenuBar class.
In the native version, the MenuBar class encapsulates a Menu object.

*/

#include "gui/filemenu.hpp"  // HASA FileMenu
#include "gui/menu.hpp"      // HASA Menu
#include "gui/playmenu.hpp"  // HASA PlayMenu
#include "gui/viewmenu.hpp"  // HASA ViewMenu
#ifdef _QT
# include <QMenuBar.h>
#endif // defined(_QT)


#ifdef _QT
class MenuBar: public QMenuBar {
    Q_OBJECT
#elif defined(_WINDOWS)
class MenuBar {
#endif // defined(_WINDOWS)

public:
    // public lifecycle
    // no default constructor
#ifdef _QT
    explicit MenuBar(Partial const& rPartial);
#elif defined(_WINDOWS)
    MenuBar(Win::CREATESTRUCT const&, Partial const&);
#endif // defined(_WINDOWS)
    // ~MenuBar(void);

    // misc public methods
    unsigned BoardTileSize(void) const;
    void     GameOver(void);
    void     HandleMenuCommand(IdType);
    unsigned HandTileSize(void) const;
    void     LoadUserOptions(User const&);
    void     NewGame(GameStyleType old);
    void     SaveUserOptions(User&) const;
    void     SetBoardTileSize(unsigned);
    void     SetHandTileSize(unsigned);
    void     Update(ThinkModeType);

    // public inquiry methods
    bool AreClocksVisible(void) const;
    bool AreScoresVisible(void) const;
    bool IsAutopause(void) const;
    bool IsGridVisible(void) const;
    bool IsPeeking(void) const;

private:
    // private data
    bool     mAutopauseFlag;
    unsigned mBoardTileSize;
    FileMenu mFileMenu;
    unsigned mHandTileSize;
    SubMenu  mHelpMenu;
#ifdef _WINDOWS
    Menu     mMenu;
#endif // defined(_WINDOWS)
    Partial const&
            mrPartial;
    bool     mPeekFlag;
    PlayMenu mPlayMenu;
    bool     mShowClocksFlag;
    bool     mShowGridFlag;
    bool     mShowScoresFlag;
    SubMenu  mThinking;
    ViewMenu mViewMenu;

    // private lifecycle
    MenuBar(MenuBar const&);  // not copyable
    void Initialize(GameStyleType);

    // private operators
    MenuBar& operator=(MenuBar const&);  // not assignable
};
#endif // !defined(MENUBAR_HPP_INCLUDED)
