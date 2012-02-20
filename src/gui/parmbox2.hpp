#ifndef PARMBOX2_HPP_INCLUDED
#define PARMBOX2_HPP_INCLUDED

// File:    parmbox2.hpp
// Purpose: ParmBox2 class
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
A ParmBox2 object represents a dialog box for setting board parameters
such as the number of rows and columns.

The ParmBox2 class is an extension of the Dialog class.
*/

#include "cell.hpp"
#include "gui/dialog.hpp"
#include "tile.hpp"

class ParmBox2: public Dialog {
public:
    // lifecycle
	ParmBox2(bool wrapFlag, IndexType height, IndexType width, GridType);
	// no default constructor
	// no copy constructor
	// ~ParmBox2(void);  compiler-generated destructor is OK

	// public operators
    ParmBox2 &operator=(ParmBox2 const &) { ASSERT(false); };
	operator GridType(void) const;

	// misc public methods
	INT_PTR   HandleMessage(UINT message, WPARAM, LPARAM);
	IndexType Height(void) const;
	IndexType Width(void) const;

	// public inquiry methods
	bool      DoesWrap(void) const;

	// constants
	static const IndexType HEIGHT_DEFAULT = 32;
	static const IndexType WIDTH_DEFAULT = 32;

private:
	GridType     mGrid;
	IndexType    mHeight;
	bool         mWrapFlag;
	IndexType    mWidth;

	// misc private methods
	ValueType GetTextIndex(IdType);
	void      SetGrid(void);
	void      SetGrid(IdType);
	void      SetTextIndex(IdType, ValueType);
	void      SetTopology(void);
	void      SetTopology(IdType);
	void      UpdateCellCnt(void);
};

#endif
