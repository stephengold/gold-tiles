#ifndef SUBMENU_HPP_INCLUDED
#define SUBMENU_HPP_INCLUDED

// File:    submenu.hpp
// Purpose: SubMenu for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A SubMenu object represents a submenu of the menu bar, for example
 the "play" submenu or the "view" submenu.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

class SubMenu {
public:
	// lifecycle
	SubMenu(void) { ASSERT(false); }
    SubMenu(HMENU parent, UINT position);

	// misc
    void Enable(bool);

private:
    HMENU mParent;
    UINT  mPosition;
};

#endif
#endif
