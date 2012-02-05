#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

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
typedef unsigned TileIdType;

// attribute types and utilities
typedef unsigned ACountType, AIndexType;
typedef unsigned AValueType;

String     attributeToString(AIndexType, AValueType);
AValueType charToAttribute(AIndexType, char ch);

class Tile {
public:
	// lifecycle
	Tile(void);          // construct a blank
	Tile(String const &);
	Tile(Tile const &);  // copy a tile
	~Tile(void);

    // operators
	bool   operator<(Tile const &) const;
	Tile & operator=(Tile const &);
	bool   operator==(Tile const &) const;
	operator String(void) const;

	// misc
	Tile          Clone(void) const;
	unsigned      CommonAttribute(Tile const &) const;
	ACountType    CountMatchingAttributes(Tile const &) const;
	void          Display(void) const;
    static void   DisplayEmpty(void);
	String        GetUserChoice(Tiles const &, Strings const &);
    static String StringEmpty(void);

    // access
	AValueType        Attribute(AIndexType) const;
    static ACountType AttributeCnt(void);
	TileIdType        Id(void) const;
	void              SetAttribute(AIndexType, AValueType);
	static void       SetStatic(ACountType, AValueType const pValueMax[]);
    static AValueType ValueMax(AIndexType);

	// inquiry
	bool HasAttribute(AIndexType, AValueType) const;
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
bool isValid(const Tile *);

#endif
