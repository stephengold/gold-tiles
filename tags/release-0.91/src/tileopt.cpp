// File:     tileopt.cpp
// Location: src
// Purpose:  implement TileOpt class
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

#include "string.hpp"
#include "tileopt.hpp"


// lifecycle

TileOpt::TileOpt(void) {
    mHasBonus = false;
}

// new TileOpt from String
TileOpt::TileOpt(String const& rString) {
    String copy = rString;

    mHasBonus = false;
    if (copy.Last() == BONUS_CHARACTER) {
        mHasBonus = true;
        copy.Shorten(1);
    }
    mCombo = Combo(copy);
}

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment method is fine.

bool TileOpt::operator==(TileOpt const& rOther) const {
    bool const result = (mCombo == rOther.mCombo
        && mHasBonus == rOther.mHasBonus);

    return result;
}

TileOpt::operator Combo(void) const {
    return mCombo;
}

TileOpt::operator String(void) const {
    String result = String(mCombo);
    if (mHasBonus) {
        result += String(BONUS_CHARACTER);
    }

    return result;
}


// misc methods

AttrType TileOpt::Attr(AttrIndexType index) const {
    AttrType const result = mCombo.Attr(index);

    return result;
}

AttrIndexType TileOpt::CommonAttr(TileOpt const& rOther) const {
    AttrIndexType const result = mCombo.CommonAttr(rOther.mCombo);

    return result;
}

AttrCntType TileOpt::CountMatchingAttrs(TileOpt const& rOther) const {
    AttrCntType const result = mCombo.CountMatchingAttrs(rOther.mCombo);

    return result;
}

String TileOpt::Description(void) const {
    String result = mCombo.Description();
    if (mHasBonus) {
        result += String('+');
    }

    return result;
}

/* static */ TileOpt TileOpt::FromDescription(String const& rDescription) {
    String copy = rDescription;
    copy.Purge();

    TileOpt result;
    if (copy.Last() == '+') {
        result.mHasBonus = true;
        copy.Shorten(1);
    }
    result.mCombo = Combo::FromDescription(copy);

    return result;
}

void TileOpt::SetAttr(AttrIndexType index, AttrType value) {
    mCombo.SetAttr(index, value);
}

void TileOpt::SetBonus(bool newBonus) {
    mHasBonus = newBonus;
}


// inquiry methods

bool TileOpt::HasAttr(AttrIndexType index, AttrType value) const {
    bool const result = mCombo.HasAttr(index, value);

    return result;
}

bool TileOpt::HasBonus(void) const {
    return mHasBonus;
}

bool TileOpt::IsCompatibleWith(TileOpt const& rOther) const {
    bool const result = mCombo.IsCompatibleWith(rOther);

    return result;
}

bool TileOpt::MatchesDescription(String const& rMatch) const {
    String description = Description();

    // Purge both strings, so that only graphic characters are compared.
    description = description.Purge();
    String const match = rMatch.Purge();

    bool const result = (description == match);

    return result;
}
