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

ACountType  Tile:: msAttributeCnt = 0;    // configured by SetStatic()
double      Tile:: msBonusProbability = 0.0; // configured by SetStatic()
TileIdType  Tile:: msNextId = ID_FIRST;
AValueType *Tile::mspValueMax = NULL;     // allocated by SetStatic()


// lifecycle

Tile::Tile(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AValueType[msAttributeCnt];
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        mpArray[i_attr] = 0;
    }
	mBonusValue = 0;
    mId = ID_DEFAULT; // an ID used only by the default constructor
}

// Mint a new tile based on a string.
Tile::Tile(String const &rString) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AValueType[msAttributeCnt];
	mBonusValue = 0;

    AIndexType i_attr = 0;
    String::ConstIterator i_char;
    for (i_char = rString.begin() ; i_char != rString.end(); i_char++) {
        char const ch = *i_char;
        if (i_attr < msAttributeCnt) {
			ADisplayType const display_mode = DefaultDisplayMode(i_attr);
			AValueType value = CharToAttribute(display_mode, ch);
			if (value > ValueMax(i_attr)) {
				value = 0; // so resulting object can be valid
			}
            mpArray[i_attr] = value;
            i_attr++;
        } else if (i_attr == msAttributeCnt && ch == '+') {
            mBonusValue = 1;
		}
    }

    while (i_attr < msAttributeCnt) {
		// not enough characters in the string -- pad the attribute array with zeroes
        mpArray[i_attr] = 0;
        i_attr++;
    }
    
    mId = NextId();

    ASSERT(IsValid());
}

// construct a copy (with the same id)
Tile::Tile(Tile const &rBase) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AValueType[msAttributeCnt];
	ASSERT(mpArray != NULL);
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AValueType const value = rBase.mpArray[i_attr];
        mpArray[i_attr] = value;
    }
	mBonusValue = rBase.mBonusValue;
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

	bool const result = (mId < rOther.mId);
     
    return result;
}

Tile &Tile::operator=(Tile const &rOther) {
    ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    mpArray = new AValueType[msAttributeCnt];
	ASSERT(mpArray != NULL);
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AValueType const value = rOther.mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        mpArray[i_attr] = value;
    }
	mBonusValue = rOther.mBonusValue;
    mId = rOther.mId;
     
    return *this;
}

bool Tile::operator==(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool const result = (mId == rOther.mId);

	ASSERT(!result || CountMatchingAttributes(rOther) == msAttributeCnt);

    return result;
}

Tile::operator String(void) const {
	ASSERT(IsValid());

	String result;

    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AValueType const value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);

		ADisplayType const display_mode = DefaultDisplayMode(i_attr);
        result += AttributeToString(display_mode, value);
    }
	if (mBonusValue > 0) {
		result += '+';
	} else {
		result += ' ';
	}

	// result length should match that of the StringEmpty() method
	ASSERT(result.Length() == unsigned(msAttributeCnt + 1));

	return result;
}


// misc methods

AValueType Tile::Attribute(AIndexType ind) const {
	ASSERT(IsValid());
    ASSERT(ind < msAttributeCnt);

    AValueType const result = mpArray[ind];
    
    return result;
}

/* static */ ACountType Tile::AttributeCnt(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

	return msAttributeCnt;
}

/* static */ String Tile::AttributeReport(void) {
	String result = ::plural(Tile::AttributeCnt(), "attribute") + ":\n";
	for (AIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		ADisplayType const display_mode = Tile::DefaultDisplayMode(i_attr);
		AValueType const value_max = Tile::ValueMax(i_attr);
		result += " " + ::ordinal(i_attr + 1) + " attribute ranges from ";
		result += Tile::AttributeToString(display_mode, 0) + " to ";
		result += Tile::AttributeToString(display_mode, value_max) + "\n";
	}
	result += "\n";

	return result;
}

/* static */ String Tile::AttributeToString(ADisplayType display_mode, AValueType value) {
    char ch = '?'; // invalid

    switch (display_mode) {
        case ADISPLAY_ABC:
            ch = char('A' + value);
            break;
        case ADISPLAY_RST:
            ch = char('R' + value);
            break;
        case ADISPLAY_123:
            ch = char('1' + value);
            break;
		default:
			FAIL();
    }

    String const result(ch);
	ASSERT(result.Length() == 1);

	return result;
}

/* static */ double Tile::BonusProbability(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

	return msBonusProbability;
}

/* static */ AValueType Tile::CharToAttribute(ADisplayType display_mode, char ch) {
    AValueType result = 0;

    switch (display_mode) {
        case ADISPLAY_ABC:
            result = AValueType(ch - 'A');
            break;
        case ADISPLAY_RST:
            result = AValueType(ch - 'R');
            break;
        case ADISPLAY_123:
            result = AValueType(ch - '1');
            break;
		default:
			FAIL();
    }

	if (result >= VALUE_CNT_MAX) {
		result = 0;
	}

    return result;
}

// create a clone (with a new id) and randomize its bonus value
Tile Tile::CloneAndSetBonus(void) const {
	ASSERT(IsValid());

	Tile result(*this);
    result.mId = NextId();

	// randomize bonus value
	bool const gets_bonus = ::random_bool(msBonusProbability);
	result.mBonusValue = gets_bonus ? 1 : 0;

	ASSERT(result.IsValid());
    return result;
}

// calculate the number of possible combinations of attributes
/* static */ long Tile::CombinationCnt(void) {
	long result = 1L;
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
		ASSERT(i_attr < Tile::ATTRIBUTE_CNT_MAX);
		AValueType const max_value = mspValueMax[i_attr];
		AValueType const possible_values = max_value + 1;  // zero is a possible value
		result *= possible_values;
	}

	ASSERT(result >= COMBINATION_CNT_MIN);
	ASSERT(result <= COMBINATION_CNT_MAX);
	return result;
}

// identify the common attribute of a compatible tile
AIndexType Tile::CommonAttribute(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());
    ASSERT(CountMatchingAttributes(rOther) == 1);
    
    AIndexType result = msAttributeCnt;
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        if (mpArray[i_attr] == rOther.mpArray[i_attr]) {
            result = i_attr;
            break;
        }
    }

	ASSERT(result < msAttributeCnt);
    return result;
}

ACountType Tile::CountMatchingAttributes(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	ACountType result = 0;
    for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
		AValueType const attr_value = rOther.mpArray[i_attr];
        if (HasAttribute(i_attr, attr_value)) {
             ++result;
        }
    }
    
    return result;
}

/* static */ ADisplayType Tile::DefaultDisplayMode(AIndexType ind) {
	ADisplayType result = ADISPLAY_123;

	switch (ind) {
#ifdef _GUI
	    case 0:
			result = ADISPLAY_SHAPE;
			break;
		case 1:
			result = ADISPLAY_COLOR;
			break;
		case 2:
			result = ADISPLAY_ABC;
			break;
		case 3:
			result = ADISPLAY_RST;
			break;
		case 4:
			result = ADISPLAY_123;
			break;
#else // !defined(_GUI)
	    case 0:
			result = ADISPLAY_ABC;
			break;
		case 1:
			result = ADISPLAY_RST;
			break;
#endif // !defined(_GUI)
		default:
			result = ADISPLAY_123;
			break;
	}

	return result;
}

void Tile::Display(void) const {
	ASSERT(IsValid());

	String const str = String(*this);

    std::cout << " [" << str << "]";
}

// display an empty cell
/* static */ void Tile::DisplayEmpty(void) {
    std::cout << " ." << StringEmpty() << ".";
}

String Tile::GetUserChoice(Tiles const &rAvailableTiles, Strings const &rAlternatives) {
	String result;

    for (;;) {
        std::cout << "Enter a tile name";
    	Strings::ConstIterator i_alt;
		for (i_alt = rAlternatives.Begin(); i_alt != rAlternatives.End(); i_alt++) {
			std::cout << " or " << i_alt->Quote();
		}
		std::cout << ": ";

        std::cin >> result;
		if (rAlternatives.Contains(result)) {
			break;
		}

        *this = Tile(result);
        ASSERT(IsValid());
        if (!MatchesString(result)) {
           std::cout << result.Quote() << " is invalid." << std::endl;
        } else if (!IsCloneAny(rAvailableTiles)) {
            std::cout << result << " is unavailable." << std::endl;
        } else {
            rAvailableTiles.UnClone(*this);
            break;
        }
    }

	return result;
}

TileIdType Tile::Id(void) const {
    return mId;
}

/* static */ TileIdType Tile::NextId(void) {
    TileIdType const result = msNextId;

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

/* static */ void Tile::SetStatic(
	ACountType attributeCnt,
	AValueType const pValueMax[],
	double bonusProbability)
{
    ASSERT(attributeCnt >= ATTRIBUTE_CNT_MIN);
#ifdef _GUI
    ASSERT(attributeCnt <= ATTRIBUTE_CNT_MAX);
#endif
    ASSERT(bonusProbability >= 0.0);
    ASSERT(bonusProbability <= 1.0);

	msAttributeCnt = attributeCnt;
	msBonusProbability = bonusProbability;

	delete[] mspValueMax;
    mspValueMax = new AValueType[attributeCnt];
	ASSERT(mspValueMax != NULL);

    for (AIndexType i_attr = 0; i_attr < attributeCnt; i_attr++) {
		unsigned const value_cnt = pValueMax[i_attr] + 1;
        ASSERT(value_cnt >= VALUE_CNT_MIN);
        ASSERT(value_cnt <= VALUE_CNT_MAX);
        mspValueMax[i_attr] = pValueMax[i_attr];
    }
}

/* static */ String Tile::StringEmpty(void) {
    String const result(msAttributeCnt + 1, '.');

	// result length should agree with String() operator
	ASSERT(result.Length() == unsigned(msAttributeCnt + 1));

    return result;
}

/* static */ AValueType Tile::ValueCnt(AIndexType attrIndex) {
    AValueType const result = ValueMax(attrIndex) + 1;
    
    return result;
}

/* static */ AValueType Tile::ValueMax(AIndexType attrIndex) {
    ASSERT(attrIndex < msAttributeCnt);
    AValueType const result = mspValueMax[attrIndex];
    
    return result;
}


// inquiry methods

bool Tile::HasAttribute(AIndexType ind, AValueType value) const {
    ASSERT(ind < msAttributeCnt);

    AValueType const attrib = mpArray[ind];
    bool const result = (attrib == value);
    
    return result;
}

bool Tile::HasBonus(void) const {
	bool const result = (mBonusValue > 0);

	return result;
}

bool Tile::HasId(TileIdType id) const {
	bool const result = (mId == id);

	return result;
}

bool Tile::IsClone(Tile const &rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    bool result = false;

	// copies (with the same ID) do not count as clones
    if (mId != rOther.mId) {
		// the bonus value and all attributes match
		ACountType const cnt = CountMatchingAttributes(rOther);
        result = (cnt == msAttributeCnt && mBonusValue == rOther.mBonusValue);
    }

    return result;
}

bool Tile::IsCloneAny(Tiles const &rSet) const {
    bool const result = rSet.ContainsClone(*this);

	return result;
}

bool Tile::IsCompatibleWith(Tile const *pOther) const {
    bool result = true;
    
    if (pOther != NULL) {
        ACountType const matchCnt = CountMatchingAttributes(*pOther);
        result = (matchCnt == 1);
    }
    
    return result;
}

bool Tile::IsValid(void) const {
    bool result = false;
    
    if (mpArray != NULL
	 && (mBonusValue == 0 || mBonusValue == 1)
	 && (mId == ID_DEFAULT || mId >= ID_FIRST && mId < msNextId))
	{
        result = true;
#if 0
        for (AIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
            if (mpArray[i_attr] > mspValueMax[i_attr]) {
                result = false;
                break;
            }
        }
#endif
    }
    
    return result;
}

bool Tile::MatchesString(String const &rMatch) const {
	String string_form = String(*this);
	string_form = string_form.Purge();
	String const match = rMatch.Purge();

	bool result = (string_form == match);

	return result;
}


