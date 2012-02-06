#ifndef BOARD_HPP_INCLUDED
#define BOARD_HPP_INCLUDED

// File:    board.hpp
// Purpose: Board class
// Author:  Stephen Gold sgold@sonic.net
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

#include "baseboard.hpp"
#include "move.hpp"

class Board: public BaseBoard {
public:
	// misc
    void	 PlayMove(Move const &);
    unsigned ScoreMove(Move const &) const;

	// inquiry
	bool	 ConnectsToOrigin(Cell const &, Cells &) const;
    bool	 HasEmptyCell(Cell const &) const;
    bool	 IsLegalMove(Move const &) const;

private:
	// misc
	void		GetColumnLimits(Cell const &, int &rRow1, int &rRow2, int &rColumn) const;
	Tile const *GetPtr(int northing, int easting) const;
	void		GetRowLimits(Cell const &, int &rRow, int &rColumn1, int &rColumn2) const;
	Tile		GetTile(int northing, int easting) const;
	Tiles		GetTiles(Cells const &) const;
    void		PlayTile(TileCell const &);
    unsigned	ScoreColumn(Cell const &) const;
    unsigned	ScoreRow(Cell const &) const;

	// inquiry
    bool		AreAllColumnsCompatible(Cells const &) const;
    bool		AreAllCompatible(Cells const &) const;
    bool		AreAllEmpty(Cells const &) const;
    bool		AreAllRowsCompatible(Cells const &) const;
    bool		DoesAnyConnectToOrigin(Cells const &) const;
	bool		HasEmptyCell(int row, int column) const;
    bool		IsColumnCompatible(Cell const &) const;
    bool		IsConnectedColumn(Cells const &) const;
    bool		IsConnectedRow(Cells const &) const;
    bool		IsRowCompatible(Cell const &) const;
};

#endif
