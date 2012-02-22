#ifndef MENUBAR_HPP_INCLUDED
#define MENUBAR_HPP_INCLUDED

// File:    menubar.hpp
// Purpose: MenuBar class
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
A MenuBar object represents the menu bar of a window.
*/

#include "gui/filemenu.hpp"
#include "gui/playmenu.hpp"
#include "gui/viewmenu.hpp"

class MenuBar: public SubMenu {
public:
	// lifecycle
    MenuBar(Win::CREATESTRUCT const *);
	// no default constructor

private:
	FileMenu mFile;
	PlayMenu mPlay;
	ViewMenu mView;
	SubMenu  mHelp;
};

#endif
