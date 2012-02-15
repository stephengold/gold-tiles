// File:    tile.cpp
// Purpose: Tile class
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

#include <iostream> // std::cout
#include "strings.hpp"
#include "tiles.hpp"

// static data

ACountType  Tile:: msAttributeCnt = 0;
TileIdType  Tile:: msNextId = ID_FIRST;
AValueType *Tile::mspValueMax = NULL;


// lifecycle

Tile::Tile(void) {
    ASSERT(msAttributeCnt >= 2);

    mpArray = new AValueType[msAttributeCnt];

    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        mpArray[i_attr] = 0;
    }
    mId = ID_DEFAULT;
}

// Mint a new tile based on a string.
Tile::Tile(String const &str) {
    ASSERT(msAttributeCnt >= 2);

    mpArray = new AValueType[msAttributeCnt];

    AIndexType i_attr = 0;

    String::const_iterator c;
    for (c = str.begin() ; c != str.end(); c++) {
        if (i_attr < msAttributeCnt) {
            char ch = *c;
            mpArray[i_attr] = char_to_attribute(i_attr, ch);
            i_attr++;
        }
    }

    while (i_attr < msAttributeCnt) {
        mpArray[i_attr] = 0;
        i_attr++;
    }
    
    mId = NextId();

    ASSERT(IsValid());
}

// construct a copy (with the same id)
Tile::Tile(Tile const &rBase) {
    ASSERT(msAttributeCnt >= 2);

    mpArray = new AValueType[msAttributeCnt];
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        mpArray[i_attr] = rBase.mpArray[i_attr];
    }
    mId = rBase.mId;
}

Tile::~Tile(void) {
    delete[] mpArray;
	mpArray = NULL;
}


// operators

bool Tile::operator<(Tile const &rOther) const {
    ASSERT(IsValid());
    ASSERT(rOther.IsValid());

	bool result = (mId < rOther.mId);
     
     return result;
}

Tile &Tile::operator=(Tile const &rOther) {
    ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    mpArray = new AValueType[msAttributeCnt];
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AValueType value = rOther.mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        mpArray[i_attr] = value;
    }
    mId = rOther.mId;
     
    return *this;
}

bool Tile::operator==(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool result = (mId == rOther.mId);

	ASSERT(!result || CountMatchingAttributes(rOther) == msAttributeCnt);

    return result;
}

Tile::operator String(void) const {
	ASSERT(IsValid());

	String result;

    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AValueType value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);

        result += attribute_to_string(i_attr, value);
    }

    ASSERT(result.Length() == msAttributeCnt);
    return result;
}


// misc methods

AValueType Tile::Attribute(AIndexType ind) const {
	ASSERT(IsValid());
    ASSERT(ind < msAttributeCnt);

    AValueType result = mpArray[ind];
    
    return result;
}

/* static */ ACountType Tile::AttributeCnt(void) {
    ASSERT(msAttributeCnt >= 2);
    ACountType result = msAttributeCnt;

    return result;
}

// create a clone (with a different id)
Tile Tile::Clone(void) const {
	ASSERT(IsValid());

	Tile result = Tile(*this);
    result.mId = NextId();

	ASSERT(result.IsValid());
    return result;
}

unsigned Tile::CommonAttribute(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());
    ASSERT(CountMatchingAttributes(rOther) == 1);
    
    AIndexType result;
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        if (mpArray[i_attr] == rOther.mpArray[i_attr]) {
            result = i_attr;
            break;
        }
    }

    return result;
}

ACountType Tile::CountMatchingAttributes(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	unsigned result = 0;
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        if (HasAttribute(i_attr, rOther.mpArray[i_attr])) {
             ++result;
        }
    }
    
    return result;
}

void Tile::Display(void) const {
	ASSERT(IsValid());

    std::cout << " [" << String(*this) << "]";
}

// display an empty cell
/* static */ void Tile::DisplayEmpty(void) {
    std::cout << " ." << StringEmpty() << ".";
}

String Tile::GetUserChoice(Tiles const &availableTiles, Strings const &alts) {
	String result;

    while (true) {
        std::cout << "Enter a tile name";
    	Strings::ConstIteratorType alt;
		for (alt = alts.Begin(); alt != alts.End(); alt++) {
			std::cout << " or '" << *alt << "'";
		}
		std::cout << ": ";

        std::cin >> result;
		if (alts.Contains(result)) {
			break;
		}

        *this = Tile(result);
        ASSERT(IsValid());
        if (result != String(*this)) {
           std::cout << "'" << result << "' is invalid." << std::endl;
        } else if (!IsCloneAny(availableTiles)) {
            std::cout << result << " is unavailable." << std::endl;
        } else {
            availableTiles.UnClone(*this);
            break;
        }
    }

	return result;
}

TileIdType Tile::Id(void) const {
    TileIdType result = mId;

    return result;
}

/* static */ TileIdType Tile::NextId(void) {
    TileIdType result = msNextId;
	++msNextId;
    ASSERT(msNextId < ID_LAST);

	return result;
}

void Tile::SetAttribute(AIndexType ind, AValueType value) {
    ASSERT(ind < msAttributeCnt);
    ASSERT(value <= mspValueMax[ind]);
    mpArray[ind] = value;
	ASSERT(IsValid());
}

/* static */ void Tile::SetStatic(ACountType aCnt, AValueType const pValueMax[]) {
    ASSERT(aCnt >= 2);
#ifdef _GUI
    ASSERT(aCnt <= 5);
#endif
    msAttributeCnt = aCnt;

	delete[] mspValueMax;
    mspValueMax = new AValueType[aCnt];
    for (AIndexType i_attr = 0; i_attr < aCnt; i_attr++) {
        ASSERT(pValueMax[i_attr] >= 3);
        ASSERT(pValueMax[i_attr] <= 8);
        mspValueMax[i_attr] = pValueMax[i_attr];
    }
}

/* static */ String Tile::StringEmpty(void) {
    String result(msAttributeCnt, '.');

    return result;
}

/* static */ AValueType Tile::ValueMax(AIndexType attr) {
    ASSERT(attr < msAttributeCnt);
    AValueType result = mspValueMax[attr];
    
    return result;
}


// inquiry methods

bool Tile::HasAttribute(AIndexType ind, AValueType value) const {
    ASSERT(ind < msAttributeCnt);
    AValueType attrib = mpArray[ind];
    bool result = (attrib == value);
    
    return result;
}

bool Tile::HasId(TileIdType id) const {
	bool result = (mId == id);

	return result;
}

bool Tile::IsClone(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    bool result = false;
    
    if (mId != rOther.mId) {
		ACountType cnt = CountMatchingAttributes(rOther);
        result = (cnt == msAttributeCnt);
    }

    return result;
}

bool Tile::IsCloneAny(Tiles const &s) const {
    bool result = s.ContainsClone(*this);

	return result;
}

bool Tile::IsCompatibleWith(Tile const *other) const {
    bool result = true;
    
    if (other != NULL) {
        ACountType matches = CountMatchingAttributes(*other);
        result = (matches == 1);
    }
    
    return result;
}

bool Tile::IsValid(void) const {
    bool result = false;
    
    if (mpArray != NULL
	 && (mId == ID_DEFAULT || mId >= ID_FIRST && mId < msNextId))
	{
        result = true;
    
        for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
            if (mpArray[i_attr] > mspValueMax[i_attr]) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}


// attribute utility functions

String attribute_to_string(AIndexType ind, AValueType value) {
    ASSERT(ind < Tile::AttributeCnt());
    ASSERT(value <= Tile::ValueMax(ind));
    char ch;

    switch (ind) {
        case 0:
            ch = 'A' + value;
            break;
        case 1:
            ch = 'R' + value;
            break;
        case 2:
            ch = 'a' + value;
            break;
        case 3:
            ch = 'r' + value;
            break;
        default:
            ch = '1' + value;
            break;
    }

    String result(ch);
	ASSERT(result.Length() == 1);

	return result;
}

AValueType char_to_attribute(AIndexType ind, char ch) {
    ASSERT(ind < Tile::AttributeCnt());

    AValueType result;
    switch (ind) {
        case 0:
            result = (AValueType)(ch - 'A');
            break;
        case 1:
            result = (AValueType)(ch - 'R');
            break;
        case 2:
            result = (AValueType)(ch - 'a');
            break;
        case 3:
            result = (AValueType)(ch - 'r');
            break;
        default:
            result = (AValueType)(ch - '1');
            break;
    }

    if (result > Tile::ValueMax(ind)) {
        result = 0;
    }
    
    return result;
}
