// File:     submenu.cpp
// Location: src/gui
// Purpose:  implement SubMenu class
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

#include "gui/submenu.hpp"
#ifdef _QT
# include <Qt>
# include "gui/menuitem.hpp"
#elif defined(_WINDOWS)
# include "gui/menu.hpp"
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// lifecycle

#ifdef _QT

SubMenu::SubMenu(QString const &label) {
    mpQMenu = new QMenu(label);
    ASSERT(mpQMenu != NULL);
}

#elif defined(_WINDOWS)

SubMenu::SubMenu(Menu const& rMenu, unsigned position):
    mrMenu(rMenu)
{
    mPosition = position;
}

#endif // defined(_WINDOWS)

// The compiler-generated destructor is OK.


// misc methods

#ifdef _QT

void SubMenu::Add(MenuItem& rItem) {
    QAction* p_action = rItem.pAction();
    mpQMenu->addAction(p_action);
}

void SubMenu::AddSeparator(void) {
    mpQMenu->addSeparator();
}

#endif // defined(_QT)

void SubMenu::Enable(bool enabledFlag) {
#ifdef _QT
    mpQMenu->setEnabled(enabledFlag);
#elif defined(_WINDOWS)
    UINT const position = mPosition;
    UINT flags = MF_BYPOSITION;

    if (enabledFlag) {
        flags |= MF_ENABLED;
    } else {
        flags |= MF_GRAYED;
    }
    Win::EnableMenuItem(HMENU(mrMenu), position, flags);
#endif // defined(_WINDOWS)
}

#ifdef _QT

QMenu *SubMenu::Qt(void) {
    return mpQMenu;
}

#endif // defined(_QT)
