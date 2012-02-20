#ifndef FILEMENU_HPP_INCLUDED
#define FILEMENU_HPP_INCLUDED

// File:    filemenu.hpp
// Purpose: FileMenu class
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
A FileMenu object represents the "File" SubMenu in the menu bar.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/menuitem.hpp"
#include "gui/submenu.hpp"

class FileMenu: public SubMenu {
public:
	// lifecycle
    FileMenu(HMENU menu, UINT position);
	// no default constructor
	//FileMenu(FileMenu const &);  compiler-generated destructor is OK

	// misc public methods
	void EnableItems(void);

private:
    MenuItem mNew, mOpen, mReopen, mRevert;
	MenuItem mSave, mSaveAs, mClose, mPrint, mExit;
};

#endif
#endif
