#ifndef BOARD_HPP_INCLUDED
#define BOARD_HPP_INCLUDED

// File:    board.hpp
// Purpose: Board class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Board object represents a two-dimensional playing surface
 which may be extended in four directions.  The surface is
 modeled as a grid of cells onto which Tile objects may be
 played.  Cells are referenced by means of GridRef objects.
 A given cell may only be played on once.

 Board extends the BaseBoard class.  The implementation of
 BaseBoard is hidden from these extensions.
*/

#include "baseboard.hpp"

class Board: public BaseBoard {
    bool anyConnectToOrigin(Locus const &) const;
    bool areAllCompatible(Locus const &) const;
    bool areAllEmpty(Locus const &) const;
    bool areAllRowsCompatible(Locus const &) const;
    bool areAllColumnsCompatible(Locus const &) const;
	Tile const *getPtr(int n, int e) const;
	Tile getTile(int n, int e) const;
	Tiles getAll(Locus const &) const;
	void getRowLimits(GridRef const &, int &r, int &c1, int &c2) const;
	void getColumnLimits(GridRef const &, int &r1, int &r2, int &c) const;
	bool isEmptyCell(int r, int c) const;
    bool isRowCompatible(GridRef const &) const;
    bool isColumnCompatible(GridRef const &) const;
    bool isConnectedRow(Locus const &) const;
    bool isConnectedColumn(Locus const &) const;
    void playTile(TileSquare const &);
    unsigned scoreRow(GridRef const &) const;
    unsigned scoreColumn(GridRef const &) const;
    
    public:
        // Board(void);  compiler-generated default constructor is OK
        // Board(BaseBoard const &);  compiler-generated copy constructor is OK
        // Board &operator=(Board const &)};  compiler-generated assignment method is OK
        // ~Board(void);  compiler-generated destructor is OK

        bool connectsToOrigin(GridRef const &, Locus &) const;
		bool isEmptyCell(GridRef const &) const;
        bool isLegalPlay(Play const &) const;
        void playTiles(Play const &);
        unsigned scorePlay(Play const &) const;
};

#endif
