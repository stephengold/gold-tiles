#ifndef TILES_HPP_INCLUDED
#define TILES_HPP_INCLUDED

// File:    tiles.hpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <set>
#include "tile.hpp"

class Tiles: public std::set<Tile> {
public:
	typedef std::set<Tile>::const_iterator ConstIteratorType;
	typedef std::set<Tile>::iterator       IteratorType;

	// operators
	operator String(void) const;

	// misc
	void     AddTile(Tile const &);
	void     AddTiles(Tiles const &);
    bool     CopyIds(Tiles const &);
    Tile     DrawRandomTile(void);
	unsigned DrawTiles(unsigned, Tiles &rBag);
	Tile     FindTile(TileIdType) const;
    void     GetUserChoice(Tiles const &);
	Tiles    LongestRun(void) const;
	void     MakeEmpty(void);
	void     RemoveTile(Tile const &);
	void     RemoveTiles(Tiles const &);
    void     UnClone(Tile &) const;
    Tiles    UniqueTiles(void) const;

	// access
	unsigned Count(void) const;

	// inquiry
    bool     AreAllCompatible(void) const;
    bool     Contains(Tile const &) const;
	bool     IsEmpty(void) const;
    bool     IsValid(void) const;
};

#endif
