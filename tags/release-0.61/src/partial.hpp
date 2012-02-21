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
A Partial object represents the active hand's move in progress, including
the location of every tile in the hand.  It also decides which cells should
be "hinted" (that is to say highlighted) for the player's benefit.

The tiles may be located in the active hand, in the swap area, or on the board.
At any instant, only one tile may be active (in motion).

The Partial class is ...
*/

#include "board.hpp"
#include "cells.hpp"
#include "indices.hpp"
#include "tiles.hpp"

enum HintType {
	HINT_NONE,
	HINT_EMPTY,
	HINT_CONNECTED,
	HINT_USABLE_ANY,
	HINT_USABLE_SELECTED,
	HINT_DEFAULT = HINT_USABLE_SELECTED,
	HINT_CHALLENGE_DEFAULT = HINT_EMPTY
};

class Partial {
public:
	// lifecycle
	Partial(Game const *, HintType strength = HINT_DEFAULT);
	// no default constructor
    //Partial(Partial const &);  compiler-generated copy constructor is OK
    //~Partial(void);
	void Reset(void);

	// operators
    //Partial &operator=(Partial const &);  compiler-generated assignment operator is OK
	operator Board(void) const;
	operator HintType(void) const;

	// misc public methods
	void       Activate(TileIdType);  // TODO Mobilize()
	void       BoardToHand(void);             // move the active tile
	unsigned   CountHand(void) const;
	unsigned   CountHinted(void);
	unsigned   CountPlayed(void) const;
	unsigned   CountSwap(void) const;
	unsigned   CountTiles(void) const;
	void       Deactivate(void);
	Cell       FirstHinted(void);
	TileIdType GetActive(void) const;
	TileIdType GetCell(Cell const &) const;
	Move       GetMove(bool includeActive) const;
	Tile       GetTileById(TileIdType) const;
	Tile       GetTileByIndex(unsigned) const;
	void       HandToCell(Cell const &);       // move the active tile
	void       HandToSwap(void);               // move the active tile
	Cell       LocateTile(void) const;
	Cell       LocateTile(TileIdType) const;
	void       SetHintStrength(HintType);
	void       SwapToHand(void);               // move the actilve tile
	
	// inquiry methods
	bool Contains(TileIdType) const;
	bool IsActive(TileIdType) const;
	bool IsEmpty(Cell const &) const;
	bool IsHinted(Cell const &);
	bool IsInHand(TileIdType) const;
	bool IsInSwap(TileIdType) const;
	bool IsOnBoard(TileIdType) const;
	bool IsPass(void) const;
	bool IsVisible(Cell const &);

private:
	TileIdType  mActiveId;        // tile actively being dragged (or else TILE_INVALID)
    Board       mBoard;
	Game const *mpGame;
	Cells       mHintedCells;     // cached choice of cells
	bool        mHintedCellsValid;
	HintType    mHintStrength;
	unsigned    mPlayedTileCnt;   // number of tiles played to the board
	Indices     mSwapIds;         // indices of all tiles in the swap area
	Tiles       mTiles;           // the set of all available tiles

	// misc private methods
	void AddValidNextUses(Move const &, Tile const &, Cells const &);
	void SetHintedCells(void);

	// private inquiry methods
	bool IsValidNextStep(Move const &, Cell const &, Tile const &) const;
};

#endif
