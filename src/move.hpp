#ifndef MOVE_HPP_INCLUDED
#define MOVE_HPP_INCLUDED

// File:     move.hpp
// Location: src
// Purpose:  declare Move class
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
A Move object represents a group of zero or more tiles and the
location(s) where they will be played, or alternatively it indicates
that a hand will resign from the game.

The Move class encapsulates a set of TileCell objects and a
resignation flag.
*/

#include <set>           // HASA std::set
#include "tilecell.hpp"  // HASA TileCell


class Move {
public:
    // public types
    typedef std::set<TileCell>::const_iterator ConstIterator;

    // public lifecycle
    Move(void);
    // Move(Move const&);  implicitly defined copy constructor
    Move(String const&, bool remote);
    // ~Move(void);  implicitly defined destructor

    // public operators
    // Move& operator=(Move const&);  implicitly defined assigment operator
    bool operator!=(Move const&) const;
    operator Cells(void) const;
    operator String(void) const;
    operator Tiles(void) const;

    // misc public methods
    void     Add(TileCell const&);
    void     Add(Tile const&, Cell const&);
    void     AddSwapTile(Tile const&);
    ConstIterator
             Begin(void) const;
    unsigned Count(void) const;
    unsigned CountTilesPlayed(void) const;
    String   Description(void) const;
    ConstIterator
             End(void) const;
    void     GetUserChoice(Tiles const&, unsigned mustPlay);
    void     MakeResign(Tiles const&);

    // public inquiry methods
    bool InvolvesSwap(void) const;
    bool IsPass(void) const;
    bool IsPlay(void) const;
    bool IsPureSwap(void) const;
    bool IsResign(void) const;
    bool RepeatsCell(void) const;
    bool RepeatsTile(void) const;

private:
    // private types
    typedef std::set<TileCell> Set;

    // private constants
    static const String PREFIX;
    static const String RESIGN;
    static const String SEPARATOR;
    static const String SUFFIX;

    // private data
    bool mResignFlag;
    Set  mSet;

    // misc private methods
    void MakePass(void);
};
#endif // !defined(MOVE_HPP_INCLUDED)
