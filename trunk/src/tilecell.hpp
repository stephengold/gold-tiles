#ifndef TILECELL_HPP_INCLUDED
#define TILECELL_HPP_INCLUDED

// File:    tilecell.hpp
// Purpose: TileCell class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
