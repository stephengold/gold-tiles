#ifndef ATTRBOX_HPP_INCLUDED
#define ATTRBOX_HPP_INCLUDED

// File:     attrbox.hpp
// Location: src/gui
// Purpose:  declare AttrBox class
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
An AttrBox object represents a dialog box for changing how tile attributes
are displayed in the GUI.

The AttrBox class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"       // ISA Dialog
#include "gui/displaymodes.hpp" // HASA DisplayModes
#include "tile.hpp"             // USES AttrIndexType

class AttrBox: public Dialog {
public:
    // public lifecycle
    // no default constructor
    AttrBox(DisplayModes const&);
    // ~AttrBox(void);  compiler-generated destructor is OK

    // public operators
    operator DisplayModes(void) const;

    // misc public methods
    int HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);

private:
    // private data
    DisplayModes mDisplayModes;

    // private lifecycle
    AttrBox(AttrBox const&);  // not copyable

    // private operators
    AttrBox& operator=(AttrBox const&); // not assignable

    // misc private methods
    static AttrIndexType 
                    AttrIndex(IdType);
    void            BumpDisplayMode(AttrIndexType);
    static TextType DisplayModeText(AttrModeType);
    static IdType   ListboxId(AttrIndexType);
    void            UpdateValue(IdType, AttrModeType);
};
#endif // !defined(ATTRBOX_HPP_INCLUDED)
