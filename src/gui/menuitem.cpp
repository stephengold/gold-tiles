// File:    menuitem.cpp
// Purpose: MenuItem class
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

#include "gui/menuitem.hpp"
#include "gui/win_types.hpp"

// lifecycle

MenuItem::MenuItem(HMENU handle, IdType itemId):
    mHandle(handle),
	mItemId(itemId)
{}

// misc menus

void MenuItem::Check(bool checkedFlag) {
	UINT flags = MF_BYCOMMAND;
	if (checkedFlag) {
	    flags |= MF_CHECKED;
	} else {
	    flags |= MF_UNCHECKED;
	}
	Win::CheckMenuItem(mHandle, mItemId, flags);
}

void MenuItem::Enable(bool enabledFlag) {
	UINT flags = MF_BYCOMMAND;
	if (enabledFlag) {
    	flags |= MF_ENABLED;
	} else {
    	flags |= MF_GRAYED;
	}
   	Win::EnableMenuItem(mHandle, mItemId, flags);
}