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
on a Board object or stored in a hand or the stock bag (Tiles object).
Each tile has at least two attributes, each of which can assume up
to ten values.  In the context of a particular game, every Tile has the 
same number of attributes.  Distinct tiles with identical attributes 
(clones) are possible; a unique id is used to distinguish clones.
In addition, some randomly-selected tiles are bonus tiles with extra value.
*/

#include "indices.hpp"  // USES IndexType
#include "project.hpp"  // USES Tiles

// tile identifier type
typedef IndexType TileIdType;

// attribute types
typedef unsigned short ACountType, AIndexType;
typedef unsigned short AValueType;

// attribute display types
enum ADisplayType {
	ADISPLAY_MIN = 0,
	ADISPLAY_ABC = 0,
	ADISPLAY_RST = 1,
	ADISPLAY_123 = 2,
#ifdef _GUI
	ADISPLAY_COLOR = 3,
	ADISPLAY_SHAPE = 4,
	ADISPLAY_MAX = 4,
	ADISPLAY_CNT = 5
#else // !defined(_GUI)
	ADISPLAY_MAX = 2,
	ADISPLAY_CNT = 3
#endif // !defined(_GUI)
};

class Tile {
public:
	// public constants
	static const ACountType ATTRIBUTE_CNT_MIN = 2;
	static const ACountType ATTRIBUTE_CNT_DEFAULT = 2;
#ifdef _GUI
	static const ACountType ATTRIBUTE_CNT_MAX = 5;
#endif // defined(_GUI)
	static const AValueType VALUE_CNT_MIN = 2;
	static const AValueType VALUE_CNT_DEFAULT = 6;
	static const AValueType VALUE_CNT_MAX = 9;

	static const TileIdType ID_NONE = 0;
	static const TileIdType ID_DEFAULT = 1; // generated only by default constructor
	static const TileIdType ID_FIRST = 2;
	static const TileIdType ID_LAST = LONG_MAX;

	// public lifecycle
	Tile(void);
	Tile(String const &);
	Tile(Tile const &);
	~Tile(void);

    // public operators
	bool     operator<(Tile const &) const;
	Tile &   operator=(Tile const &);
	bool     operator==(Tile const &) const;
	operator String(void) const;

	// misc public methods
	AValueType        Attribute(AIndexType) const;
    static ACountType AttributeCnt(void);
	static String     AttributeReport(void);
	static String     AttributeToString(ADisplayType, AValueType);
	static double     BonusProbability(void);
	Tile              CloneAndSetBonus(void) const;
	static long       CombinationCnt(void);
	AIndexType        CommonAttribute(Tile const &) const;
	ACountType        CountMatchingAttributes(Tile const &) const;
	static ADisplayType 
		              DefaultDisplayMode(AIndexType);
	void              Display(void) const;
    static void       DisplayEmpty(void);
	String            GetUserChoice(Tiles const &, Strings const &);
	TileIdType        Id(void) const;
	void              SetAttribute(AIndexType, AValueType);
	static void       SetStatic(ACountType, AValueType const maxes[], double bonusProb);
    static String     StringEmpty(void);
    static AValueType ValueCnt(AIndexType);
    static AValueType ValueMax(AIndexType);

	// public inquiry methods
	bool HasAttribute(AIndexType, AValueType) const;
	bool HasBonus(void) const;
	bool HasId(TileIdType) const;
	bool IsClone(Tile const &) const;
	bool IsCloneAny(Tiles const &) const;
	bool IsCompatibleWith(Tile const *) const;
	bool IsValid(void) const;
	bool MatchesString(String const &) const;

private:
	// private constants
	static const long COMBINATION_CNT_MIN = 4L;
	static const long COMBINATION_CNT_MAX = 59049L;

	// private data
	AValueType *mpArray;     // array of attributes
	TileIdType   mId;        // to distinguish clones
	unsigned     mBonusValue;

	static AIndexType   msAttributeCnt;  // number of attributes per tile
	static double       msBonusProbability;
	static TileIdType   msNextId;
	static AValueType *mspValueMax;      // max value for each tile attribute

	// misc private methods
	static AValueType   CharToAttribute(ADisplayType, char);
	static TileIdType   NextId(void);
};
#endif // !defined(TILE_HPP_INCLUDED)
