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
such as the number of tiles per hand.

The ParmBox class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"
#include "tile.hpp"

class ParmBox: public Dialog {
public:
    // lifecycle
	ParmBox(); 
	// no copy constructor
	// ~ParmBox(void);  compiler-generated destructor is OK

	// public operators
    ParmBox &operator=(ParmBox const &) { ASSERT(false); };

	// misc public methods
	ACountType AttributeCnt(void) const;
	unsigned   ClonesPerTile(void) const;
	unsigned   HandCnt(void) const;
	INT_PTR    HandleMessage(UINT message, WPARAM, LPARAM);
	unsigned   HandSize(void) const;

private:
    ValueType mAttributeCnt, mClonesPerTile, mHandSize, mHandCnt;

	// misc private methods
    IdType    EditboxId(IdType slider) const;
    void      InitControl(IdType slider, ValueType value, 
		                  ValueType min, ValueType max);
    IdType    MaxId(IdType slider) const;
    IdType    MinId(IdType slider) const;
    IdType    SliderId(IdType editbox) const;
	IdType    SliderId(HWND) const;
	void      UpdateValue(IdType slider, ValueType);
};

#endif
