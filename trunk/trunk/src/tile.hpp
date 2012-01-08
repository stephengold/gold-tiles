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
	static unsigned *maxAttribute;  // number of values (excluding blank) for each tile attribute
	static unsigned numAttributes;  // number of attributes for each tile
	static unsigned nextId;
	
	unsigned *arr;
	unsigned id;

	public:
        static void displayEmpty(void);
		static void setNumAttributes(unsigned);

		Tile(void);          // construct a blank tile
		Tile(const Tile &);  // copy a tile
		~Tile(void);
		
		bool compatibleWith(const Tile *) const;
        void display(void) const;
		unsigned getAttribute(unsigned);
		bool operator<(const Tile &) const;
		void setAttribute(unsigned ind, unsigned value);
		string toString(void) const;
};

#endif
