#ifndef SUBMENU_HPP_INCLUDED
#define SUBMENU_HPP_INCLUDED

// File:    submenu.hpp
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
    SubMenu(HMENU parent, UINT position);
	// no default constructor

	// misc public methods
    void Enable(bool);

private:
    HMENU mParent;
    UINT  mPosition;
};

#endif
#endif
