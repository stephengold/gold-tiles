// File:     combo.cpp
// Location: src
// Purpose:  implement Combo class
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

#include "combo.hpp"
#include "gameopt.hpp"


// static data

AttrCntType Combo:: msAttributeCnt = 0;    // configured by SetStatic()
AttrType*   Combo::mspValueMax = NULL;     // allocated by SetStatic()


// lifecycle

Combo::Combo(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);

	for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        mpArray[i_attr] = 0;
    }
}

// new Combo from String
Combo::Combo(String const& rString) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);

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
		}
    }

    while (i_attr < msAttributeCnt) {
		// not enough characters in the string -- pad the attribute array with zeroes
        mpArray[i_attr] = 0;
        i_attr++;
    }
}

// construct a copy
Combo::Combo(Combo const& rBase) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);

    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = rBase.mpArray[i_attr];
        mpArray[i_attr] = value;
    }
}

Combo::~Combo(void) {
    delete[] mpArray;
	mpArray = NULL;
}


// operators

Combo& Combo::operator=(Combo const& rOther) {
    mpArray = new AttrType[msAttributeCnt];
	ASSERT(mpArray != NULL);
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = rOther.mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        mpArray[i_attr] = value;
    }
     
    return *this;
}

bool Combo::operator==(Combo const& rOther) const {
	bool const result = (CountMatchingAttributes(rOther) == msAttributeCnt);

    return result;
}

Combo::operator String(void) const {
	String result;

    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
        AttrType const value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);

		AttrModeType const display_mode = DefaultDisplayMode(i_attr);
        result += AttributeToString(display_mode, value);
    }

	return result;
}


// misc methods

AttrType Combo::Attribute(AttrIndexType ind) const {
    ASSERT(ind < msAttributeCnt);

    AttrType const result = mpArray[ind];
    
    return result;
}

/* static */ AttrCntType Combo::AttributeCnt(void) {
    ASSERT(msAttributeCnt >= ATTRIBUTE_CNT_MIN);

	return msAttributeCnt;
}

/* static */ String Combo::AttributeToString(AttrModeType displayMode, AttrType value) {
    char ch = '?'; // invalid

    switch (displayMode) {
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

/* static */ AttrType Combo::CharToAttribute(AttrModeType display_mode, char ch) {
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

// calculate the number of possible combinations of attributes
/* static */ long Combo::CombinationCnt(void) {
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

// identify the common attribute of a compatible combo
AttrIndexType Combo::CommonAttribute(Combo const& rOther) const {
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

AttrCntType Combo::CountMatchingAttributes(Combo const& rOther) const {
	AttrCntType result = 0;
    for (AttrIndexType i_attr = 0; i_attr < msAttributeCnt; i_attr++) {
		AttrType const attr_value = rOther.mpArray[i_attr];
        if (HasAttribute(i_attr, attr_value)) {
             ++result;
        }
    }
    
    return result;
}

/* static */ AttrModeType Combo::DefaultDisplayMode(AttrIndexType ind) {
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

void Combo::SetAttribute(AttrIndexType ind, AttrType value) {
    ASSERT(ind < msAttributeCnt);
    ASSERT(value <= mspValueMax[ind]);

    mpArray[ind] = value;
}

/* static */ void Combo::SetStatic(GameOpt const& rGameOpt) {
	AttrCntType const attr_cnt = rGameOpt.AttrCnt();

    ASSERT(attr_cnt >= ATTRIBUTE_CNT_MIN);
#ifdef _GUI
    ASSERT(attr_cnt <= ATTRIBUTE_CNT_MAX);
#endif

	msAttributeCnt = attr_cnt;

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

/* static */ AttrType Combo::ValueCnt(AttrIndexType attrIndex) {
    AttrType const result = ValueMax(attrIndex) + 1;
    
    return result;
}

/* static */ AttrType Combo::ValueMax(AttrIndexType attrIndex) {
    ASSERT(attrIndex < msAttributeCnt);
    AttrType const result = mspValueMax[attrIndex];
    
    return result;
}


// inquiry methods

bool Combo::HasAttribute(AttrIndexType index, AttrType value) const {
    ASSERT(index < msAttributeCnt);

    AttrType const attrib = mpArray[index];
    bool const result = (attrib == value);
    
    return result;
}

bool Combo::IsCompatibleWith(Combo const& rOther) const {
    AttrCntType const matchCnt = CountMatchingAttributes(rOther);
    bool const result = (matchCnt == 1);

	return result;
}