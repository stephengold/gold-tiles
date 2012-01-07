#ifndef TILESQUARE_HPP
#define TILESQUARE_HPP

#include <list>
#include "gridof.hpp"
#include "tile.hpp"

class TileSquare {
    GridRef *square;
    Tile *tile;
    
    public:
        TileSquare(void);
        TileSquare(Tile *, GridRef *);
        GridRef *getSquare(void);  
        Tile *getTile(void);
};

#endif
