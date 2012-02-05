#ifndef MENUITEM_HPP_INCLUDED
#define MENUITEM_HPP_INCLUDED

// File:    menuitem.hpp
// Purpose: MenuItem class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A MenuItem object represents one item in a SubMenu. 
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

class MenuItem {
public:
	// lifecycle
	MenuItem(void) { ASSERT(false); }
    MenuItem(HMENU, UINT);

	// misc
    void Check(bool);
    void Enable(bool);

private:
    HMENU mHandle;
    UINT  mItemId;
};

#endif
#endif
