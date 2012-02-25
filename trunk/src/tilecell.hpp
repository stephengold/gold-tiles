#ifndef TILECELL_HPP_INCLUDED
#define TILECELL_HPP_INCLUDED

// File:    tilecell.hpp
// Purpose: TileCell class
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
A TileCell object describes a playing piece (Tile) and also the location
where it is to be played.
*/

#include "cell.hpp"  // HASA Cell
#include "tile.hpp"  // HASA Tile

class TileCell {
public:
	// lifecycle
	TileCell(void);
    TileCell(Tile const &);  // swap to swap area
    TileCell(Tile const &, Cell const &);  // place on a cell
	// TileCell(TileCell const &);  compiler-generated copy constructor is OK
    // ~TileCell(void);  compiler-generated destructor is OK

	// operators
    bool   operator<(TileCell const &) const;
    // TileCell &operator=(TileCell const &);  compiler-generated assignment method is OK
	operator Cell(void) const;
    operator String(void) const;
	operator Tile(void) const;

	// misc public methods
    String GetUserChoice(Tiles const &, Strings const &);

	// public inquiry methods
	bool   IsSwap(void) const;

private:
    Cell mCell;  // ignored if mSwapFlag is true
	bool mSwapFlag;
    Tile mTile;
};

#endif
