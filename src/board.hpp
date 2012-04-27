#ifndef BOARD_HPP_INCLUDED
#define BOARD_HPP_INCLUDED

// File:     board.hpp
// Location: src
// Purpose:  declare Board class
// Author:   Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
A Board object represents a two-dimensional playing surface
which may be extended in four directions.  The surface is
modeled as a grid of cells onto which Tile objects may be
played.  Cells are referenced by means of Cell objects.
A given cell may only be played on once.

The Board class extends the BaseBoard class.  The implementation of
BaseBoard is hidden from these extensions.
*/

#include "baseboard.hpp" // ISA BaseBoard
#include "move.hpp"      // USES Move

enum UmType { // user message handles
	UM_COLUMNCOMPAT,
	UM_DIAGCOMPAT,
	UM_EMPTY,
	UM_FIRST,
	UM_NEIGHBOR,
	UM_REPEATCELL,
	UM_REPEATTILE,
	UM_ROWCOLUMN,
	UM_ROWCOMPAT,
	UM_RULES,
	UM_START,
	UM_STARTSIMPLE,
	UM_STOCK,
	UM_SWAP
};

class Board: public BaseBoard {
public:
    // public lifecycle
    // Board(void);  compiler-generated default constructor is OK
    // Board(Board const&);  compiler-generated copy constructor is OK
    // ~Board(void);  compiler-generated destructor is OK

    // public operators
    // Board& operator=(Board const&);  compiler-generated assignment operator is OK 

	// misc public methods
    String        Description(void) const;
    Cell          FirstCell(void) const;
    Tile::IdType  GetId(Cell const&) const;
    void          Next(Cell&) const;
    void          PlayMove(Move const&);
    static String ReasonMessage(UmType, String& title);
    ScoreType     ScoreMove(Move const&) const;
    void          UnplayMove(Move const&);

	// public inquiry methods
    bool Contains(Tile::IdType) const;
    bool HasEmptyCell(Cell const&) const;
    bool HasNeighbor(Cell const&) const;
	bool IsEmpty(void) const;
    bool IsValidMove(Move const&) const;
    bool IsValidMove(Move const&, UmType& reason) const;
	bool MightUse(Cell const&) const;

private:
	// misc private methods
    Cells     GetRun(Cell const&, Direction const&) const;
    Tile      GetTile(Cell const&) const;
	Tiles     GetTiles(Cells const&) const;
    void      PlayTile(TileCell const&);
    ScoreType ScoreDirection(Cell const&, Direction const&) const;

	// private inquiry methods
    bool AreAllCompatible(Cells const&) const;
    bool AreAllEmpty(Cells const&) const;
    bool AreAllRunsCompatible(Cells const&, Direction const&) const;
    bool AreSingleConnectedRun(Cells const&, Direction const&) const;
    bool DoesAnyHaveNeighbor(Cells const&) const;
    bool IsRunCompatible(Cell const&, Direction const&) const;
};
#endif // !defined(BOARD_HPP_INCLUDED)
