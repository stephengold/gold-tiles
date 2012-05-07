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
A HandBox object represents a dialog box for setting hand parameters
such as the type of player.

The HandBox class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"   // ISA Dialog
#include "handopt.hpp"      // HASA HandOpt

class HandBox: public Dialog {
public:
    // public lifecycle
	HandBox(unsigned handIndex, bool more, HandOpt const &options); 
	// no default constructor
	// ~HandBox(void);  compiler-generated destructor is OK

	// public operators
	operator HandOpt(void) const;

	// misc public methods
	int    HandleMessage(MessageType, Win::WPARAM);

private:
	// private constants
	static const ValueType LEVEL_MAX = 8;

	// private data
	bool          mAreMoreHands;
	unsigned      mHandIndex;
	HandOpt       mOptions;

	// private lifecycle
    HandBox(HandBox const &);  // not copyable

	// private operators
    HandBox &operator=(HandBox const &);  // not assignable

	// private misc methods
	void HandleButtonClick(IdType);
	void UpdateButtons(void);
	void UpdateNameBox(String const &);
	void UpdateSlider(void);
};
#endif