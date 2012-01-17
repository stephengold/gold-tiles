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
        string toString(void) const;
        Tiles uniqueTiles(void) const;
};

#endif
