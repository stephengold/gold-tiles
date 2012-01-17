#ifndef BOARD_HPP
#define BOARD_HPP

// File:    board.hpp
// Purpose: Board class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp"
#include "locus.hpp"
#include "play.hpp"
#include "tile.hpp"
#include "tiles.hpp"
#include "tilesquare.hpp"

class Board : public GridOf<Tile> {
    public:
        bool areAllCompatible(Locus const &) const;
        bool areAllEmpty(Locus const &) const;
        bool areAllRowsCompatible(Locus const &) const;
        bool areAllColumnsCompatible(Locus const &) const;
		Tiles getAll(Locus const &) const;
        bool isRowCompatible(int r, int firstC, int lastC) const;
        bool isRowCompatible(GridRef const &) const;
        bool isColumnCompatible(int firstR, int lastR, int c) const;
        bool isColumnCompatible(GridRef const &) const;
        bool isContiguousRow(Locus const &) const;
        bool isContiguousColumn(Locus const &) const;
        void placeTile(TileSquare const &);
        void placeTiles(Play const &);
};

#endif
