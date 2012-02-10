#ifndef PARTIAL_HPP_INCLUDED
#define PARTIAL_HPP_INCLUDED

// File:    partial.hpp
// Purpose: Partial class
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
A Partial object represents the active player's move in progress.

The Partial class is ...
*/

#include "board.hpp"
#include "cells.hpp"
#include "indices.hpp"
#include "partial.hpp"
#include "tiles.hpp"

class Partial {
public:
	// lifecycle
	Partial(void) { ASSERT(false); };
	Partial(Game const *, unsigned hintStrength);
    //Partial(Partial const &);  compiler-generated copy constructor is OK
    //~Partial(void);
	void Reset(void);

	// operators
    //Partial &operator=(Partial const &);  compiler-generated assignment operator is OK
	operator Board(void) const;
	operator Move(void) const;

	// misc public methods
	void       Activate(TileIdType);
	void       BoardToHand(void);
	unsigned   CountHand(void) const;
	unsigned   CountSwap(void) const;
	unsigned   CountPlayed(void) const;
	unsigned   CountTiles(void) const;
	void       Deactivate(void);
	TileIdType GetActive(void) const;
	TileIdType GetCell(Cell const &) const;
	Tile       GetTileById(TileIdType) const;
	Tile       GetTileByIndex(unsigned) const;
	void       HandToCell(Cell const &);
	void       HandToSwap(void);
	Cell       LocateTile(void) const;
	Cell       LocateTile(TileIdType) const;
	void       SwapToHand(void);
	
	// inquiry methods
	bool Contains(TileIdType) const;
	bool IsActive(TileIdType) const;
	bool IsHinted(Cell const &rCell);
	bool IsInHand(TileIdType) const;
	bool IsInSwap(TileIdType) const;
	bool IsOnBoard(TileIdType) const;
	bool IsPass(void) const;

private:
	TileIdType  mActiveId;
    Board       mBoard;
	Game const *mpGame;
	Cells       mHintedCells;
	bool        mHintedCellsValid;
	unsigned    mHintStrength;
	unsigned    mPlayedTileCnt;
	Indices     mSwapIds;
	Tiles       mTiles;

	// misc private methods
	void AddValidNextUses(Move const &, Tile const &, Cells const &);
	void SetHintedCells(void);

	// private inquiry methods
	bool IsValidNextStep(Move const &, Cell const &, Tile const &) const;
};

#endif
