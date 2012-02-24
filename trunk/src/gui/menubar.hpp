#ifndef MENUBAR_HPP_INCLUDED
#define MENUBAR_HPP_INCLUDED

// File:    menubar.hpp
// Purpose: MenuBar class
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
A MenuBar object represents the menu bar of the top window.
*/

#include "gui/filemenu.hpp"
#include "gui/menu.hpp"
#include "gui/playmenu.hpp"
#include "gui/submenu.hpp"
#include "gui/viewmenu.hpp"

class MenuBar {
public:
	// lifecycle
    MenuBar(Win::CREATESTRUCT const &, Partial const &);
	// no default constructor
	
	// public operators
	MenuBar &operator=(MenuBar const &) { FAIL(); }

    // misc public methods
    void GameOver(void);
    void HandleMenuCommand(IdType);
    void NewGame(void);
    void SetTileSize(IdType);
    void Update(void);

    // public inquiry methods
    bool AreClocksVisible(void) const;    
    bool AreScoresVisible(void) const;
    bool IsAutocenter(void) const;
    bool IsAutopause(void) const;    
    bool IsGridVisible(void) const;
    bool IsPeeking(void) const;

private:
	bool             mAutocenterFlag;
	bool             mAutopauseFlag;
	FileMenu         mFileMenu;
	SubMenu          mHelpMenu;
	Menu             mMenu;
	Partial const & mrPartial;
    bool             mPeekFlag;
	PlayMenu         mPlayMenu;
	bool             mShowClocksFlag;
    bool             mShowGridFlag;
	bool             mShowScoresFlag;
	IdType           mTileSizeItem;
	ViewMenu         mViewMenu;
};
#endif
