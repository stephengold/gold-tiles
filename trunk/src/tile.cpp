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
#include "project.hpp"
#include "string.hpp"
#include "strings.hpp"
#include "tile.hpp"
#include "tiles.hpp"

// attribute utility functions

String attributeToString(AIndexType ind, AValueType value) {
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

AValueType charToAttribute(AIndexType ind, char ch) {
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

// static private data

ACountType  Tile:: msAttributeCnt = 0;
TileIdType  Tile:: msNextId = 2; // 0 is invalid, 1 is default tile
AValueType *Tile::mspValueMax = NULL;

// lifecycle

Tile::Tile(void) {
    ASSERT(msAttributeCnt >= 2);
    mpArray = new AValueType[msAttributeCnt];

    for (AIndexType i = 0; i < msAttributeCnt; i++) {
        mpArray[i] = 0;
    }
    mId = 1;
}

// Mint a new tile based on a string.
Tile::Tile(String const &str) {
    ASSERT(msAttributeCnt >= 2);
    mpArray = new AValueType[msAttributeCnt];

    AIndexType ind = 0;

    String::const_iterator c;
    for (c = str.begin() ; c != str.end(); c++) {
        if (ind < msAttributeCnt) {
            char ch = *c;
            mpArray[ind] = charToAttribute(ind, ch);
            ind++;
        }
    }

    while (ind < msAttributeCnt) {
        mpArray[ind] = 0;
        ind++;
    }
    
    mId = NextId();

    ASSERT(IsValid());
}

// construct a copy (with the same id)
Tile::Tile(Tile const &rBase) {
    ASSERT(msAttributeCnt >= 2);
    mpArray = new AValueType[msAttributeCnt];
    for (AIndexType i = 0; i < msAttributeCnt; i++) {
        mpArray[i] = rBase.mpArray[i];
    }
    mId = rBase.mId;
}

Tile::~Tile(void) {
    ASSERT(IsValid());
    delete[] mpArray;
	mpArray = NULL;
}


// operators

bool Tile::operator<(Tile const &rOther) const {
     bool result = (mId < rOther.mId);
     
     return result;
}

Tile &Tile::operator=(Tile const &rOther) {
	ASSERT(rOther.IsValid());

    mpArray = new AValueType[msAttributeCnt];
    for (AIndexType ind = 0; ind < msAttributeCnt; ind++) {
        AValueType value = rOther.mpArray[ind];
        ASSERT(value <= mspValueMax[ind]);
        mpArray[ind] = value;
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
    String result;

    for (AIndexType ind = 0; ind < msAttributeCnt; ind++) {
        AValueType value = mpArray[ind];
        ASSERT(value <= mspValueMax[ind]);

        result += attributeToString(ind, value);
    }
    ASSERT(result.Length() == msAttributeCnt);

    return result;
}


// misc

AValueType Tile::Attribute(AIndexType ind) const {
    ASSERT(ind < msAttributeCnt);
    AValueType result = mpArray[ind];
    
    return result;
}

ACountType Tile::AttributeCnt(void) {
    ASSERT(msAttributeCnt >= 2);
    ACountType result = msAttributeCnt;

    return result;
}

// create a clone (with a different id)
Tile Tile::Clone(void) const {
    Tile result = Tile(*this);
    result.mId = NextId();
	ASSERT(result.IsValid());

    return result;
}

unsigned Tile::CommonAttribute(Tile const &rOther) const {
    ASSERT(CountMatchingAttributes(rOther) == 1);
    
    AIndexType result;
    for (AIndexType ind = 0; ind < msAttributeCnt; ind++) {
        if (mpArray[ind] == rOther.mpArray[ind]) {
            result = ind;
            break;
        }
    }
    return result;
}

ACountType Tile::CountMatchingAttributes(Tile const &rOther) const {
    unsigned result = 0;
    for (AIndexType ind = 0; ind < msAttributeCnt; ind++) {
        if (HasAttribute(ind, rOther.mpArray[ind])) {
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
void Tile::DisplayEmpty(void) {
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
		if (alts.Find(result) != alts.End()) {
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
    ASSERT(msNextId < ULONG_MAX);

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
    ASSERT(aCnt <= 4);
#endif
    msAttributeCnt = aCnt;
    
    mspValueMax = new AValueType[aCnt];
    for (AIndexType i = 0; i < aCnt; i++) {
        ASSERT(pValueMax[i] >= 3);
        ASSERT(pValueMax[i] <= 8);
        mspValueMax[i] = pValueMax[i];
    }
}

String Tile::StringEmpty(void) {
    String result(msAttributeCnt, '.');

    return result;
}

AValueType Tile::ValueMax(AIndexType attr) {
    ASSERT(attr < msAttributeCnt);
    AValueType result = mspValueMax[attr];
    
    return result;
}

// inquiry

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

    D(std::cout << "Tile::IsClone(" << String(*this) << ", " << String(rOther)
        << ") returns " << result << std::endl);
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
        D(std::cout << "Tile::IsCompatibleWith(" << String(*this) << ", " << String(*other)
            << ") returns " << result << std::endl);
    }
    
    return result;
}

bool Tile::IsValid(void) const {
    bool result = false;
    
    if (mId >= 1 && mId < msNextId && mpArray != NULL) {
        result = true;
    
        for (AIndexType ind = 0; ind < msAttributeCnt; ind++) {
            if (mpArray[ind] > mspValueMax[ind]) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}
