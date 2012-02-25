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

#include "game.hpp"
#include "gui/menuitem.hpp" // HASA MenuItem
#include "gui/submenu.hpp"  // ISA SubMenu


class FileMenu: public SubMenu {
public:
	// public lifecycle
    FileMenu(Menu const &, unsigned position);
	// no default constructor
	//FileMenu(FileMenu const &);  compiler-generated destructor is OK

	// misc public methods
	void EnableItems(bool haveGame);

private:
	// private data
    MenuItem mNew, mOpen, mReopen, mRevert;
	MenuItem mSave, mSaveAs, mClose, mPrint, mExit;

	// private lifecycle
	FileMenu(FileMenu const &);   // not copyable

	// private operators
	FileMenu &operator=(FileMenu const &);  // not assignable
};

#endif
