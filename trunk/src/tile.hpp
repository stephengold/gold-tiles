#ifndef TILE_HPP
#define TILE_HPP

// File:    tile.hpp
// Purpose: Tile class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

using namespace std;

class Tile {
    // private data
	static unsigned blankAttribute;
	static unsigned *maxAttribute;
	static unsigned numAttributes;
	static unsigned nextId;
	unsigned *arr;
	unsigned id;

	public:
		Tile(void);
		~Tile(void);
		bool compatibleWith(const Tile *) const;
        void display(void) const;
        static void displayEmpty(void);
		unsigned getAttribute(unsigned);
		bool operator<(const Tile &) const;
		void setAttribute(unsigned ind, unsigned value);
};

#endif
