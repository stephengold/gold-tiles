// File:    submenu.cpp
// Purpose: implement the SubMenu class.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/submenu.hpp"

SubMenu::SubMenu(HMENU parent, UINT position):
	mParent(parent),
    mPosition(position)
 {}

void SubMenu::Enable(bool enabledFlag) {
	if (enabledFlag) {
  	    ::EnableMenuItem(mParent, mPosition, MF_ENABLED | MF_BYPOSITION);
	} else {
		::EnableMenuItem(mParent, mPosition, MF_GRAYED | MF_BYPOSITION);
	}
}

#endif
