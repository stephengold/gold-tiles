#ifndef TILES_HPP
#define TILES_HPP

// File:    tiles.hpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <set>
#include <string>
#include "tile.hpp"

class Tiles : public set<Tile> {
    public:
        bool areAllCompatible(void) const;
        bool contains(Tile const &) const;
        bool copyIds(Tiles const &);
        void display(/* User & */) const;
        Tile drawRandomTile(void);
        void getUserChoice(/* User &, */ Tiles const &);
        bool isValid(void) const;
        string toString(void) const;
        void unClone(Tile &) const;
        Tiles uniqueTiles(void) const;
};

#endif
