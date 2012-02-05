// File:    menuitem.cpp
// Purpose: implement the MenuItem class.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/menuitem.hpp"

// MenuItem

MenuItem::MenuItem(HMENU handle, UINT itemId):
    mHandle(handle),
	mItemId(itemId)
{}

void MenuItem::Check(bool checkedFlag) {
	if (checkedFlag) {
	    ::CheckMenuItem(mHandle, mItemId, MF_CHECKED | MF_BYCOMMAND);
	} else {
	    ::CheckMenuItem(mHandle, mItemId, MF_UNCHECKED | MF_BYCOMMAND);
	}
}

void MenuItem::Enable(bool enabledFlag) {
	if (enabledFlag) {
    	::EnableMenuItem(mHandle, mItemId, MF_ENABLED | MF_BYCOMMAND);
	} else {
    	::EnableMenuItem(mHandle, mItemId, MF_GRAYED | MF_BYCOMMAND);
	}
}

#endif