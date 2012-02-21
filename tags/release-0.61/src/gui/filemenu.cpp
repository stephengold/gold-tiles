// File:    filemenu.cpp
// Purpose: FileMenu class
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
#include "gui/filemenu.hpp"

// lifecycle

FileMenu::FileMenu(HMENU menu, unsigned position):
	SubMenu(menu, position),
    mNew(menu, IDM_NEW),
    mOpen(menu, IDM_OPEN),
    mReopen(menu, IDM_REOPEN),
    mRevert(menu, IDM_REVERT),
    mSave(menu, IDM_SAVE),
    mSaveAs(menu, IDM_SAVE_AS),
    mClose(menu, IDM_CLOSE),
    mPrint(menu, IDM_PRINT),
    mExit(menu, IDM_EXIT)
{}

// misc methods

void FileMenu::EnableItems(bool isGame) {
    mNew.Enable(true);
    mOpen.Enable(false); // TODO
    mReopen.Enable(false);
    mRevert.Enable(false);
    mSave.Enable(false);
    mSaveAs.Enable(false);
    mClose.Enable(isGame);
    mPrint.Enable(false);
    mExit.Enable(true);
}

