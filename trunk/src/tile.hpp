#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

// File:    tile.hpp
// Purpose: Tile class
// Author:  Stephen Gold sgold@sonic.net
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
on a Board object or stored in a bag or hand (Tiles).  In the
context of a particular game, every Tile has the same number 
of attributes.  Distinct tiles with identical attributes 
("clones") are possible; a unique id is used to distinguish clones.
*/

#include "indices.hpp"  // USES IndexType
#include "project.hpp"  // USES Tiles

// tile identifier type
typedef IndexType TileIdType;

// attribute types and utilities
typedef unsigned short ACountType, AIndexType;
typedef unsigned short AValueType;

String     attribute_to_string(AIndexType, AValueType);
AValueType char_to_attribute(AIndexType, char ch);

class Tile {
public:
	static const TileIdType ID_NONE = 0;
	static const TileIdType ID_DEFAULT = 1;
	static const TileIdType ID_FIRST = 2;
	static const TileIdType ID_LAST = LONG_MAX;

	// public lifecycle
	Tile(void);
	Tile(String const &);
	Tile(Tile const &);
	~Tile(void);

    // public operators
	bool   operator<(Tile const &) const;
	Tile & operator=(Tile const &);
	bool   operator==(Tile const &) const;
	operator String(void) const;

	// misc public methods
	AValueType        Attribute(AIndexType) const;
    static ACountType AttributeCnt(void);
	Tile              Clone(void) const;
	static long       CombinationCnt(void);
	AIndexType        CommonAttribute(Tile const &) const;
	ACountType        CountMatchingAttributes(Tile const &) const;
	void              Display(void) const;
    static void       DisplayEmpty(void);
	String            GetUserChoice(Tiles const &, Strings const &);
	TileIdType        Id(void) const;
	void              SetAttribute(AIndexType, AValueType);
	static void       SetStatic(ACountType, AValueType const pValueMax[]);
    static String     StringEmpty(void);
    static AValueType ValueMax(AIndexType);

	// public inquiry methods
	bool HasAttribute(AIndexType, AValueType) const;
	bool HasId(TileIdType) const;
	bool IsClone(Tile const &) const;
	bool IsCloneAny(Tiles const &) const;
	bool IsCompatibleWith(Tile const *) const;
	bool IsValid(void) const;

	static const ACountType ATTRIBUTE_CNT_MIN = 2;
	static const ACountType ATTRIBUTE_CNT_DEFAULT = 2;
	static const AValueType VALUE_CNT_MIN = 4;
	static const AValueType VALUE_CNT_DEFAULT = 4;
	static const AValueType VALUE_CNT_MAX = 9;

private:
	AValueType *mpArray;
	TileIdType   mId;

	static AIndexType   msAttributeCnt;  // number of attributes per tile
	static TileIdType   msNextId;
	static AValueType *mspValueMax;      // number of values for each tile attribute

	// misc private methods
	static TileIdType NextId(void);
};

#endif // !defined(TILE_HPP_INCLUDED)
