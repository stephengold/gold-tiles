#ifndef SQUARE_HPP
#define SQUARE_HPP

// File:    square.hpp
// Purpose: Square class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "tile.hpp"

using namespace std;

class Square {
    Tile *tile;
	
	public:
        Square(void);
        Square & operator= (Tile *);
};

#endif
