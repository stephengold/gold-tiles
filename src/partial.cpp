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

#include "game.hpp"
#include "partial.hpp"


// lifecycle

Partial::Partial(Game const *pGame, HintType strength) {
    Reset(pGame, strength);
}
// Partial(Partial const &);  compiler-generated copy constructor is OK
// ~Partial(void);  compiler-generated destructor is OK

void Partial::Reset(Game const *pGame, HintType strength) {
	mpGame = pGame;
    mHintStrength = strength;
    Reset();
}

// method invoked by takeback and at the start of a turn
void Partial::Reset(void) {
    mActiveId = Tile::ID_NONE;
    mHintedCellsValid = false;
    mPlayedTileCnt = 0;
	mSwapIds.MakeEmpty();
	if (HaveGame()) {
        mBoard = Board(*mpGame);
        mTiles = mpGame->ActiveTiles();
    } else {
	    mBoard.MakeEmpty();
		mTiles.MakeEmpty();
	}
}


// operators

// BaseBoard &operator=(BaseBoard const &);  compiler-generated assignment operator is OK

Partial::operator Board(void) const {
   return mBoard;
 }

Partial::operator HintType(void) const {
	return mHintStrength;
}


// misc methods

void Partial::Activate(TileIdType id) {
    ASSERT(id != Tile::ID_NONE);
    ASSERT(mActiveId == Tile::ID_NONE);
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
    Cells::ConstIterator i_cell;
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
    ASSERT(mActiveId != Tile::ID_NONE);
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

unsigned Partial::CountHinted(void) {
    if (!mHintedCellsValid) {
        SetHintedCells();
    }
    ASSERT(mHintedCellsValid);

    unsigned result = mHintedCells.Count();

	return result;
}

unsigned Partial::CountPlayed(void) const {
    return mPlayedTileCnt; 
}

unsigned Partial::CountSwap(void) const {
    unsigned result = mSwapIds.Count();
    
    ASSERT(result <= CountTiles());
    ASSERT(!HaveGame() || result <= mpGame->CountStock());

    return result; 
}

unsigned Partial::CountTiles(void) const {
    unsigned result = mTiles.Count();
    
    return result; 
}

void Partial::Deactivate(void) {
    if (mActiveId != Tile::ID_NONE) {
        mActiveId = Tile::ID_NONE;     
        mHintedCellsValid = false;
    }
}

Cell Partial::FirstHinted(void) {
    if (!mHintedCellsValid) {
        SetHintedCells();
    }
    ASSERT(mHintedCellsValid);
	ASSERT(!mHintedCells.IsEmpty());
    Cells::ConstIterator i_cell = mHintedCells.begin();
	Cell result = *i_cell;

	return result;
}

GameStyleType Partial::GameStyle(void) const {
    GameStyleType result = GAME_STYLE_NONE;
    if (HaveGame()) {
        result = mpGame->Style();
    }
    
    return result;
}

TileIdType Partial::GetActive(void) const {
    return mActiveId;
}

TileIdType Partial::GetCellTile(Cell const &rCell) const {
    TileIdType result = Tile::ID_NONE;
    
    Tile const *p_tile = mBoard.GetCell(rCell);
    if (p_tile != NULL) {
        result = p_tile->Id();
    }

    return result;
}
     
Move Partial::GetMove(bool includeActiveFlag) const {
    Move result;
    
    for (unsigned i = 0; i < CountTiles(); i++) {
        Tile tile = GetTileByIndex(i);
        TileIdType id = tile.Id();
		if (includeActiveFlag || !IsActive(id)) {
            Cell cell;
            if (mBoard.LocateTileId(id, cell)) {
                result.Add(tile, cell);
            } else if (IsInSwap(id)) {
                result.Add(tile);
			}
        }
    }

	return result;
}

Tile Partial::GetTileById(TileIdType id) const {
    ASSERT(id != Tile::ID_NONE);

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
    ASSERT(mActiveId != Tile::ID_NONE);
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
    ASSERT(mActiveId != Tile::ID_NONE);
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
    ASSERT(HaveGame());

	// start with no cells hinted
    mHintedCells.MakeEmpty();
	if (mHintStrength == HINT_NONE) {
        mHintedCellsValid = true;
		return;
	}

    // hint all valid empty cells (from start of turn)
	int row_fringe = 1;
	int column_fringe = 1;
	if (Cell::Grid() == GRID_HEX) {
		row_fringe = 2;
	}
    IndexType top_row = row_fringe + mBoard.NorthMax();
    IndexType bottom_row = -row_fringe - mBoard.SouthMax();
    IndexType right_column = column_fringe + mBoard.EastMax();
    IndexType left_column = -column_fringe - mBoard.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);
    for (IndexType row = top_row; row >= bottom_row; row--) {
        for (IndexType column = left_column; column <= right_column; column++) {
			if (Cell::IsValid(row, column)) {
                Cell cell(row, column);
                if (mpGame->HasEmptyCell(cell)) {
                    mHintedCells.Add(cell);
                }
			}
        }
    }
	if (mHintStrength == HINT_EMPTY) {
        mHintedCellsValid = true;
		return;
	}
    
    // unhint any cells that are not the start or the neighbor of a used cell
	Cells base = mHintedCells;
	mHintedCells.MakeEmpty();
	if (mBoard.IsEmpty()) {
        Cell start;
        mHintedCells.Add(start);
    } else {
        Cells::Iterator i_cell;
        for (i_cell = base.begin(); i_cell != base.end(); i_cell++) {
            Cell cell = *i_cell;
            if (mBoard.HasNeighbor(cell)) {
                mHintedCells.Add(cell);
            }
        }
    }
	if (mHintStrength == HINT_CONNECTED) {
        mHintedCellsValid = true;
		return;
	}

	// unhint cells not usable ...
	//   HINT_USABLE_ANY:       by any available tile
	//   HINT_USABLE_SELECTED:  by the selected tile

    base = mHintedCells;
    mHintedCells.MakeEmpty();
    Move move = GetMove(false);

	for (unsigned i = 0; i < CountTiles(); i++) {
        Tile tile = mTiles[i];
	    TileIdType id = tile.Id();
        bool include_tile = (!mBoard.ContainsId(id) || id == mActiveId);
        if (mHintStrength == HINT_USABLE_SELECTED && mActiveId != Tile::ID_NONE) {
            include_tile = (mActiveId == id);
        }
		if (include_tile) {
            AddValidNextUses(move, tile, base);
        }
    }
    
    mHintedCellsValid = true;
}

void Partial::SetHintStrength(HintType strength) {
	switch (strength) {
	    case HINT_NONE:
		case HINT_EMPTY:
		case HINT_CONNECTED:
		case HINT_USABLE_ANY:
		case HINT_USABLE_SELECTED:
		    break;
		default:
			FAIL();
	}

	if (mHintStrength != strength) {
        mHintedCellsValid = false;
	}
    mHintStrength = strength;
}

void Partial::SwapAll(void) {
    ASSERT(mActiveId == Tile::ID_NONE);
    ASSERT(CanSwapAll());

	for (unsigned i = 0; i < CountTiles(); i++) {
        Tile tile = mTiles[i];
	    TileIdType id = tile.Id();
		if (!mSwapIds.Contains(id)) {
	        mSwapIds.Add(id);
		}
	}
}

void Partial::SwapToHand(void) {
    ASSERT(mActiveId != Tile::ID_NONE);
	ASSERT(mSwapIds.Contains(mActiveId));
	
	mSwapIds.Remove(mActiveId);
    mHintedCellsValid = false;
	
	ASSERT(!mSwapIds.Contains(mActiveId));
}

// inquiry methods

bool Partial::CanSwapAll(void) const {
	bool result = false;
    if (HaveGame()) {
        result = (mpGame->CountStock() >= CountTiles());
    }

	return result;
}

bool Partial::Contains(TileIdType id) const {
     bool result = mTiles.ContainsId(id);     

     return result;
}

bool Partial::HaveGame(void) const {
     bool result = (mpGame != NULL);
     
     return result;
}

bool Partial::IsActive(TileIdType id) const {
    ASSERT(id != Tile::ID_NONE);
    bool result = (mActiveId == id);
    
    return result;
}

bool Partial::IsEmpty(Cell const &rCell) const {
    Tile const *p_tile = mBoard.GetCell(rCell);
    bool result = (p_tile == NULL);

    return result;
}

bool Partial::IsGameOver(void) const {
     bool result = false;
     if (HaveGame()) {
         result = mpGame->IsOver();
     }
     
     return result;
}

bool Partial::IsGamePaused(void) const {
	bool result = false;
	if (HaveGame() && !IsGameOver()) {
		result = mpGame->IsPaused();
	}

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

bool Partial::IsValidNextStep(
	Move const &rBase,
	Cell const &rCell,
	Tile const &rTile) const
{
	// Check whether a hypothetical next step would be legal.
	ASSERT(HaveGame());

	Move move = rBase;
	move.Add(rTile, rCell);
    char const *reason;
	bool result = mpGame->IsLegalMove(move, reason);

	if (!result && ::str_eq(reason, "FIRST")) {  
		// legal as a partial move.
	    result = true;
	}

	return result;
}

bool Partial::IsVisible(Cell const &rCell) {
	int fringe = 1;
	if (Cell::Grid() == GRID_HEX) {
		fringe = 2;
	}

    IndexType top_row = fringe + mBoard.NorthMax();
    IndexType bottom_row = -fringe - mBoard.SouthMax();
    IndexType right_column = fringe + mBoard.EastMax();
    IndexType left_column = -fringe - mBoard.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	IndexType row = rCell.Row();
	IndexType column = rCell.Column();

	bool result = (row >= bottom_row) 
		       && (row <= top_row)
			   && (column >= left_column)
			   && (column <= right_column);

	return result;
}
