// File:     sizemenu.cpp
// Location: src/gui
// Purpose:  implement SizeMenu class
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

#ifdef _WINDOWS
# include "gui/resource.hpp"
#endif  // defined(_WINDOWS)
#include "gui/sizemenu.hpp"


// lifecycle

#ifdef _QT

SizeMenu::SizeMenu(QString const& label)
:   SubMenu(label),
    mSize1(Qt(), "&1 Tiny"),
    mSize2(Qt(), "&2 Small"),
    mSize3(Qt(), "&3 Smallish"),
    mSize4(Qt(), "&4 Medium"),
    mSize5(Qt(), "&5 Largish"),
    mSize6(Qt(), "&6 Large"),
    mSize7(Qt(), "&7 Very Large")
{
    Add(mSize1);
    Add(mSize2);
    Add(mSize3);
    Add(mSize4);
    Add(mSize5);
    Add(mSize6);
    Add(mSize7);
}

#elif defined(_WINDOWS)

SizeMenu::SizeMenu(Menu const& rRootMenu, unsigned position):
SubMenu(rRootMenu, position),
    mSize1(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS1),
    mSize2(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS2),
    mSize3(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS3),
    mSize4(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS4),
    mSize5(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS5),
    mSize6(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS6),
    mSize7(rRootMenu, IDM_TS1, IDM_TS7, IDM_TS7)
{}

#endif  // defined(_WINDOWS)

// The implicitly defined destructor is OK.


// misc methods

void SizeMenu::EnableItems(void) {
    mSize1.Enable(true);
    mSize2.Enable(true);
    mSize3.Enable(true);
    mSize4.Enable(true);
    mSize5.Enable(true);
    mSize6.Enable(true);
    mSize7.Enable(true);
}

void SizeMenu::SetSize(TileSizeType size) {
    switch (size) {
    case 1:
        mSize1.CheckRadio(true);
        break;
    case 2:
        mSize2.CheckRadio(true);
        break;
    case 3:
        mSize3.CheckRadio(true);
        break;
    case 4:
        mSize4.CheckRadio(true);
        break;
    case 5:
        mSize5.CheckRadio(true);
        break;
    case 6:
        mSize6.CheckRadio(true);
        break;
    case 7:
        mSize7.CheckRadio(true);
        break;
    default:
        FAIL();
    }
}
