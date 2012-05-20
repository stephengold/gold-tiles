#ifndef USER_HPP_INCLUDED
#define USER_HPP_INCLUDED

// File:     user.hpp
// Location: src/gui
// Purpose:  declare User class
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
A User object represents a (human) user of the local computer.

The User class implements a static map for translating names to
user-interface settings.
*/

#include <map>                   // HASA std::map
#include "gui/displaymodes.hpp"  // HASA DisplayModes
#include "gui/point.hpp"         // HASA Point
#include "string.hpp"            // HASA String

class User {
public:
    // public lifecycle
    // no default constructor
    explicit User(String const& name);
    // ~User(void);  implicitly defined destructor

    // public operators
    operator DisplayModes(void) const;
    operator Point(void) const;    // get logical coordinates of Start cell

    // misc public methods
    unsigned BoardTileSize(void) const;
    unsigned HandTileSize(void) const;
    static User& 
            rLookup(String const&);
    void     SetAutopause(bool);
    void     SetBoardTileSize(TileSizeType);
    void     SetDisplayModes(DisplayModes const&);
    void     SetHandTileSize(TileSizeType);
    void     SetPeek(bool);
    void     SetShowClocks(bool);
    void     SetShowGrid(bool);
    void     SetShowScores(bool);
    void     SetStartCellPosition(Point const&);

    // public inquiry methods
    bool Autopause(void) const;
    bool Peek(void) const;
    bool ShowClocks(void) const;
    bool ShowGrid(void) const;
    bool ShowScores(void) const;

private:
    // private types
    typedef std::map<String, User*>   Map;
    typedef std::pair<String, User*>  Pair;
    typedef Map::const_iterator       ConstIterator;
    typedef Map::iterator             Iterator;
    typedef std::pair<Iterator, bool> InsertResult;

    // private data
    bool         mAutopause;
    TileSizeType mBoardTileSize;
    DisplayModes mDisplayModes;
    TileSizeType mHandTileSize;
    static Map  msMap;
    String       mName;
    bool         mPeek;
    bool         mShowClocks;
    bool         mShowGrid;
    bool         mShowScores;
    Point        mStartCellPosition;  // logical coordinates of the Start cell

    // private lifecycle
    User(User const&);  // not copyable

    // private operators
    User& operator=(User const&);  // not assignable
};
#endif  // !defined(USER_HPP_INCLUDED)
