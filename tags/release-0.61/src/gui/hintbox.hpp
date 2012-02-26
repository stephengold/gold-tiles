#ifndef HINTBOX_HPP_INCLUDED
#define HINTBOX_HPP_INCLUDED

// File:    hintbox.hpp
// Purpose: HintBox class
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
A HintBox object represents a dialog box for setting hint parameters.

The HintBox class is an extension of the Dialog class.
*/

#include "game.hpp"
#include "gui/dialog.hpp"
#include "partial.hpp"

class HintBox: public Dialog {
public:
    // lifecycle
	HintBox(HintType, GameStyleType);
	// no default constructor
	// no copy constructor
	// ~HintBox(void);  compiler-generated destructor is OK

	// public operators
    HintBox &operator=(HintBox const &) { FAIL(); };
	operator HintType(void) const;

	// misc public methods
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);

private:
	HintType      mStrength;
	GameStyleType mGameStyle;

	// misc private methods
	void SetHintStrength(void);
	void SetHintStrength(IdType buttonId);
};

#endif