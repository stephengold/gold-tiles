#ifndef PLAY_HPP
#define PLAY_HPP

// File:    play.hpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <set>
#include "locus.hpp"
#include "tiles.hpp"
#include "tilesquare.hpp"

class Play : public set<TileSquare> {
    public:
        Tiles getTiles(void) const;
        Locus getSquares(void) const;
        bool repeatsTile(void) const;
        bool repeatsSquare(void) const;
};

#endif
