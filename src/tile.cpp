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

double     Tile::msBonusProbability = 0.0; // configured by SetStatic()
TileIdType Tile::msNextId = ID_FIRST;


// lifecycle

Tile::Tile(void) {
	mBonusValue = 0;
    mId = ID_DEFAULT; // an ID used only by the default constructor
}

// Mint a new tile based on a string.
Tile::Tile(String const& rString) {
	mBonusValue = 0;
	String combo_string = rString;
	combo_string.Purge();

	char const last_char = combo_string.Last();
    if (last_char == '+') {
        mBonusValue = 1;
		combo_string.Shorten(1);
    }
	mCombo = Combo(combo_string);
    mId = NextId();

    ASSERT(IsValid());
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


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

	mCombo = rOther.mCombo;
	mBonusValue = rOther.mBonusValue;
    mId = rOther.mId;
     
    return *this;
}

bool Tile::operator==(Tile const& rOther) const {
	ASSERT(IsValid());
	ASSERT(rOther.IsValid());

	bool const result = (mId == rOther.mId);

	ASSERT(!result || mCombo == rOther.mCombo);

    return result;
}

Tile::operator String(void) const {
	ASSERT(IsValid());

	String result = mCombo;

	if (mBonusValue > 0) {
		result += '+';
	} else {
		result += ' ';
	}

	// result length should match that of the StringEmpty() method
	ASSERT(result.Length() == unsigned(Combo::AttributeCnt() + 1));

	return result;
}


// misc methods

AttrType Tile::Attribute(AttrIndexType index) const {
    AttrType const result = mCombo.Attribute(index);
    
    return result;
}

/* static */ double Tile::BonusProbability(void) {
	return msBonusProbability;
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

// identify the common attribute of a compatible tile
AttrIndexType Tile::CommonAttribute(Tile const& rOther) const {
    AttrIndexType const result = mCombo.CommonAttribute(rOther.mCombo);

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

void Tile::SetAttribute(AttrIndexType index, AttrType value) {
	mCombo.SetAttribute(index, value);

	ASSERT(IsValid());
}

/* static */ void Tile::SetStatic(GameOpt const& rGameOpt) {
	double const bonus_probability = rGameOpt.BonusPercent()/100.0;

    ASSERT(bonus_probability >= 0.0);
    ASSERT(bonus_probability <= 1.0);

	msBonusProbability = bonus_probability;
}

/* static */ String Tile::StringEmpty(void) {
    String const result(Combo::AttributeCnt() + 1, '.'); // +1 for bonus indication

	// result length should agree with String() operator
	ASSERT(result.Length() == unsigned(Combo::AttributeCnt() + 1));

    return result;
}


// inquiry methods

bool Tile::HasAttribute(AttrIndexType index, AttrType value) const {
	bool const result = mCombo.HasAttribute(index, value);
    
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
        result = (mCombo == rOther.mCombo 
			   && mBonusValue == rOther.mBonusValue);
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
        result = mCombo.IsCompatibleWith(pOther->mCombo);
    }
    
    return result;
}

bool Tile::IsValid(void) const {
    bool result = false;
    
    if ((mBonusValue == 0 || mBonusValue == 1)
	 && (mId == ID_DEFAULT || mId >= ID_FIRST && mId < msNextId))
	{
        result = true;
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
