#ifndef PARMBOX_HPP_INCLUDED
#define PARMBOX_HPP_INCLUDED

// File:    parmbox.hpp
// Purpose: ParmBox class
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
A ParmBox object represents a dialog box for setting game parameters
such as the number of tile in a player's hand.

The ParmBox class is an extension of the Dialog class.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "dialog.hpp"

class ParmBox: public Dialog {
public:
	ParmBox(void) { ASSERT(false); };
	ParmBox(Window const &parent); 
    ParmBox(ParmBox const &) { ASSERT(false); };
    // ~ParmBox(void);  compiler-generated destructor is OK

	// operators
    ParmBox &operator=(ParmBox const &) { ASSERT(false); };

	// misc methods
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);
};

#endif
#endif