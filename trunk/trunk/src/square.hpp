#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "tile.hpp"

class Square {
    Tile *tile;
	
	public:
        Square(void);
        Square & operator= (Tile *);
};

#endif
