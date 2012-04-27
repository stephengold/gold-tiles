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
	copy.Purge();

    if (copy.Last() == BONUS_CHARACTER) {
        mHasBonus = true;
		copy.Shorten(1);
    } else {
		mHasBonus = false;
	}
	mCombo = Combo(copy);
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment method is fine.

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

AttrType TileOpt::Attribute(AttrIndexType index) const {
	AttrType const result = mCombo.Attribute(index);

	return result;
}
AttrIndexType TileOpt::CommonAttribute(TileOpt const& rOther) const {
	AttrIndexType const result = mCombo.CommonAttribute(rOther.mCombo);

	return result;
}
AttrCntType TileOpt::CountMatchingAttributes(TileOpt const& rOther) const {
	AttrCntType const result = mCombo.CountMatchingAttributes(rOther.mCombo);

	return result;
}
void TileOpt::SetAttribute(AttrIndexType index, AttrType value) {
    mCombo.SetAttribute(index, value);
}

void TileOpt::SetBonus(bool newBonus) {
	mHasBonus = newBonus;
}


// inquiry methods

bool TileOpt::HasAttribute(AttrIndexType index, AttrType value) const {
	bool const result = mCombo.HasAttribute(index, value);

	return result;
}

bool TileOpt::HasBonus(void) const {
    return mHasBonus;
}

bool TileOpt::IsCompatibleWith(TileOpt const& rOther) const {
	bool const result = mCombo.IsCompatibleWith(rOther);

	return result;
}

bool TileOpt::MatchesString(String const& rMatch) const {
	String string_form = String(*this);
	string_form = string_form.Purge();
	String const match = rMatch.Purge();

	bool const result = (string_form == match);

	return result;
}
