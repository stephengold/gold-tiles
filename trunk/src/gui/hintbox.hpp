#ifndef HINTBOX_HPP_INCLUDED
#define HINTBOX_HPP_INCLUDED

// File:     hintbox.hpp
// Location: src/gui
// Purpose:  declare HintBox class
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
A HintBox object represents a dialog box for setting hint parameters.

The HintBox class is an extension of the Dialog class.
*/

#include "game.hpp"         // HASA GameStyleType
#include "gui/dialog.hpp"   // ISA Dialog
#include "partial.hpp"      // HASA HintType

class HintBox: public Dialog {
public:
    // lifecycle
	HintBox(HintType, GameStyleType);
	// no default constructor
	// ~HintBox(void);  compiler-generated destructor is OK

	// public operators
	operator HintType(void) const;

	// misc public methods
	int HandleMessage(MessageType, Win::WPARAM);

private:
	// private data
	GameStyleType mGameStyle;
	HintType      mStrength;

	// private lifecycle
    HintBox (HintBox const&); // not copyable

	// private operators
    HintBox& operator=(HintBox const&); // not assignable

	// misc private methods
	TextType 
		 Name(void) const;
	void SetHintStrength(void);
	void SetHintStrength(IdType buttonId);
};
#endif // !defined(HINTBOX_HPP_INCLUDED)
