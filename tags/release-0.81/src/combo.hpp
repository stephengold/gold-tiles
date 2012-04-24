#ifndef COMBO_HPP_INCLUDED
#define COMBO_HPP_INCLUDED

// File:     combo.hpp
// Location: src
// Purpose:  declare Combo class
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
A Combo object represents a combination of tile attributes.
Each combo has at least two attributes, each of which can assume up
to nine values.  In the context of a particular game, every combo has 
the same number of attributes.
*/

#include "project.hpp"  // USES String

// attribute types
typedef unsigned short AttrCntType;   // an attribute count
typedef unsigned short AttrIndexType; // an index into an array of attributes
typedef unsigned short AttrType;      // an attribute value

enum AttrModeType {                   // display mode type
	ATTR_MODE_MIN = 0,
	ATTR_MODE_ABC = 0,
	ATTR_MODE_RST = 1,
	ATTR_MODE_123 = 2,
#ifdef _GUI
	ATTR_MODE_COLOR = 3,
	ATTR_MODE_SHAPE = 4,
	ATTR_MODE_MAX = 4,
	ATTR_MODE_CNT = 5
#else // !defined(_GUI)
	ATTR_MODE_MAX = 2,
	ATTR_MODE_CNT = 3
#endif // !defined(_GUI)
};

class Combo {
public:
	// public constants
	static const AttrCntType ATTRIBUTE_CNT_MIN = 2;
	static const AttrCntType ATTRIBUTE_CNT_DEFAULT = 2;
#ifdef _GUI
	static const AttrCntType ATTRIBUTE_CNT_MAX = 5;
#endif // defined(_GUI)

	static const AttrType VALUE_CNT_MIN = 2;
	static const AttrType VALUE_CNT_DEFAULT = 6;
	static const AttrType VALUE_CNT_MAX = 9;

	static const long COMBINATION_CNT_MIN = 4L;
#ifdef _GUI
	static const long COMBINATION_CNT_MAX = 59049L; // 9^5
#endif // defined(_GUI)

    // public lifecycle
	Combo(void);
	Combo(String const&);
	Combo(Combo const&);
	~Combo(void);

    // public operators
	Combo&   operator=(Combo const&);
	bool     operator==(Combo const&) const;
	operator String(void) const;

	// misc public methods
	AttrType            Attribute(AttrIndexType) const;
    static AttrCntType  AttributeCnt(void);
	static String       AttributeToString(AttrModeType, AttrType);
	static long         CombinationCnt(void);
	AttrIndexType       CommonAttribute(Combo const&) const;
	AttrCntType         CountMatchingAttributes(Combo const&) const;
	static AttrModeType DefaultDisplayMode(AttrIndexType);
	void                SetAttribute(AttrIndexType, AttrType);
	static void         SetStatic(GameOpt const&);
    static AttrType     ValueCnt(AttrIndexType);
    static AttrType     ValueMax(AttrIndexType);

	// public inquiry methods
	bool HasAttribute(AttrIndexType, AttrType) const;
	bool IsCompatibleWith(Combo const&) const;

private:
	// private data
	AttrType* mpArray;     // array of attributes

	static AttrIndexType msAttributeCnt;  // number of attributes per tile
	static AttrType*    mspValueMax;     // max value for each tile attribute

	// misc private methods
	static AttrType   CharToAttribute(AttrModeType, char);
};
#endif // !defined(COMBO_HPP_INCLUDED)
