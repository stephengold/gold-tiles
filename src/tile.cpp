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

#include <iostream>    // std::cin
#include "gameopt.hpp"
#include "strings.hpp"
#include "tiles.hpp"


// static constants

const String Tile::SEPARATOR(":");


// static data

Fraction     Tile::msBonusProbability = 0.0; // configured by SetStatic()
Tile::IdType Tile::msNextId = ID_FIRST;
Tile::Map    Tile::msOpts;


// lifecycle

Tile::Tile(void) {
    mId = ID_DEFAULT; // special ID generated *only* by this constructor
    msOpts[mId] = TileOpt();

    ASSERT(!IsValid(mId));
}

Tile::Tile(String const& rString, bool remoteFlag) {
    Strings const parts(rString, SEPARATOR);
    ASSERT(parts.Count() == 2); // TODO recovery

    String const first = parts.First();
    IdType id = first;
    if (remoteFlag) {
        id = -id; // change sign of remote ID
    }
    ASSERT(IsValid(id));
    mId = id;

    String const second = parts.Second();
    TileOpt const opt = TileOpt(second);
    msOpts[id] = opt;
}

// Mint a new tile based on a TileOpt.
Tile::Tile(TileOpt const& rOpt) {
    mId = NextId();
    msOpts[mId] = rOpt;
}

Tile::Tile(IdType id, bool remoteFlag) {
    if (remoteFlag) {
        id = -id;
    }
    ASSERT(IsValid(id));
    mId = id;
}

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

bool Tile::operator<(Tile const& rOther) const {
    bool const result = (mId < rOther.mId);

    return result;
}

// The implicitly defined assignment method is fine.

bool Tile::operator==(Tile const& rOther) const {
    bool const result = (mId == rOther.mId);

    return result;
}

Tile::operator Combo(void) const {
    TileOpt const& r_opt = msOpts[mId];
    Combo const result = r_opt;

    return result;
}

Tile::operator IndexType(void) const {
    return mId;
}

Tile::operator String(void) const {
    TileOpt const& r_opt = msOpts[mId];
    String const result = String(mId) + SEPARATOR + String(r_opt);

    return result;
}

Tile::operator TileOpt(void) const {
    TileOpt const result = msOpts[mId];

    return result;
}


// misc methods

AttrType Tile::Attr(AttrIndexType index) const {
    TileOpt const& r_opt = msOpts[mId];
    AttrType const result = r_opt.Attr(index);

    return result;
}

/* static */ Fraction Tile::BonusProbability(void) {
    return msBonusProbability;
}

// Create a clone (with a new ID) and randomize its bonus value.
Tile Tile::CloneAndSetBonus(void) const {
    // Create a Tile with a new ID.
    Tile result(*this);
    result.mId = NextId();

    // Copy the visible options.
    TileOpt& r_result_opt = msOpts[result.mId];
    r_result_opt = msOpts[mId];

    // Randomize its bonus value.
    bool const gets_bonus = msBonusProbability.RandomBool();
    r_result_opt.SetBonus(gets_bonus);

    return result;
}

// Identify the common attribute of a compatible tile.
AttrIndexType Tile::CommonAttr(Tile const& rOther) const {
    TileOpt const& r_opt = msOpts[mId];
    TileOpt const& r_other = msOpts[rOther.mId];
    AttrIndexType const result = r_opt.CommonAttr(r_other);

    return result;
}

String Tile::Description(void) const {
    TileOpt const& r_opt = msOpts[mId];
    String const result = r_opt.Description();

    return result;
}

String Tile::GetUserChoice(Tiles const& rAvailableTiles, Strings const& rAlternatives) {
    ASSERT(mId == ID_DEFAULT);

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

        TileOpt const opt = TileOpt::FromDescription(result);
        if (!opt.MatchesDescription(result)) {
            std::cout << result.Quote() << " is invalid input." << std::endl;

        } else if (!rAvailableTiles.ContainsOpt(opt)) {
            std::cout << result << " is unavailable." << std::endl;

        } else {
            mId = rAvailableTiles.FindFirst(opt);
            ASSERT(IsValid(mId));
            break;
        }
    }

    return result;
}

Tile::IdType Tile::Id(void) const {
    return mId;
}

/* static */ Tile::IdType Tile::NextId(void) {
    ASSERT(msNextId < ID_MAX);

    IdType const result = msNextId;
    msNextId = result + 1;

    ASSERT(IsValid(result));
    return result;
}

void Tile::SetAttr(AttrIndexType index, AttrType value) {
    TileOpt& r_opt = msOpts[mId];
    r_opt.SetAttr(index, value);
}

/* static */ void Tile::SetStatic(GameOpt const& rGameOpt) {
    Combo::SetStatic(rGameOpt);

    Fraction const bonus_probability = rGameOpt.BonusPercent()/100.0;
    msBonusProbability = bonus_probability;

    msNextId = ID_FIRST;
    msOpts.clear();
}


// inquiry methods

bool Tile::HasAttr(AttrIndexType index, AttrType value) const {
    TileOpt const& r_opt = msOpts[mId];
    bool const result = r_opt.HasAttr(index, value);

    return result;
}

bool Tile::HasBonus(void) const {
    TileOpt const& r_opt = msOpts[mId];
    bool const result = r_opt.HasBonus();

    return result;
}

bool Tile::HasId(IdType id) const {
    bool const result = (mId == id);

    return result;
}

// Do the bonus value and all attributes match?
bool Tile::HasOpt(TileOpt const& rOpt) const {
    bool const result = (msOpts[mId] == rOpt);

    return result;
}

// Same options but different ID?
bool Tile::IsClone(Tile const& rOther) const {
    TileOpt& r_other = msOpts[rOther.mId];
    bool const result = (mId != rOther.mId && HasOpt(r_other));

    return result;
}

bool Tile::IsCompatibleWith(Tile const* pOther) const {
    bool result = true;

    if (pOther != NULL) {
        TileOpt const& r_opt = msOpts[mId];
        TileOpt const& r_other = msOpts[pOther->mId];
        result = r_opt.IsCompatibleWith(r_other);
    }

    return result;
}

/* static */ bool Tile::IsValid(Tile::IdType id) {
    bool const result = (id <= -ID_FIRST || (id >= ID_FIRST && id < msNextId));

    return result;
}
