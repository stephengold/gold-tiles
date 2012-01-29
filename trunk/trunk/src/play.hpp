#ifndef PLAY_HPP_INCLUDED
#define PLAY_HPP_INCLUDED

// File:    play.hpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*
 A Play object represents a group of tiles and the
 locations where they will be played on a 2-D grid.
*/

#include <set>
#include <string>
#include "tilesquare.hpp"

class Play: public set<TileSquare> {
    public:
        void add(Tile const &, GridRef const &);
        void display(void) const;
        unsigned getCount(void) const;
        Tiles getTiles(void) const;
        Locus getSquares(void) const;
        void getUserChoice(Tiles const &);
		bool isPass(void) const;
		bool isPureSwap(void) const;
		bool isSwap(void) const;
        bool repeatsTile(void) const;
        bool repeatsSquare(void) const;
        string toString(void) const;
};

#endif
