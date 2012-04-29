#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

// File:     player.hpp
// Location: src/gui
// Purpose:  declare Player class
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
A Player object represents a (human) user of the local computer.

TODO rename this class!

The Player class implements a static map for translating names to
user-interface settings.
*/

#include <map>                  // HASA std::map
#include "gui/displaymodes.hpp" // HASA DisplayModes
#include "gui/point.hpp"        // HASA Point
#include "gui/win.hpp"          // HASA IdType
#include "string.hpp"           // HASA String

class Player {
public:
    // public lifecycle
    // Player(void); no default constructor
    explicit Player(String const& name);
    // ~Player(void);  compiler-generated destructor is OK

    // public operators
    operator DisplayModes(void) const;
    operator Point(void) const;    // get logical coordinates of Start cell

    // misc public methods
    static Player& 
             rLookup(String const&);
    void     SetAutopause(bool);
    void     SetDisplayModes(DisplayModes const&);
    void     SetPeek(bool);
    void     SetShowClocks(bool);
    void     SetShowGrid(bool);
    void     SetShowScores(bool);
    void     SetStartCellPosition(Point const&);
    void     SetTileSize(unsigned);
    unsigned TileSize(void) const;

    // public inquiry methods
    bool Autopause(void) const;
    bool Peek(void) const;
    bool ShowClocks(void) const;
    bool ShowGrid(void) const;
    bool ShowScores(void) const;

private:
    // private types
    typedef std::map<String, Player*>  Map;
    typedef std::pair<String, Player*> Pair;
    typedef Map::const_iterator        ConstIterator;
    typedef Map::iterator              Iterator;
    typedef std::pair<Iterator, bool>  InsertResult;

    // private data
    bool         mAutopause;
    DisplayModes mDisplayModes;
    static Map  msMap;
    String       mName;
    bool         mPeek;
    bool         mShowClocks;
    bool         mShowGrid;
    bool         mShowScores;
    Point        mStartCellPosition;  // logical coordinates of the Start cell
    unsigned     mTileSize;

    // private lifecycle
    Player(Player const&);  // not copyable

    // private operators
    Player& operator=(Player const&);  // not assignable
};
#endif // !defined(PLAYER_HPP_INCLUDED)
