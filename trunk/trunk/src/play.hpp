#ifndef PLAY_HPP
#define PLAY_HPP

// File:    play.hpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*
 A Play object describes a group of tiles and the
 locations where they will be played on a 2-D grid.
*/

#include <set>
#include <string>
#include "tilesquare.hpp"

class Play : public set<TileSquare> {
    public:
        void display(/* User & */) const;
        Tiles getTiles(void) const;
        Locus getSquares(void) const;
        void getUserChoice(Tiles const &);
        bool repeatsTile(void) const;
        bool repeatsSquare(void) const;
        string toString(void) const;
};

#endif
