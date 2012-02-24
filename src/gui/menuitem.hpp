#ifndef MENUITEM_HPP_INCLUDED
#define MENUITEM_HPP_INCLUDED

// File:    menuitem.hpp
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

/*
A MenuItem object represents a single item in a SubMenu.

The MenuItem class is implemented by encapsulating a handle and an item id.
*/

#include "gui/win.hpp"
#include "project.hpp"

typedef Win::UINT IdType;

class MenuItem {
public:
	// lifecycle
    MenuItem(Menu const &, IdType);
	// no default constructor
	// ~MenuItem(void);  compiler-generated destructor is OK

	// misc operators
	MenuItem &operator=(MenuItem const &) { FAIL(); };

	// misc public methods
    void Check(bool);
    void Enable(bool);

private:
    IdType       mItemId;
    Menu const &mrMenu;
};
#endif
