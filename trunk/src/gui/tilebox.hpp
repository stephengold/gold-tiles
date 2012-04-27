#ifndef TILEBOX_HPP_INCLUDED
#define TILEBOX_HPP_INCLUDED

// File:     tilebox.hpp
// Location: src/gui
// Purpose:  declare TileBox class
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
A TileBox object represents a dialog box for adjusting the number of values
for each attribute.

The TileBox class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"   // ISA Dialog
#include "gameopt.hpp"      // HASA GameOpt

class TileBox: public Dialog {
public:
    // public lifecycle
	// no default constructor
	TileBox(GameOpt&);
	// ~TileBox(void);  compiler-generated destructor is OK

	// misc public methods
	int HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);

private:
	// private data
	GameOpt& mrGameOpt;

	// private lifecycle
    TileBox(TileBox const&);  // not copyable

	// private operators
    TileBox& operator=(TileBox const&); // not assignable

	// misc private methods
    IdType EditboxId(IdType slider) const;
    void   InitControl(IdType slider, ValueType value, bool enable);
    IdType MaxId(IdType slider) const;
    IdType MinId(IdType slider) const;
    IdType SliderId(IdType editbox) const;
	IdType SliderId(Win::HWND) const;
	void   UpdateTileCnt(void);
	void   UpdateValue(IdType slider, ValueType);
};
#endif // !defined(TILEBOX_HPP_INCLUDED)
