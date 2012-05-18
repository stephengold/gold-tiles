// File:     menu.cpp
// Location: src/gui
// Purpose:  implement Menu class
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

#include "menu.hpp"
#include "gui/win_types.hpp"

// lifecycle

Menu::Menu(HMENU handle) {
    mHandle = handle;
}

// The implicitly defined copy constructor is OK.
// The implicitly defined destructor is OK.


// operators

// The implicitly defined assignment method is OK.
Menu::operator HMENU(void) const {
    return mHandle;
}
