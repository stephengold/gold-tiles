#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

// File:    player.hpp
// Purpose: Player class header file
// Author:  Stephen Gold sgold@sonic.net
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
A Player object represents a human user of the local computer.

The Player class implements a static map for translating names to
user-interface settings.
*/

#include <map>
#include "gui/win.hpp"
#include "project.hpp"
#include "string.hpp"

class Player {
public:
	// lifecycle
	// Player(void); no default constructor
	Player(String const &);
	Player(Player const &) { FAIL(); }
    // ~Player(void);  compiler-generated destructor is OK

	// operators
	Player &operator=(Player const &) { FAIL(); }

	// misc public methods
    static Player *Lookup(String const &);
	void           SetAutocenter(bool);
	void           SetAutopause(bool);
    void           SetPeek(bool);
	void           SetShowClocks(bool);
    void           SetShowGrid(bool);
	void           SetShowScores(bool);
	void           SetTileSize(IdType);
	IdType         TileSize(void) const;

	// public inquiry methods
	bool Autocenter(void) const;
	bool Autopause(void) const;
    bool Peek(void) const;
	bool ShowClocks(void) const;
    bool ShowGrid(void) const;
	bool ShowScores(void) const;

private:
    typedef std::map<String, Player*>     Map;
    typedef std::pair<String, Player*>    Pair;
	typedef Map::const_iterator           ConstIteratorType;
    typedef Map::iterator                 IteratorType;
    typedef std::pair<IteratorType, bool> InsertResultType;

    static Map msMap;

	String mName;
	bool   mAutocenter;
	bool   mAutopause;
	bool   mPeek;
	bool   mShowClocks;
	bool   mShowGrid;
	bool   mShowScores;
	IdType mTileSize;

	// misc methods
};

// global utility functions

#endif
