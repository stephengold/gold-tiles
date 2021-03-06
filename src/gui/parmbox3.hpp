#ifndef PARMBOX3_HPP_INCLUDED
#define PARMBOX3_HPP_INCLUDED

// File:     parmbox3.hpp
// Location: src/gui
// Purpose:  declare ParmBox3 class
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

/*
A ParmBox3 object represents a dialog box for setting hand/tile parameters
such as the number of tiles per hand.

The ParmBox3 class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"   // ISA Dialog
#include "gameopt.hpp"      // HASA GameOpt


class ParmBox3: public Dialog {
public:
    // lifecycle
    // no default constructor
    explicit ParmBox3(GameOpt&);
    // ~ParmBox3(void);  implicitly defined destructor

    // misc public methods
    int HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);

private:
    // private constants
    static const ValueType BONUS_PERCENT_MAX = 25;
    static const ValueType CLONES_PER_COMBO_MAX = 10;
    static const ValueType HAND_SIZE_MAX = 12;
    static const ValueType HANDS_DEALT_MAX = 10;

    // private data
    GameOpt& mrGameOpt;

    // private lifecycle
    ParmBox3(ParmBox3 const&);  // not copyable

    // private operators
    ParmBox3& operator=(ParmBox3 const&);  // not assignable

    // misc private methods
    static IdType EditboxId(IdType slider);
    void          InitControl(IdType slider, ValueType value, 
                      ValueType min, ValueType max);
    static IdType MaxId(IdType slider);
    static IdType MinId(IdType slider);
    static IdType SliderId(IdType editbox);
    IdType        SliderId(Win::HWND) const;
    void          UpdateValue(IdType slider, ValueType);
};
#endif // !defined(PARMBOX3_HPP_INCLUDED)
