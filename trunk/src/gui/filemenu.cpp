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

#include "gui/filemenu.hpp"
#ifdef _WINDOWS
# include "gui/resource.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT
FileMenu::FileMenu(void):
    SubMenu(tr("&File")),
    mNew(Qt(), tr("&New\tCtrl+N")),
    mOpen(Qt(), tr("&Open...\tCtrl+O")),
    mReopen(Qt(), tr("Open &Recent...")),
    mRevert(Qt(), tr("Re&vert")),
    mSave(Qt(), tr("&Save\tCtrl+S")),
    mSaveAs(Qt(), tr("Save &As...")),
    mClose(Qt(), tr("&Close\tCtrl+W")),
    mPrint(Qt(), tr("&Print...\tCtrl+P")),
    mExit(Qt(), tr("&Exit"))
{
    mNew.SetShortcut(tr("Ctrl+N"));
    mOpen.SetShortcut(tr("Ctrl+O"));
    mSave.SetShortcut(tr("Ctrl+S"));
    mClose.SetShortcut(tr("Ctrl+W"));
    mPrint.SetShortcut(tr("Ctrl+P"));
    mExit.SetShortcut(tr("Alt+F4"));

    Add(mNew);
    Add(mOpen);
    Add(mReopen);
    Add(mRevert);
    AddSeparator();
    Add(mSave);
    Add(mSaveAs);
    AddSeparator();
    Add(mClose);
    Add(mPrint);
    Add(mExit);
}

#elif defined(_WINDOWS)

FileMenu::FileMenu(Menu const &rRootMenu, unsigned position):
    SubMenu(rRootMenu, position),
    mNew(rRootMenu, IDM_NEW),
    mOpen(rRootMenu, IDM_OPEN),
    mReopen(rRootMenu, IDM_REOPEN),
    mRevert(rRootMenu, IDM_REVERT),
    mSave(rRootMenu, IDM_SAVE),
    mSaveAs(rRootMenu, IDM_SAVE_AS),
    mClose(rRootMenu, IDM_CLOSE),
    mPrint(rRootMenu, IDM_PRINT),
    mExit(rRootMenu, IDM_EXIT)
{}

#endif // defined(_WINDOWS)

// The compiler-generated destructor is OK.


// misc methods

void FileMenu::EnableItems(bool haveGame) {
    mNew.Enable(true);
    mOpen.Enable(false); // TODO
    mReopen.Enable(false);
    mRevert.Enable(false);
    mSave.Enable(false);
    mSaveAs.Enable(false);
    mClose.Enable(haveGame);
    mPrint.Enable(false);
    mExit.Enable(true);
}
