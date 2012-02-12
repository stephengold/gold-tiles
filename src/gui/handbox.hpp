#ifndef HANDBOX_HPP_INCLUDED
#define HANDBOX_HPP_INCLUDED

// File:    handbox.hpp
// Purpose: HandBox class
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

#include "gui/dialog.hpp"
#include "string.hpp"

class HandBox: public Dialog {
public:
	static const INT_PTR RESULT_BACK = 5;

    // lifecycle
	HandBox(unsigned handIndex, bool moreFlag); 
	// no default constructor
	// no copy constructor
	// ~HandBox(void);  compiler-generated destructor is OK

	// public operators
    ParmBox &operator=(ParmBox const &) { ASSERT(false); };

	// misc public methods
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);
	String  PlayerName(void) const;

	// public inquiry methods
	bool IsAutomatic(void) const;
	bool IsRemote(void) const;

private:
	bool      mAreMoreHands;
	unsigned  mHandIndex;
    bool      mIsAutomatic, mIsRemote;
	String    mPlayerName;
};

#endif
