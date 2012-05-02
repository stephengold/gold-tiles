#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED

// File:     tile.hpp
// Location: src
// Purpose:  declare Tile class
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
A Tile object represents a playing piece which may be played
on a Board object or stored in a hand or the stock bag (Tiles object).
Each tile has at least two attributes, each of which can assume up
to ten values.  In the context of a particular game, every Tile has the 
same number of attributes.  Distinct tiles with identical attributes 
(clones) are possible; a unique id is used to distinguish clones.
In addition, some randomly-selected tiles are bonus tiles with extra value.

The Tile class is implemented as a static container which maps IDs to
tile options.  Individual tiles are represented by their IDs, which
are positive for locally-generated tiles and negative for tiles
generated by a remote client.
*/

#include <map>          // HASA std::map
#include "indices.hpp"  // HASA IndexType
#include "tileopt.hpp"  // HASA TileOpt


class Tile {
public:
    // public types
    typedef IndexType IdType;

    // public constants
    static const IdType ID_NONE = 0;    // indicates "no such tile"
    static const IdType ID_DEFAULT = 1; // generated only by default constructor
    static const IdType ID_FIRST = 2;   // first ID returned by NextId()
    static const IdType ID_MAX = Indices::INDEX_MAX;

    // public lifecycle
    Tile(void);
    Tile(IdType, bool remote = false);
    // Tile(Tile const&);  compiler-generated copy constructor is OK
    explicit Tile(TileOpt const&);
    explicit Tile(String const&, bool remote = false);
    // ~Tile(void);  compiler-generated destructor is OK

    // public operators
    bool     operator<(Tile const&) const;
    //Tile& operator=(Tile const&);  compiler-generated assignment method is OK
    bool     operator==(Tile const&) const;
    operator Combo(void) const;
    operator IndexType(void) const;
    operator TileOpt(void) const;
    operator String(void) const;

    // misc public methods
    AttrType      Attr(AttrIndexType) const;
    static double BonusProbability(void);
    Tile          CloneAndSetBonus(void) const;
    AttrIndexType CommonAttr(Tile const&) const;
    String        Description(void) const;
    String        GetUserChoice(Tiles const&, Strings const&);
    IdType        Id(void) const;
    void          SetAttr(AttrIndexType, AttrType);
    static void   SetStatic(GameOpt const&);

    // public inquiry methods
    bool HasAttr(AttrIndexType, AttrType) const;
    bool HasBonus(void) const;
    bool HasId(IdType) const;
    bool HasOpt(TileOpt const&) const;
    bool IsClone(Tile const&) const;
    bool IsCompatibleWith(Tile const*) const;
    static bool 
         IsValid(IdType);

private:
    // private types
    typedef std::map<IdType,TileOpt> 
                                Map; 
    typedef Map::const_iterator ConstIterator;
    typedef Map::iterator       Iterator;

    // private constants
    static const String SEPARATOR;

    // private data
    static double msBonusProbability; // configured by SetStatic()
    IdType         mId;
    static IdType msNextId;
    static Map    msOpts;

    // misc private methods
    static IdType NextId(void);
};
#endif // !defined(TILE_HPP_INCLUDED)
