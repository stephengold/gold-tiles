#ifndef PARMBOX1_HPP_INCLUDED
#define PARMBOX1_HPP_INCLUDED

// File:    parmbox1.hpp
// Purpose: ParmBox1 class
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
A ParmBox1 object represents a dialog box for setting the style of the game.

The ParmBox1 class is an extension of the Dialog class.
*/

#include "game.hpp"        // HASA GameStyleType
#include "gui/dialog.hpp"  // ISA Dialog

class ParmBox1: public Dialog {
public:
    // lifecycle
	ParmBox1(GameStyleType, unsigned secondsPerHand);
	// no default constructor
	// ~ParmBox1(void);  compiler-generated destructor is OK

	// public operators
	operator GameStyleType(void) const;

	// misc public methods
	Win::INT_PTR  HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
	unsigned PlayerSeconds(void) const;

	// constants
	static const ValueType PLAYER_MINUTES_MIN = 2;
	static const ValueType PLAYER_MINUTES_DEFAULT = 30;
	static const ValueType PLAYER_MINUTES_MAX = 120;

private:
	GameStyleType mGameStyle;
    ValueType     mPlayerMinutes;

	// private lifecycle
	ParmBox1(ParmBox1 const &);    // not copyable

	// private operators
    ParmBox1 &operator=(ParmBox1 const &);   // not assignable

	// misc private methods
    void SetStyle(void);
	void SetStyle(IdType buttonId);
};

#endif
