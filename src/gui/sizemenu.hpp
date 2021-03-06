#ifndef SIZEMENU_HPP_INCLUDED
#define SIZEMENU_HPP_INCLUDED

// File:     sizemenu.hpp
// Location: src/gui
// Purpose:  declare SizeMenu class
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
A SizeMenu object represents a tile size SubMenu in the "View" Menu.

The SizeMenu class extends the SubMenu class.
*/

#include "gui/gameview.hpp"  // USES TileSizeType
#include "gui/menuitem.hpp"  // HASA MenuItem
#include "gui/submenu.hpp"   // ISA SubMenu


class SizeMenu: public SubMenu {
public:
    // public lifecycle
#ifdef _QT
    SizeMenu(QString const& label);
#elif defined(_WINDOWS)
    // no default constructor
    SizeMenu(Menu const&, SizeType position, IdType ts1);
#endif  // defined(_WINDOWS)
    // ~SizeMenu(void);  implicitly defined destructor

    // misc public methods
    void EnableItems(void);
    void SetSize(TileSizeType);

private:
    MenuItem mSize1;
    MenuItem mSize2;
    MenuItem mSize3;
    MenuItem mSize4;
    MenuItem mSize5;
    MenuItem mSize6;
    MenuItem mSize7;

    // private lifecycle
    SizeMenu(SizeMenu const&);  // not copyable

    // private operators
    SizeMenu& operator=(SizeMenu const&);  // not assignable
};
#endif  // !defined(SIZEMENU_HPP_INCLUDED)
