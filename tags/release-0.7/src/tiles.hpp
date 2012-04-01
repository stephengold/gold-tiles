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

#include <map>       // HASA std::map
#include "tile.hpp"  // HASA Tile

class Tiles {
public:
    // public lifecycle
    // Tiles(void); compiler-generated default constructor is OK
    // Tiles(Tiles const &);  compiler-generated copy constructor is OK
    // ~Tiles(void);  compiler-generated destructor is OK

	// public operators
    // Tiles &operator=(Tiles const &);  compiler-generated assignment method is OK
	bool operator==(Tiles const &) const;
	Tile operator[](unsigned) const;
	operator Indices(void) const;
	operator String(void) const;

	// misc public methods
	void     Add(Tile const &);
	void     AddTiles(Tiles const &);
    bool     CopyIds(Tiles const &);
	unsigned Count(void) const;
    Tile     DrawRandomTile(void);
	unsigned DrawTiles(unsigned, Tiles &rBag);
	Tile     FindTile(TileIdType) const;
    void     GetUserChoice(Tiles const &);
	Tiles    LongestRun(void) const;
	void     MakeEmpty(void);
	void     RemoveTile(Tile const &);
	void     RemoveTileId(TileIdType);
	void     RemoveTiles(Tiles const &);
    void     UnClone(Tile &) const;
    Tiles    UniqueTiles(void) const;

	// public inquiry methods
    bool     AreAllCompatible(void) const;
    bool     Contains(Tile const &) const;
	bool     ContainsClone(Tile const &) const;
    bool     ContainsId(TileIdType) const;
	bool     IsEmpty(void) const;
    bool     IsValid(void) const;

private:
	typedef std::map<TileIdType,Tile>::const_iterator ConstIterator;
	typedef std::map<TileIdType,Tile>::iterator       Iterator;

	std::map<TileIdType,Tile> mMap;
};

#endif