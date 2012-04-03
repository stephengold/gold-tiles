#ifndef PARMBOX3_HPP_INCLUDED
#define PARMBOX3_HPP_INCLUDED

// File:    parmbox3.hpp
// Purpose: ParmBox3 class
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
A ParmBox3 object represents a dialog box for setting hand/tile parameters
such as the number of tiles per hand.

The ParmBox3 class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"   // ISA Dialog
#include "tile.hpp"         // AttrCntType

class ParmBox3: public Dialog {
public:
    // lifecycle
	ParmBox3(unsigned attributeCnt, unsigned clonesPerTile, 
		     unsigned handSize, unsigned handCnt, unsigned bonusPercent);
	// no default constructor
	// ~ParmBox3(void);  compiler-generated destructor is OK

	// misc public methods
	AttrCntType AttributeCnt(void) const;
	unsigned    BonusTilePercentage(void) const;
	unsigned    ClonesPerCombo(void) const;
	unsigned    HandCnt(void) const;
	int         HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
	unsigned    HandSize(void) const;

private:
    ValueType mAttributeCnt;
	ValueType mClonesPerCombo;
	ValueType mHandSize;
	ValueType mHandCnt;
	ValueType mBonusTilePercentage;

	// private lifecycle
    ParmBox3(ParmBox3 const &);  // not copyable

	// private operators
    ParmBox3 &operator=(ParmBox3 const &);  // not assignable

	// misc private methods
    static IdType EditboxId(IdType slider);
    void          InitControl(IdType slider, ValueType value, 
		                  ValueType min, ValueType max);
    static IdType MaxId(IdType slider);
    static IdType MinId(IdType slider);
    static IdType SliderId(IdType editbox);
	IdType        SliderId(Win::HWND) const;
	void          UpdateValue(IdType slider, ValueType);
};

#endif
