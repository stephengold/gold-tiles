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
("clones") are possible; a unique id is used to distinguish 
between clones.
*/

#include "project.hpp"
#include "string.hpp"

// tile identifier type
typedef unsigned long TileIdType;

// attribute types and utilities
typedef unsigned ACountType, AIndexType;
typedef unsigned AValueType;

String     attributeToString(AIndexType, AValueType);
AValueType charToAttribute(AIndexType, char ch);

class Tile {
public:
	// lifecycle
	Tile(void);
	Tile(String const &);
	Tile(Tile const &);  // copy a tile
	~Tile(void);

    // operators
	bool   operator<(Tile const &) const;
	Tile & operator=(Tile const &);
	bool   operator==(Tile const &) const;
	operator String(void) const;

	// misc
	AValueType        Attribute(AIndexType) const;
    static ACountType AttributeCnt(void);
	Tile              Clone(void) const;
	unsigned          CommonAttribute(Tile const &) const;
	ACountType        CountMatchingAttributes(Tile const &) const;
	void              Display(void) const;
    static void       DisplayEmpty(void);
	String            GetUserChoice(Tiles const &, Strings const &);
	TileIdType        Id(void) const;
	void              SetAttribute(AIndexType, AValueType);
	static void       SetStatic(ACountType, AValueType const pValueMax[]);
    static String     StringEmpty(void);
    static AValueType ValueMax(AIndexType);

	// inquiry
	bool HasAttribute(AIndexType, AValueType) const;
	bool HasId(TileIdType) const;
	bool IsClone(Tile const &) const;
	bool IsCloneAny(Tiles const &) const;
	bool IsCompatibleWith(Tile const *) const;
	bool IsValid(void) const;

private:
	AValueType *mpArray;
	TileIdType   mId;

	static AIndexType   msAttributeCnt;  // number of attributes per tile
	static TileIdType   msNextId;
	static AValueType *mspValueMax;      // number of values for each tile attribute

	// misc
	static TileIdType NextId(void);
};

// Tile* utility function
//bool isValid(Tile const *);

#endif
