#ifndef TILES_HPP_INCLUDED
#define TILES_HPP_INCLUDED

// File:     tiles.hpp
// Location: src
// Purpose:  declare Tiles class
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

#include <map>       // HASA std::map
#include "tile.hpp"  // HASA Tile


class Tiles {
public:
    // public lifecycle
    Tiles(void);
	Tiles(Tile const&);
    // Tiles(Tiles const&);  compiler-generated copy constructor is OK
    // ~Tiles(void);  compiler-generated destructor is OK

	// public operators
    // Tiles& operator=(Tiles const&);  compiler-generated assignment method is OK
	bool operator==(Tiles const&) const;
	Tile operator[](unsigned) const;
	operator Indices(void) const;
	operator String(void) const;

	// misc public methods
	void     Add(Tile const&);
	void     AddAllTiles(AttrIndexType, Tile&);
	void     AddTiles(Tiles const&);
	unsigned BonusFactor(void) const;
	AttrIndexType
		     CommonAttribute(void) const;
    bool     CopyIds(Tiles const&);
	unsigned Count(void) const;
	Tile     DrawFirstTile(void);
    Tile     DrawRandomTile(void);
	void     DrawTiles(unsigned, Tiles& bag);
	Tile     FindTile(TileIdType) const;
    void     GetUserChoice(Tiles const&);
	Tiles    LongestRun(void) const;
	void     MakeEmpty(void);
	void     RemoveTile(Tile const&);
	void     RemoveTileId(TileIdType);
	void     RemoveTiles(Tiles const&);
	void     Restock(void);
    void     UnClone(Tile&) const;
    Tiles    UniqueTiles(void) const;

	// public inquiry methods
    bool AreAllCompatible(void) const;
    bool AreAllCompatibleWith(Tile const&) const;
    bool Contains(Tile const&) const;
	bool ContainsClone(Tile const&) const;
    bool ContainsId(TileIdType) const;
	bool IsEmpty(void) const;
    bool IsValid(void) const;

private:
	// private types
	typedef std::map<TileIdType,Tile>::const_iterator ConstIterator;
	typedef std::map<TileIdType,Tile>::iterator       Iterator;

	// private data
	std::map<TileIdType,Tile> mMap;

	// private methods
	void BuildRuns(Tiles const& runSoFar, Tiles& bestRun) const;
};
#endif // !defined(TILES_HPP_INCLUDED)
