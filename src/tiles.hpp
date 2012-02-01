#ifndef TILES_HPP_INCLUDED
#define TILES_HPP_INCLUDED

// File:    tiles.hpp
// Purpose: Tiles class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <set>
#include "tile.hpp"

class Tiles: public set<Tile> {
    public:
        void addTile(Tile const &);
		void addTiles(Tiles const &);
        bool areAllCompatible(void) const;
        bool contains(Tile const &) const;
        bool copyIds(Tiles const &);
        void display(void) const;
        Tile drawRandomTile(void);
		unsigned drawTiles(unsigned, Tiles &bag);
		Tile findTile(TileId) const;
		Tiles getLongestRun(void) const;
        void getUserChoice(Tiles const &);
		bool isEmpty(void) const;
        bool isValid(void) const;
		void removeTile(Tile const &);
		void removeTiles(Tiles const &);
        string toString(void) const;
        void unClone(Tile &) const;
        Tiles uniqueTiles(void) const;
};

#endif
