// File:     tile.cpp
// Location: src
// Purpose:  implement Tile class
// Author:   Stephen Gold sgold@sonic.net
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
#include "gameopt.hpp"
#include "strings.hpp"
#include "tiles.hpp"

// static data

AttrCntType Tile:: msAttributeCnt = 0;    // configured by SetStatic()
double      Tile:: msBonusProbability = 0.0; // configured by SetStatic()
TileIdType  Tile:: msNextId = ID_FIRST;
AttrType *  Tile::mspValueMax = NULL;     // allocated by SetStatic()


// lifecycle

Tile::Tile(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        mpArray[i_attr] = 0;
    }
	mBonusValue = 0;
    mId = ID_DEFAULT; // an ID used only by the default constructor
}

// Mint a new tile based on a string.
Tile::Tile(String const& rString) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
	mBonusValue = 0;

    AttrIndexType i_attr = 0;
    String::ConstIterator i_char;
    for (i_char = rString.begin() ; i_char != rString.end(); i_char++) {
        char const ch = *i_char;
        if (i_attr < msAttributeCnt) {
			AttrModeType const display_mode = DefaultDisplayMode(i_attr);
			AttrType value = CharToAttribute(display_mode, ch);
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
Tile::Tile(Tile const& rBase) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = rBase.mpArray[i_attr];
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

bool Tile::operator<(Tile const& rOther) const {
    ASSERT(IsValid());
    ASSERT(rOther.IsValid());

	bool const result = (mId < rOther.mId);
     
    return result;
}

Tile& Tile::operator=(Tile const& rOther) {
    ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = rOther.mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        mpArray[i_attr] = value;
    }
	mBonusValue = rOther.mBonusValue;
    mId = rOther.mId;
     
    return *this;
}

bool Tile::operator==(Tile const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool const result = (mId == rOther.mId);

	ASSERT(!result || CountMatchingAttributes(rOther) == msAttributeCnt);

    return result;
}

Tile::operator String(void) const {
	ASSERT(IsValid());

	String result;

    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);

		AttrModeType const display_mode = DefaultDisplayMode(i_attr);
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

AttrType Tile::Attribute(AttrIndexType ind) const {
	ASSERT(IsValid());
    ASSERT(ind < msAttributeCnt);

    AttrType const result = mpArray[ind];
    
    return result;
}

/* static */ AttrCntType Tile::AttributeCnt(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

	return msAttributeCnt;
}

/* static */ String Tile::AttributeToString(AttrModeType display_mode, AttrType value) {
    char ch = '?'; // invalid

    switch (display_mode) {
        case ATTR_MODE_ABC:
            ch = char('A' + value);
            break;
        case ATTR_MODE_RST:
            ch = char('R' + value);
            break;
        case ATTR_MODE_123:
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

/* static */ AttrType Tile::CharToAttribute(AttrModeType display_mode, char ch) {
    AttrType result = 0;

    switch (display_mode) {
        case ATTR_MODE_ABC:
            result = AttrType(ch - 'A');
            break;
        case ATTR_MODE_RST:
            result = AttrType(ch - 'R');
            break;
        case ATTR_MODE_123:
            result = AttrType(ch - '1');
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
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

	long result = 1L;
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
		AttrType const max_value = mspValueMax[i_attr];
		AttrType const possible_values = max_value + 1;  // zero is a possible value
		result *= possible_values;
	}

	ASSERT(result >= COMBINATION_CNT_MIN);
#ifdef _GUI
	ASSERT(result <= COMBINATION_CNT_MAX);
#endif // defined(_GUI)
	return result;
}

// identify the common attribute of a compatible tile
AttrIndexType Tile::CommonAttribute(Tile const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());
    ASSERT(CountMatchingAttributes(rOther) == 1);
    
    AttrIndexType result = msAttributeCnt;
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        if (mpArray[i_attr] == rOther.mpArray[i_attr]) {
            result = i_attr;
            break;
        }
    }

	ASSERT(result < msAttributeCnt);
    return result;
}

AttrCntType Tile::CountMatchingAttributes(Tile const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	AttrCntType result = 0;
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
		AttrType const attr_value = rOther.mpArray[i_attr];
        if (HasAttribute(i_attr, attr_value)) {
             ++result;
        }
    }
    
    return result;
}

/* static */ AttrModeType Tile::DefaultDisplayMode(AttrIndexType ind) {
	AttrModeType result = ATTR_MODE_123;

	switch (ind) {
#ifdef _GUI
	    case 0:
			result = ATTR_MODE_SHAPE;
			break;
		case 1:
			result = ATTR_MODE_COLOR;
			break;
		case 2:
			result = ATTR_MODE_ABC;
			break;
		case 3:
			result = ATTR_MODE_RST;
			break;
		case 4:
			result = ATTR_MODE_123;
			break;
#else // !defined(_GUI)
	    case 0:
			result = ATTR_MODE_ABC;
			break;
		case 1:
			result = ATTR_MODE_RST;
			break;
#endif // !defined(_GUI)
		default:
			result = ATTR_MODE_123;
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

String Tile::GetUserChoice(Tiles const& rAvailableTiles, Strings const& rAlternatives) {
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

void Tile::SetAttribute(AttrIndexType ind, AttrType value) {
    ASSERT(ind < msAttributeCnt);
    ASSERT(value <= mspValueMax[ind]);

    mpArray[ind] = value;

	ASSERT(IsValid());
}

/* static */ void Tile::SetStatic(GameOpt const& rGameOpt) {
	AttrCntType const attr_cnt = rGameOpt.AttrCnt();
	double const bonus_probability = rGameOpt.BonusPercent()/100.0;

    ASSERT(attr_cnt >= ATTRIBUTE_CNT_MIN);
#ifdef _GUI
    ASSERT(attr_cnt <= ATTRIBUTE_CNT_MAX);
#endif
    ASSERT(bonus_probability >= 0.0);
    ASSERT(bonus_probability <= 1.0);

	msAttributeCnt = attr_cnt;
	msBonusProbability = bonus_probability;

	delete[] mspValueMax;
    mspValueMax = new AttrType[attr_cnt];
	ASSERT(mspValueMax != NULL);

    for (AttrIndexType i_attr = 0; i_attr < attr_cnt; i_attr++) {
		AttrType const value_cnt = rGameOpt.CountAttrValues(i_attr);
        ASSERT(value_cnt >= VALUE_CNT_MIN);
        ASSERT(value_cnt <= VALUE_CNT_MAX);
        mspValueMax[i_attr] = value_cnt - 1;
    }
}

/* static */ String Tile::StringEmpty(void) {
    String const result(msAttributeCnt + 1, '.'); // +1 for bonus indication

	// result length should agree with String() operator
	ASSERT(result.Length() == unsigned(msAttributeCnt + 1));

    return result;
}

/* static */ AttrType Tile::ValueCnt(AttrIndexType attrIndex) {
    AttrType const result = ValueMax(attrIndex) + 1;
    
    return result;
}

/* static */ AttrType Tile::ValueMax(AttrIndexType attrIndex) {
    ASSERT(attrIndex < msAttributeCnt);
    AttrType const result = mspValueMax[attrIndex];
    
    return result;
}


// inquiry methods

bool Tile::HasAttribute(AttrIndexType ind, AttrType value) const {
    ASSERT(ind < msAttributeCnt);

    AttrType const attrib = mpArray[ind];
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

bool Tile::IsClone(Tile const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

    bool result = false;

	// copies (with the same ID) do not count as clones
    if (mId != rOther.mId) {
		// the bonus value and all attributes match
		AttrCntType const cnt = CountMatchingAttributes(rOther);
        result = (cnt == msAttributeCnt && mBonusValue == rOther.mBonusValue);
    }

    return result;
}

bool Tile::IsCloneAny(Tiles const& rSet) const {
    bool const result = rSet.ContainsClone(*this);

	return result;
}

bool Tile::IsCompatibleWith(Tile const* pOther) const {
    bool result = true;
    
    if (pOther != NULL) {
        AttrCntType const matchCnt = CountMatchingAttributes(*pOther);
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
        for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
            if (mpArray[i_attr] > mspValueMax[i_attr]) {
                result = false;
                break;
            }
        }
#endif
    }
    
    return result;
}

bool Tile::MatchesString(String const& rMatch) const {
	String string_form = String(*this);
	string_form = string_form.Purge();
	String const match = rMatch.Purge();

	bool const result = (string_form == match);

	return result;
}


