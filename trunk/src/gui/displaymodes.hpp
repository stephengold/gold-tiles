#ifndef DISPLAYMODES_HPP_INCLUDED
#define DISPLAYMODES_HPP_INCLUDED

// File:    displaymodes.hpp
// Purpose: DisplayModes class header file
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
A DisplayModes object represents the parameters used to display tiles in the GUI.

The DisplayModes class encapsulates an array of ADisplayType values.
*/

#include "tile.hpp"  // HASA ADisplayType 

class DisplayModes {
public:
	// public lifecycle
	DisplayModes(void);
    DisplayModes(DisplayModes const &);
    // ~DisplayModes(void);  compiler-generated destructor is OK

	// public operators
    DisplayModes &operator=(DisplayModes const &);

	// misc public methods
	ADisplayType Mode(AIndexType) const;
	void         SetMode(AIndexType, ADisplayType);

	// public inquiry methods
	bool IsColor(AIndexType) const;

private:
	// private data
	ADisplayType mArray[Tile::ATTRIBUTE_CNT_MAX];
};
#endif // !defined(DISPLAYMODES_HPP_INCLUDED)
