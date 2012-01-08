#ifndef TILE_HPP
#define TILE_HPP

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

using namespace std;

#include <string>

class Tile {
    // private data
	static unsigned blankAttribute; // attribute value used for wildcard tiles
	static unsigned *maxAttribute;  // number of non-blank values for each tile attribute
	static unsigned nextId;
	static unsigned numAttributes;  // number of attributes for each tile
	
	unsigned *arr;
	unsigned id;

	public:
        static void displayEmpty(void);
        static unsigned getNumAttributes(void);
		static unsigned getMaxAttribute(unsigned);
		static void setStatic(unsigned numAttr, unsigned maxAttr[]);

		Tile(void);          // construct a blank tile
		Tile(const Tile &);  // copy a tile
		~Tile(void);
		
		bool compatibleWith(const Tile *) const;
		Tile *clone(void) const;
        void display(void) const;
		unsigned getAttribute(unsigned) const;
		bool hasAttribute(unsigned ind, unsigned value) const;
		bool operator<(const Tile &) const;
		void setAttribute(unsigned ind, unsigned value);
		string toString(void) const;
};

#endif
