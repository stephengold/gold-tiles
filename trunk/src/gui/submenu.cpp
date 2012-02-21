// File:    submenu.cpp
// Purpose: SubMenu class
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

#include "gui/submenu.hpp"
#include "gui/win_types.hpp"


// lifecycle

SubMenu::SubMenu(SubMenu const &parent, unsigned position) {
    mParent = HMENU(parent);
    mPosition = position;
}

// operators

SubMenu::operator HMENU(void) const {
	return mParent;
}

// misc methods

void SubMenu::Enable(bool enabledFlag) {
	UINT position = mPosition;
	UINT flags = MF_BYPOSITION;

	if (enabledFlag) {
  	    flags |= MF_ENABLED;
	} else {
		flags |= MF_GRAYED;
	}
	Win::EnableMenuItem(mParent, position, flags);
}