#ifndef TILE_HPP
#define TILE_HPP

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"
#include <set>
#include <string>

class Tiles;

// attribute utilities
string attributeToString(unsigned ind, unsigned value);

class Tile {
    // private data
	static unsigned *maxAttribute;  // number of values for each tile attribute
	static unsigned nextId;
	static unsigned numAttributes;  // number of attributes for each tile

	unsigned *arr;
	public:
	unsigned id;
	unsigned magic;
	static unsigned tileMagic;

	public:
        static void displayEmpty(void);
        static unsigned getNumAttributes(void);
		static unsigned getMaxAttribute(unsigned);
		static void setStatic(unsigned numAttr, unsigned const maxAttr[]);

		Tile(void);          // construct a blank tile
		Tile(string const &);
		Tile(Tile const &);  // copy a tile
		~Tile(void);
		
		Tile clone(void) const;
        void display(void) const;
		unsigned getAttribute(unsigned) const;
		bool hasAttribute(unsigned ind, unsigned value) const;
		bool isClone(Tile const &) const;
		bool isCloneAny(Tiles const &) const;
		bool isCompatibleWith(Tile const *) const;
		bool operator<(Tile const &) const;
		Tile &operator=(Tile const &);
	    bool operator==(Tile const &) const;
		void setAttribute(unsigned ind, unsigned value);
		string toString(void) const;
};

// Tile* utilities
bool isValid(const Tile *);

#endif
