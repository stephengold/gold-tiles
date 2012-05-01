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

AttrCntType Combo:: msAttrCnt = 0;     // configured by SetStatic()
AttrType*   Combo::mspValueMax = NULL; // allocated by SetStatic()


// lifecycle

Combo::Combo(void) {
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    mpArray = new AttrType[msAttrCnt];
    ASSERT(mpArray != NULL);

    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        mpArray[i_attr] = 0;
    }
}

// Construct a new Combo from a save/send string.
Combo::Combo(String const& rString) {
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    mpArray = new AttrType[msAttrCnt];
    ASSERT(mpArray != NULL);

    AttrIndexType i_attr = 0;
    String::ConstIterator i_char;
    for (i_char = rString.begin() ; i_char != rString.end(); i_char++) {
        char const ch = *i_char;
        if (i_attr < msAttrCnt) {
            AttrModeType const display_mode = ATTR_MODE_ABC;
            AttrType value = CharToAttr(display_mode, ch);
            if (value > ValueMax(i_attr)) {
                value = 0; // so resulting object can be valid
            }
            mpArray[i_attr] = value;
            i_attr++;
        }
    }

    while (i_attr < msAttrCnt) {
        // not enough characters in the string -- pad the attribute array with zeroes
        mpArray[i_attr] = 0;
        i_attr++;
    }
}

// construct a copy
Combo::Combo(Combo const& rBase) {
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    mpArray = new AttrType[msAttrCnt];
    ASSERT(mpArray != NULL);

    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
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
    mpArray = new AttrType[msAttrCnt];
    ASSERT(mpArray != NULL);
    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        AttrType const value = rOther.mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        mpArray[i_attr] = value;
    }

    return *this;
}

bool Combo::operator==(Combo const& rOther) const {
    bool const result = (CountMatchingAttrs(rOther) == msAttrCnt);

    return result;
}

Combo::operator String(void) const {
    String result;

    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        AttrType const value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        AttrModeType const display_mode = ATTR_MODE_ABC;
        result += AttrToString(display_mode, value);
    }

    return result;
}


// misc methods

AttrType Combo::Attr(AttrIndexType ind) const {
    ASSERT(ind < msAttrCnt);

    AttrType const result = mpArray[ind];

    return result;
}

/* static */ AttrCntType Combo::AttrCnt(void) {
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    return msAttrCnt;
}

/* static */ String Combo::AttrToString(AttrModeType displayMode, AttrType value) {
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

/* static */ AttrType Combo::CharToAttr(AttrModeType display_mode, char ch) {
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
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    long result = 1L;
    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
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
AttrIndexType Combo::CommonAttr(Combo const& rOther) const {
    ASSERT(CountMatchingAttrs(rOther) == 1);

    AttrIndexType result = msAttrCnt;
    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        if (mpArray[i_attr] == rOther.mpArray[i_attr]) {
            result = i_attr;
            break;
        }
    }

    ASSERT(result < msAttrCnt);
    return result;
}

AttrCntType Combo::CountMatchingAttrs(Combo const& rOther) const {
    AttrCntType result = 0;
    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        AttrType const attr_value = rOther.mpArray[i_attr];
        if (HasAttr(i_attr, attr_value)) {
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

String Combo::Description(void) const {
    String result;

    for (AttrIndexType i_attr = 0; i_attr < msAttrCnt; i_attr++) {
        AttrType const value = mpArray[i_attr];
        ASSERT(value <= mspValueMax[i_attr]);
        AttrModeType const display_mode = DefaultDisplayMode(i_attr);
        result += AttrToString(display_mode, value);
    }

    return result;
}

// Construct a new Combo from a description.
/* static */ Combo Combo::FromDescription(String const& rDescription) {
    ASSERT(msAttrCnt >= ATTR_CNT_MIN);

    Combo result;
    result.mpArray = new AttrType[msAttrCnt];
    ASSERT(result.mpArray != NULL);

    AttrIndexType i_attr = 0;
    String::ConstIterator i_char;
    for (i_char = rDescription.begin() ; i_char != rDescription.end(); i_char++) {
        char const ch = *i_char;
        if (i_attr < msAttrCnt) {
            AttrModeType const display_mode = DefaultDisplayMode(i_attr);
            AttrType value = CharToAttr(display_mode, ch);
            if (value > ValueMax(i_attr)) {
                value = 0; // so resulting object can be valid
            }
            result.mpArray[i_attr] = value;
            i_attr++;
        }
    }

    while (i_attr < msAttrCnt) {
        // Not enough characters in the string -- pad the attribute array with zeroes.
        result.mpArray[i_attr] = 0;
        i_attr++;
    }

    return result;
}

void Combo::SetAttr(AttrIndexType ind, AttrType value) {
    ASSERT(ind < msAttrCnt);
    ASSERT(value <= mspValueMax[ind]);

    mpArray[ind] = value;
}

/* static */ void Combo::SetStatic(GameOpt const& rGameOpt) {
    AttrCntType const attr_cnt = rGameOpt.AttrCnt();

    ASSERT(attr_cnt >= ATTR_CNT_MIN);
    ASSERT(attr_cnt <= ATTR_CNT_MAX);

    msAttrCnt = attr_cnt;

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

/* static */ String Combo::StringEmpty(void) {
    String const result(msAttrCnt, '.');

    return result;
}

/* static */ AttrType Combo::ValueCnt(AttrIndexType attrIndex) {
    AttrType const result = ValueMax(attrIndex) + 1;

    return result;
}

/* static */ AttrType Combo::ValueMax(AttrIndexType attrIndex) {
    ASSERT(attrIndex < msAttrCnt);
    AttrType const result = mspValueMax[attrIndex];

    return result;
}


// inquiry methods

bool Combo::HasAttr(AttrIndexType index, AttrType value) const {
    ASSERT(index < msAttrCnt);

    AttrType const attrib = mpArray[index];
    bool const result = (attrib == value);

    return result;
}

bool Combo::IsCompatibleWith(Combo const& rOther) const {
    AttrCntType const matchCnt = CountMatchingAttrs(rOther);
    bool const result = (matchCnt == 1);

    return result;
}


// global utility functions

AttrCntType string_to_attr_cnt(String const& rString) {
    long const attr_cnt = long(rString);
    ASSERT(attr_cnt >= Combo::ATTR_CNT_MIN);
    ASSERT(attr_cnt <= Combo::ATTR_CNT_MAX);

    AttrCntType const result = AttrCntType(attr_cnt);

    return result;
}

AttrType string_to_max_attr(String const& rString) {
    long const max_attr = long(rString);
    long const value_cnt = max_attr + 1;
    ASSERT(value_cnt >= Combo::VALUE_CNT_MIN);
    ASSERT(value_cnt <= Combo::VALUE_CNT_MAX);

    AttrType const result = AttrType(value_cnt);

    return result;
}
