#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

// File:     menu.hpp
// Location: src/gui
// Purpose:  declare Menu class
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

/*
A Menu object represents a top-level menu such as the menu bar of a window.

The Menu class is implemented by encapsulating a menu handle (Win::HMENU).
*/

#include "gui/win.hpp"   // HASA Win::HMENU


class Menu {
public:
    // public lifecycle
    // no default constructor
    explicit Menu(Win::HMENU);
    // ~Menu(void); The implicitly defined destructor is fine.

    // public operators
    operator Win::HMENU(void) const;

private:
    // private data
    Win::HMENU mHandle;

    // private lifecycle
    Menu(Menu const&);   // not copyable

    // private operators
    Menu& operator=(Menu const&);  // not copyable
};
#endif  // !defined(MENU_HPP_INCLUDED)
