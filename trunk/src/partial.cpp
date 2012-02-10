// File:    partial.cpp
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

#include "project.hpp"
#include "cell.hpp"
#include "cells.hpp"
#include "game.hpp"
#include "move.hpp"
#include "partial.hpp"
#include "tile.hpp"
#include "tiles.hpp"


// lifecycle

Partial::Partial(Game const *pGame, unsigned hintStrength) {
	mpGame = pGame;
    mHintStrength = hintStrength;
    Reset();
}
// Partial(Partial const &);  compiler-generated copy constructor is OK
// ~Partial(void);  compiler-generated destructor is OK

void Partial::Reset(void) {
    mActiveId = 0;
    mHintedCellsValid = false;
    mPlayedTileCnt = 0;
	if (mpGame == NULL) {
	    mBoard.MakeEmpty();
		mTiles.MakeEmpty();
	} else {
        mBoard = Board(*mpGame);
        mTiles = mpGame->ActiveHand();
	}
}


// operators

// BaseBoard &operator=(BaseBoard const &);  compiler-generated assignment operator is OK

Partial::operator Board(void) const {
   Board result = mBoard;
   
   return result;
 }

Partial::operator Move(void) const {
    Move result;
    
    for (unsigned i = 0; i < CountTiles(); i++) {
        Tile tile = GetTileByIndex(i);
        TileIdType id = tile.Id();
        Cell cell;
        if (mBoard.LocateTileId(id, cell)) {
           result.Add(tile, cell);
        } else if (IsInSwap(id)) {
           result.Add(tile);
        }
    }

	return result;
}


// misc methods

void Partial::Activate(TileIdType id) {
    ASSERT(id != 0);
    ASSERT(mActiveId == 0);
    ASSERT(Contains(id));

    mActiveId = id;     
    mHintedCellsValid = false;
}

// Add cells from rBasis to the hint-set if they are valid next
// steps for the given Tile.
void Partial::AddValidNextUses(
    Move const &rMove, 
    Tile const &rTile, 
    Cells const &rBasis)
{
    Cells::ConstIteratorType i_cell;
    for (i_cell = rBasis.begin(); i_cell != rBasis.end(); i_cell++) {
        Cell cell = *i_cell;
        if (!mHintedCells.Contains(cell) 
         && IsValidNextStep(rMove, cell, rTile))
        {
      		mHintedCells.Add(cell);
        }
    }
}

void Partial::BoardToHand(void) {
    ASSERT(mActiveId != 0);
    ASSERT(IsOnBoard(mActiveId));
    ASSERT(!IsInHand(mActiveId));
    
    Cell cell = LocateTile(mActiveId);
    mBoard.MakeEmpty(cell);
    --mPlayedTileCnt;
    mHintedCellsValid = false;

	ASSERT(mBoard.HasEmptyCell(cell));
    ASSERT(!IsOnBoard(mActiveId));
    ASSERT(IsInHand(mActiveId));
}

unsigned Partial::CountHand(void) const {
    ASSERT(CountTiles() >= CountPlayed() + CountSwap());
    
    unsigned result = CountTiles() - CountPlayed() - CountSwap();
    
    return result; 
}

unsigned Partial::CountPlayed(void) const {
    unsigned result = mPlayedTileCnt;
    
    return result; 
}

unsigned Partial::CountSwap(void) const {
    unsigned result = mSwapIds.Count();
    
    ASSERT(result <= CountTiles());
    ASSERT(mpGame == NULL || result <= mpGame->CountStock());

    return result; 
}

unsigned Partial::CountTiles(void) const {
    unsigned result = mTiles.Count();
    
    return result; 
}

void Partial::Deactivate(void) {
    if (mActiveId != 0) {
        mActiveId = 0;     
        mHintedCellsValid = false;
    }
}

TileIdType Partial::GetActive(void) const {
    TileIdType result = mActiveId;

    return result;    
}

TileIdType Partial::GetCell(Cell const &rCell) const {
    TileIdType result = 0;
    
    Tile const *p_tile = mBoard.GetCell(rCell);
    if (p_tile != NULL) {
        result = p_tile->Id();
    }

    return result;
}
     
Tile Partial::GetTileById(TileIdType id) const {
    ASSERT(id != 0);

    Tile result;
     
    if (Contains(id)) {
        result = mTiles.FindTile(id);
    } else {
        Cell cell;
        bool found = mBoard.LocateTileId(id, cell);
        ASSERT(found);
        Tile const *p_tile = mBoard.GetCell(cell);
        ASSERT(p_tile != NULL);
        result = *p_tile;
    }
    
    ASSERT(result.Id() == id); 
    return result;                       
}

Tile Partial::GetTileByIndex(unsigned i) const {
    ASSERT(i < mTiles.Count());

    Tile result = mTiles[i];
    
    return result;                       
}

void Partial::HandToCell(Cell const &cell) {
    ASSERT(mActiveId != 0);
	ASSERT(IsInHand(mActiveId));
	ASSERT(mBoard.HasEmptyCell(cell));
	ASSERT(!IsOnBoard(mActiveId));

    Tile tile = mTiles.FindTile(mActiveId);
    mBoard.PlayOnCell(cell, tile);
    ++mPlayedTileCnt;
    mHintedCellsValid = false;
    
	ASSERT(!IsInHand(mActiveId));
    ASSERT(mBoard.GetCell(cell) != NULL);
    ASSERT(*(mBoard.GetCell(cell)) == tile);
}

void Partial::HandToSwap(void) {
    ASSERT(mActiveId != 0);
	ASSERT(!mSwapIds.Contains(mActiveId));
	
	mSwapIds.Add(mActiveId);
	mHintedCellsValid = false;
	
	ASSERT(mSwapIds.Contains(mActiveId));
}

Cell Partial::LocateTile(TileIdType id) const {
    Cell result;
    bool success = mBoard.LocateTileId(id, result);
    ASSERT(success);
    
    return result;
}

void Partial::SetHintedCells(void) {
    ASSERT(!mHintedCellsValid);
    mHintedCells.MakeEmpty();

    // for mHintStrength == 0, empty cells (from start of turn) are hinted
    int top_row = 1 + mBoard.NorthMax();
    int bottom_row = -1 - mBoard.SouthMax();
    int right_column = 1 + mBoard.EastMax();
    int left_column = -1 - mBoard.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);
    for (int row = top_row; row >= bottom_row; row--) {
        for (int column = left_column; column <= right_column; column++) {
            Cell cell(row, column);
            if (mpGame != NULL && mpGame->HasEmptyCell(cell)) {
                mHintedCells.Add(cell);
            }
        }
    }
    
    if (mHintStrength > 0) {
        // for mHintStrength == 1, only cells connected to the start are hinted

		Cells base = mHintedCells;
		mHintedCells.MakeEmpty();

        Cells::IteratorType i_cell;
        for (i_cell = base.begin(); i_cell != base.end(); i_cell++) {
            Cell cell = *i_cell;
            if (mBoard.ConnectsToStart(cell)) {
                mHintedCells.insert(cell);
            }
        }
    }

    if (mHintStrength > 1) {
        // for mHintStrength == 2, only cells usable with available tiles are hinted
        // for mHintStrength == 3, only cells usable with active tile are hinted
        Cells base = mHintedCells;
        mHintedCells.MakeEmpty();
	    Move move = Move(*this);

	    for (unsigned i = 0; i < CountTiles(); i++) {
            Tile tile = mTiles[i];
	        TileIdType id = tile.Id();
            bool include_tile = (!mBoard.ContainsId(id) || id == mActiveId);
            if (mHintStrength == 3 && mActiveId != 0) {
                include_tile = (mActiveId == id);
            }
            
		    if (include_tile) {
                 AddValidNextUses(move, tile, base);
            }
		}
    }
    
    mHintedCellsValid = true;
}

void Partial::SwapToHand(void) {
    ASSERT(mActiveId != 0);
	ASSERT(mSwapIds.Contains(mActiveId));
	
	mSwapIds.Remove(mActiveId);
    mHintedCellsValid = false;
	
	ASSERT(!mSwapIds.Contains(mActiveId));
}

// inquiry methods

bool Partial::IsActive(TileIdType id) const {
    bool result = (mActiveId == id);
    
    return result;
}

bool Partial::Contains(TileIdType id) const {
     bool result = mTiles.ContainsId(id);     

     return result;
}

bool Partial::IsHinted(Cell const &rCell) {
    if (!mHintedCellsValid) {
        SetHintedCells();
    }
    ASSERT(mHintedCellsValid);
    
	bool result = mHintedCells.Contains(rCell);
	
	return result;
}

bool Partial::IsInHand(TileIdType id) const {
    bool result = true;
    
    if (!mTiles.ContainsId(id)) {
        result = false;
    } else if (IsOnBoard(id)) {
        result = false;
    } else if (IsInSwap(id)) {
        result = false;
    }
    
    return result;
}

bool Partial::IsInSwap(TileIdType id) const {
    bool result = mSwapIds.Contains(id);
    
    return result;
}

bool Partial::IsOnBoard(TileIdType id) const {
    bool result = mBoard.ContainsId(id);
    
    return result;
}

bool Partial::IsPass(void) const {
    bool result = (CountPlayed() + CountSwap() == 0);

    return result;
}

bool Partial::IsValidNextStep(Move const &base, Cell const &rCell, Tile const &rTile) const {
	// Check whether a hypothetical next step would be legal.
	ASSERT(mpGame != NULL);

	Move move = base;
	move.Add(rTile, rCell);
    char const *reason;
	bool result = mpGame->IsLegalMove(move, reason);

	if (!result && ::strcmp(reason, "FIRST") == 0) {  
		// legal as a partial move.
	    result = true;
	}

	return result;
}
