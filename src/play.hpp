#ifndef PLAY_HPP_INCLUDED
#define PLAY_HPP_INCLUDED

// File:    play.hpp
// Purpose: Play class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Play object represents a group of tiles and the
 locations where they will be played.
*/

#include <set>
#include "tilesquare.hpp"

class Play: public set<TileSquare> {
    public:
        // Play(void);  compiler-generated default constructor is OK
        // Play(Play const &);  compiler-generated copy constructor is OK
        // Play &operator=(Play const &)};  compiler-generated assignment method is OK
        // ~Play(void);  compiler-generated destructor is OK

        void add(Tile const &);
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
