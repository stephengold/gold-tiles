#ifndef PARMBOX2_HPP_INCLUDED
#define PARMBOX2_HPP_INCLUDED

// File:     parmbox2.hpp
// Location: src/gui
// Purpose:  declare ParmBox2 class
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
A ParmBox2 object represents a dialog box for setting board parameters
such as the number of rows and columns.

The ParmBox2 class extends the Dialog class.
*/

#include "gameopt.hpp"     // HASA GameOpt
#include "gui/dialog.hpp"  // ISA Dialog


class ParmBox2: public Dialog {
public:
	// public constants
	static const RowType    HEIGHT_DEFAULT = 12;
	static const ColumnType WIDTH_DEFAULT = 12;

    // lifecycle
	ParmBox2(GameOpt&);
	// no default constructor
	// ~ParmBox2(void);  compiler-generated destructor is OK

	// misc public methods
	int HandleMessage(MessageType, Win::WPARAM);

private:
	// private data
	GameOpt& mrGameOpt;

	// private lifecycle
    ParmBox2(ParmBox2 const&);  // not copyable

	// private operators
    ParmBox2& operator=(ParmBox2 const&);  // not assignable

	// misc private methods
	ValueType GetTextIndex(IdType);
	void      SetGrid(void);
	void      SetGrid(IdType);
	void      SetTextIndex(IdType, ValueType);
	void      SetTopology(void);
	void      SetTopology(IdType);
	void      UpdateCellCnt(void);
};
#endif // !defined(PARMBOX2_CPP_INCLUDED)
