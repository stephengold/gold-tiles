#ifndef TILES_HPP_INCLUDED
#define TILES_HPP_INCLUDED

// File:     tiles.hpp
// Location: src
// Purpose:  declare Tiles class
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
A Tiles represents a collection of zero or more distinct tiles.

The Tiles class is implemented as a collection of tile IDs, using
the Indices class as its base.
*/

#include "tile.hpp" // USES Tile


class Tiles: public Indices {
public:
    // public lifecycle
    Tiles(void);
    explicit Tiles(Tile const&);
    // Tiles(Tiles const&);  implicitly defined copy constructor
    Tiles(Indices const&, bool remote);
    Tiles(String const&, bool remote);
    // ~Tiles(void);  implicitly defined destructor

    // public operators
    // Tiles& operator=(Tiles const&);  implicitly defined assignment method
    operator String(void) const; // for display/debug only, not for save/send

    // misc public methods
    void     Add(Tile::IdType);
    void     AddAllTiles(AttrIndexType, Tile& model);
    unsigned BonusFactor(void) const;
    AttrIndexType
             CommonAttr(void) const;
    bool     CopyIds(Tiles const&);
    String   Description(void) const;
    Tile::IdType
             FindFirst(TileOpt const&) const;
    Tiles    LongestRun(void) const;
    Tile     PullFirstTile(void);
    Tile     PullRandomTile(void);
    Tiles    PullRandomTiles(unsigned);
    void     Restock(void);
    Tiles    UniqueTiles(void) const;

    // public inquiry methods
    bool AreAllCompatible(void) const;
    bool AreAllCompatibleWith(Tile const&) const;
    bool ContainsOpt(TileOpt const&) const;

private:
    // private constants
    static const String PREFIX;
    static const String SEPARATOR;
    static const String SUFFIX;

    // private methods
    void BuildRuns(Tiles const& runSoFar, Tiles& bestRun) const;
};
#endif // !defined(TILES_HPP_INCLUDED)
