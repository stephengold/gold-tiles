#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

// File:     tile.hpp
// Location: src
// Purpose:  declare Tile class
// Author:   Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of theGNU General Public License

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
A Tile object represents a playing piece which may be played
on a Board object or stored in a hand or the stock bag (Tiles object).
Each tile has at least two attributes, each of which can assume up
to ten values.  In the context of a particular game, every Tile has the 
same number of attributes.  Distinct tiles with identical attributes 
(clones) are possible; a unique id is used to distinguish clones.
In addition, some randomly-selected tiles are bonus tiles with extra value.
*/

#include "combo.hpp"    // HASA Combo
#include "indices.hpp"  // HASA IndexType

// tile identifier type
typedef IndexType TileIdType;

class Tile {
public:
	// public constants
	static const TileIdType ID_NONE = 0;
	static const TileIdType ID_DEFAULT = 1; // generated only by default constructor
	static const TileIdType ID_FIRST = 2;
	static const TileIdType ID_LAST = LONG_MAX;

    // public lifecycle
	Tile(void);
	Tile(String const&);
	// Tile(Tile const&);  compiler-generated copy constructor is OK
	// ~Tile(void);  compiler-generated destructor is OK

    // public operators
	bool     operator<(Tile const&) const;
	Tile&    operator=(Tile const&);
	bool     operator==(Tile const&) const;
	operator Combo(void) const;
	operator String(void) const;

	// misc public methods
	AttrType           Attribute(AttrIndexType) const;
	static double      BonusProbability(void);
	Tile               CloneAndSetBonus(void) const;
	AttrIndexType      CommonAttribute(Tile const&) const;
	void               Display(void) const;
    static void        DisplayEmpty(void);
	String             GetUserChoice(Tiles const&, Strings const&);
	TileIdType         Id(void) const;
	void               SetAttribute(AttrIndexType, AttrType);
	static void        SetStatic(GameOpt const&);
    static String      StringEmpty(void);

	// public inquiry methods
	bool HasAttribute(AttrIndexType, AttrType) const;
	bool HasBonus(void) const;
	bool HasId(TileIdType) const;
	bool IsClone(Tile const&) const;
	bool IsCloneAny(Tiles const&) const;
	bool IsCompatibleWith(Tile const*) const;
	bool IsValid(void) const;
	bool MatchesString(String const&) const;

private:
	// private data
	unsigned   mBonusValue;
	Combo      mCombo;     // all attributes
	TileIdType mId;        // to distinguish clones

	static double        msBonusProbability;
	static TileIdType    msNextId;

	// misc private methods
	static TileIdType NextId(void);
};
#endif // !defined(TILE_HPP_INCLUDED)
