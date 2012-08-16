#ifndef TILEOPT_HPP_INCLUDED
#define TILEOPT_HPP_INCLUDED

// File:     tileopt.hpp
// Location: src
// Purpose:  declare TileOpt class
// Author:   Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of theGNU General Public License

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

/*
A TileOpt object represents the user-visible options of a Tile, namely
its bonus value and attributes.

The TileOpt class is implemented by combining a bool (for the bonus)
with Combo (for the attributes).
*/

#include "combo.hpp"    // HASA Combo

class TileOpt {
public:
    // public lifecycle
    TileOpt(void);
    explicit TileOpt(String const&);
    // TileOpt(TileOpt const&);  implicitly defined copy constructor
    // ~TileOpt(void);  implicitly defined destructor

    // public operators
    // TileOpt& operator=(TileOpt const&);  implicitly defined assignment operator
    bool     operator==(TileOpt const&) const;
    operator Combo(void) const;
    operator String(void) const;

    // misc public methods
    AttrType       Attr(AttrIndexType) const;
    AttrIndexType  CommonAttr(TileOpt const&) const;
    AttrCntType    CountMatchingAttrs(TileOpt const&) const;
    String         Description(void) const;
    static TileOpt FromDescription(String const&);
    void           SetAttr(AttrIndexType, AttrType);
    void           SetBonus(bool);

    // public inquiry methods
    bool HasAttr(AttrIndexType, AttrType) const;
    bool HasBonus(void) const;
    bool IsCompatibleWith(TileOpt const&) const;
    bool MatchesDescription(String const&) const;

private:
    // private constants
    static const char BONUS_CHARACTER = '+';

    // private data
    Combo mCombo;    // all attributes
    bool  mHasBonus;
};
#endif  // !defined(TILEOPT_HPP_INCLUDED)
