#ifndef MOVE_HPP_INCLUDED
#define MOVE_HPP_INCLUDED

// File:    move.hpp
// Purpose: Move class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Move object represents zero or more tiles and the
 locations where they will be played.
*/

#include <set>
#include "tilecell.hpp"

class Move: public std::set<TileCell> {
public:
    typedef std::set<TileCell>::const_iterator ConstIteratorType;
 	typedef std::set<TileCell>::iterator       IteratorType;

	// operators
    operator String(void) const;

	// misc
    void     Add(Tile const &);
    void     Add(Tile const &, Cell const &);
    void     GetUserChoice(Tiles const &);
	void     MakeEmpty(void);
 
	// access
    unsigned Count(void) const;
    Cells    GetCells(void) const;
    Tiles    GetTiles(void) const;

	// inquiry
	bool     InvolvesSwap(void) const;
	bool     IsPass(void) const;
	bool     IsPureSwap(void) const;
    bool     RepeatsCell(void) const;
    bool     RepeatsTile(void) const;
};

#endif
