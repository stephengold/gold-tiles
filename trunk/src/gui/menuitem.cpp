// File:     menuitem.cpp
// Location: src/gui
// Purpose:  implement MenuItem class
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

#include "gui/menuitem.hpp"
#ifdef _WINDOWS
# include "gui/menu.hpp"
# include "gui/submenu.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT

MenuItem::MenuItem(QObject* pParent, QString const& rLabel) {
    mpAction = new QAction(rLabel, pParent);
    ASSERT(mpAction != NULL);
}

#elif defined(_WINDOWS)

MenuItem::MenuItem(Menu const& rMenu, IdType itemId):
mrMenu(rMenu)
{
    mFirstItemId = itemId;
    mItemId = itemId;
    mLastItemId = itemId;
}

// a menu item that's part of a group
MenuItem::MenuItem(Menu const& rMenu, IdType firstId, IdType lastId, IdType itemId):
mrMenu(rMenu)
{
    mFirstItemId = firstId;
    mItemId = itemId;
    mLastItemId = lastId;
}

#endif // defined(_WINDOWS)

// The compiler-generated destructor is OK.


// misc methods

#ifdef _QT

QAction *MenuItem::pAction(void) {
    return mpAction;
}

#endif // defined(_QT)

void MenuItem::Check(bool checkedFlag) {
#ifdef _QT
    mpAction->setChecked(checkedFlag);
#elif defined(_WINDOWS)
    UINT flags = MF_BYCOMMAND;
    if (checkedFlag) {
        flags |= MF_CHECKED;
    } else {
        flags |= MF_UNCHECKED;
    }
    Win::CheckMenuItem(HMENU(mrMenu), mItemId, flags);
#endif // defined(_WINDOWS)
}

void MenuItem::CheckRadio(bool checkedFlag) {
#ifdef _QT
    mpAction->setChecked(checkedFlag);
#elif defined(_WINDOWS)
    UINT flags = MF_BYCOMMAND;
    if (checkedFlag) {
        flags |= MF_CHECKED;
    } else {
        flags |= MF_UNCHECKED;
    }
    Win::CheckMenuRadioItem(HMENU(mrMenu), mFirstItemId, mLastItemId, mItemId, flags);
#endif // defined(_WINDOWS)
}

void MenuItem::Enable(bool enabledFlag) {
#ifdef _QT
    mpAction->setEnabled(enabledFlag);
#elif defined(_WINDOWS)
    UINT flags = MF_BYCOMMAND;
    if (enabledFlag) {
        flags |= MF_ENABLED;
    } else {
        flags |= MF_GRAYED;
    }
    Win::EnableMenuItem(HMENU(mrMenu), mItemId, flags);
#endif // defined(_WINDOWS)
}

#ifdef _QT

void MenuItem::SetShortcut(QString const &sequence) {
    mpAction->setShortcut(sequence);
}

#endif // defined(_QT)
