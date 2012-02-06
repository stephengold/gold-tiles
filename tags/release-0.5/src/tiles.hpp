#ifndef TILES_HPP_INCLUDED
#define TILES_HPP_INCLUDED

// File:    tiles.hpp
// Purpose: Tiles class
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
