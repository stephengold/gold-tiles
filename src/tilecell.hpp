#ifndef TILECELL_HPP_INCLUDED
#define TILECELL_HPP_INCLUDED

// File:     tilecell.hpp
// Location: src
// Purpose:  declare TileCell class
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
A TileCell object describes a playing piece (Tile) and also the location
where it is to be played.
*/

#include "cell.hpp"  // HASA Cell
#include "tile.hpp"  // HASA Tile


class TileCell {
public:
    // public lifecycle
    TileCell(void);
    explicit TileCell(Tile const&);  // swap to swap area
    // TileCell(TileCell const&);  compiler-generated copy constructor is OK
    TileCell(String const&, bool remote);
    TileCell(Tile const&, Cell const&);  // place on a cell
    // ~TileCell(void);  compiler-generated destructor is OK

    // public operators
    bool operator!=(TileCell const&) const;
    bool operator<(TileCell const&) const;
    // TileCell& operator=(TileCell const&);  compiler-generated assignment method is OK
    operator Cell(void) const;
    operator String(void) const;
    operator Tile(void) const;

    // misc public methods
    String Description(void) const;
    String GetUserChoice(Tiles const&, Strings const&);

    // public inquiry methods
    bool IsSwap(void) const;

private:
    // private constants
    static const String SEPARATOR;
    static const String SWAP;

    // private data
    Cell mCell;  // ignored if mSwapFlag is true
    bool mSwapFlag;
    Tile mTile;
};
#endif // !defined(TILECELL_HPP_INCLUDED)
