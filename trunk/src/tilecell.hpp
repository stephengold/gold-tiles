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
A TileCell object describes and playing piece 
(Tile) and also how it is to be played.
*/

#include "cell.hpp"
#include "tile.hpp"

class TileCell {
public:
	// lifecycle
	TileCell(void);
    TileCell(Tile const &);  // swap to swap area
    TileCell(Tile const &, Cell const &);  // place on a cell
	// TileCell(TileCell const &);  compiler-generated copy constructor is OK
    // ~TileCell(void);  implicitly declared destructor is fine

	// operators
    bool   operator<(TileCell const &) const;
    // TileCell &operator=(TileCell const &);  compiler-generated assignment method is OK
    operator String(void) const;

	// misc
    String GetUserChoice(Tiles const &, Strings const &);

	// access
	operator Cell(void) const;
	operator Tile(void) const;

	// inquiry
	bool   IsSwap(void) const;

private:
    Cell mCell;
	bool mSwapFlag;
    Tile mTile;
};

#endif
