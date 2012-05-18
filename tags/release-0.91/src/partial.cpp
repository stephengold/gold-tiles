// File:     partial.cpp
// Location: src
// Purpose:  implement Partial class
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

#include "game.hpp"
#include "partial.hpp"


// static data

void*                       Partial::mspYieldArgument = NULL;
Partial::YieldFunctionType* Partial::mspYieldFunction = NULL;


// lifecycle

Partial::Partial(
    Game const* pGame, 
    HintType strength, 
    Fraction const& rSkipProbability)
:
    mSkipProbability(rSkipProbability)
{
    Reset(pGame, strength, rSkipProbability);
}

// Partial(Partial const &);  implicitly defined copy constructor is OK
// ~Partial(void);  implicitly defined destructor is OK


// operators

// BaseBoard &operator=(BaseBoard const &);  implicitly defined assignment operator is OK

Partial::operator HintType(void) const {
    return mHintStrength;
}

Partial::operator Tiles(void) const {
    return mTiles;
}


// misc methods

void Partial::Activate(Tile::IdType id) {
    ASSERT(id != Tile::ID_NONE);
    ASSERT(mActiveId == Tile::ID_NONE);
    ASSERT(Contains(id));

    mActiveId = id;     
    mHintedCellsValid = false;
}

// Add cells from rBasis to the hint-set if they are valid next
// steps for the given Tile.
void Partial::AddValidNextUses(
    Move const& rMove, 
    Tile const& rTile, 
    Cells const& rBasis)
{
    Cells::ConstIterator i_cell;
    for (i_cell = rBasis.begin(); i_cell != rBasis.end(); i_cell++) {
        Cell const cell = *i_cell;
        if (!mHintedCells.Contains(cell) 
            && IsValidNextStep(rMove, cell, rTile))
        {
            mHintedCells.Add(cell);
        }
    }
}

Board const& Partial::BoardReference(void) const {
    return mBoard;
}

void Partial::BoardToHand(void) {
    ASSERT(mActiveId != Tile::ID_NONE);
    ASSERT(IsOnBoard(mActiveId));
    ASSERT(!IsInHand(mActiveId));

    Cell const cell = LocateTile(mActiveId);
    mBoard.MakeEmpty(cell);
    --mPlayedTileCnt;
    mHintedCellsValid = false;

    ASSERT(mBoard.HasEmptyCell(cell));
    ASSERT(!IsOnBoard(mActiveId));
    ASSERT(IsInHand(mActiveId));
}

unsigned Partial::CountHand(void) const {
    ASSERT(CountTiles() >= CountPlayed() + CountSwap());

    unsigned const result = CountTiles() - CountPlayed() - CountSwap();

    return result; 
}

unsigned Partial::CountHinted(void) {
    if (!mHintedCellsValid) {
        SetHintedCells();
    }
    ASSERT(mHintedCellsValid);

    unsigned const result = mHintedCells.Count();

    return result;
}

unsigned Partial::CountPlayed(void) const {
    return mPlayedTileCnt; 
}

unsigned Partial::CountSwap(void) const {
    unsigned const result = mSwapIds.Count();

    ASSERT(result <= CountTiles());
    ASSERT(!HasGame() || result <= mpGame->CountStock());

    return result; 
}

unsigned Partial::CountTiles(void) const {
    unsigned const result = mTiles.Count();

    return result; 
}

void Partial::Deactivate(void) {
    if (mActiveId != Tile::ID_NONE) {
        mActiveId = Tile::ID_NONE;     
        mHintedCellsValid = false;
    }
}

// RECURSIVE
void Partial::FindBestMove(Partial& rBest, ScoreType& rBestPoints) const {
    ASSERT(HasGame());

    Partial temp = *this; // make a temporary copy
    temp.Deactivate();
    temp.SetHintStrength(HINT_USABLE_SELECTED);
    ScoreType const points = temp.Points();

    if (points > rBestPoints) {        
        rBest = temp;
        rBestPoints = points;
    }

    Tiles::ConstIterator i_tile;
    for (i_tile = mTiles.begin(); i_tile != mTiles.end(); i_tile++) {
        Tile const tile = *i_tile;
        Tile::IdType const id = tile.Id();
        if (temp.IsInHand(id) && !mSkipProbability.RandomBool()) {
            bool canceled = false;

            Yields(canceled);
            if (canceled) {
                break;
            }

            temp.Activate(id);
            temp.SetHintedCells();
            Cells const cells = temp.mHintedCells;

            Cells::ConstIterator i_cell;
            for (i_cell = cells.begin(); i_cell != cells.end(); i_cell++) {
                Yields(canceled);
                if (canceled) {
                    break;
                }

                temp.HandToCell(*i_cell);
                temp.FindBestMove(rBest, rBestPoints);
                temp.BoardToHand();
            }
            temp.Deactivate();
        }    
    }
}

Cell Partial::FirstHinted(void) {
    if (!mHintedCellsValid) {
        SetHintedCells();
    }
    ASSERT(mHintedCellsValid);
    ASSERT(!mHintedCells.IsEmpty());
    Cells::ConstIterator const i_cell = mHintedCells.begin();
    Cell const result = *i_cell;

    return result;
}

GameStyleType Partial::GameStyle(void) const {
    GameStyleType result = GAME_STYLE_NONE;
    if (HasGame()) {
        result = mpGame->Style();
    }

    return result;
}

Tile::IdType Partial::GetActive(void) const {
    return mActiveId;
}

Tile::IdType Partial::GetCellTile(Cell const& rCell) const {
    Tile::IdType result = Tile::ID_NONE;

    Tile const* const p_tile = mBoard.GetCell(rCell);
    if (p_tile != NULL) {
        result = p_tile->Id();
    }

    return result;
}

Move Partial::GetMove(bool includeActiveFlag) const {
    Move result;

    Tiles::ConstIterator i_tile;
    for (i_tile = mTiles.begin(); i_tile != mTiles.end(); i_tile++) {
        Tile const tile = *i_tile;
        Tile::IdType const id = tile.Id();
        if (includeActiveFlag || !IsActive(id)) {
            Cell cell;
            if (mBoard.LocateTile(id, cell)) {
                result.Add(tile, cell);
            } else if (IsInSwap(id)) {
                result.AddSwapTile(tile);
            }
        }
    }

    return result;
}


void Partial::HandToCell(Cell const &cell) {
    ASSERT(mActiveId != Tile::ID_NONE);
    ASSERT(IsInHand(mActiveId));
    ASSERT(mBoard.HasEmptyCell(cell));
    ASSERT(!IsOnBoard(mActiveId));

    Tile const tile(mActiveId);
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

Cell Partial::LocateTile(Tile::IdType id) const {
    Cell result;
    bool const success = mBoard.LocateTile(id, result);
    ASSERT(success);

    return result;
}

ScoreType Partial::Points(void) const {
    unsigned const played_tile_cnt = CountPlayed();
    unsigned const must_play = mpGame->MustPlay();
    ScoreType result = 0;
    if (must_play == 0 || played_tile_cnt == must_play) {
        Move const move = GetMove(false);
        result = mBoard.ScoreMove(move);
    }

    return result;
}

void Partial::Reset(
    Game const* pGame, 
    HintType strength, 
    Fraction const& rSkipProbability)
{
    mpGame = pGame;
    mHintStrength = strength;
    Reset(rSkipProbability);
}

// method invoked at the start of a turn
void Partial::Reset(Fraction const& rSkipProbability) {
    ASSERT(rSkipProbability < 1.0);

    mSkipProbability = rSkipProbability;
    Reset();
}

// method invoked by takeback
void Partial::Reset(void) {
    mActiveId = Tile::ID_NONE;
    mHintedCellsValid = false;
    mPlayedTileCnt = 0;
    mSwapIds.MakeEmpty();
    if (HasGame()) {
        mBoard = *mpGame;
        mTiles = mpGame->ActiveTiles();
    } else {
        mBoard.MakeEmpty();
        mTiles.MakeEmpty();
    }
}

void Partial::SetHintedCells(void) {
    ASSERT(!mHintedCellsValid);
    ASSERT(HasGame());

    // start with no cells hinted
    mHintedCells.MakeEmpty();
    if (mHintStrength == HINT_NONE) {
        mHintedCellsValid = true;
        return;
    }

    // hint all valid empty cells (from start of turn)
    for (Cell cell = mBoard.FirstCell(); mBoard.MightUse(cell); mBoard.Next(cell)) {
        Cell wrap_cell = cell;
        if (Cell::DoesBoardWrap()) {
            wrap_cell.Wrap();
        }
        if (wrap_cell.IsValid() 
            && mpGame->HasEmptyCell(wrap_cell)
            && !mHintedCells.Contains(wrap_cell)) {
                mHintedCells.Add(wrap_cell);
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
        Cell const start_cell;
        mHintedCells.Add(start_cell);
    } else {
        Cells::Iterator i_cell;
        for (i_cell = base.begin(); i_cell != base.end(); i_cell++) {
            Cell const cell = *i_cell;
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
    Move const move = GetMove(false);

    Tiles::ConstIterator i_tile;
    for (i_tile = mTiles.begin(); i_tile != mTiles.end(); i_tile++) {
        Tile const tile = *i_tile;
        Tile::IdType const id = tile.Id();
        bool include_tile;
        if (mHintStrength == HINT_USABLE_SELECTED && mActiveId != Tile::ID_NONE) {
            include_tile = (id == mActiveId);
        } else {
            include_tile = (!mBoard.Contains(id) || id == mActiveId);
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
        mHintStrength = strength;
    }
}

/*
Set up a callback to be invoked periodically during long-running operations.
Currently used only in FindBestMove(), by way of Yields().
*/
/* static */ void Partial::SetYield(
    YieldFunctionType* pFunction,
    void* pArgument)
{
    mspYieldArgument = pArgument;
    mspYieldFunction = pFunction;
}

void Partial::Suggest(void) {
    ASSERT(HasGame());

    Reset();
    Partial best = *this; // make a copy
    ScoreType best_score = 0;
    FindBestMove(best, best_score);
    if (best_score == 0) {
        if (CanSwapAll()) {
            SwapAll();
        } // TODO - partial swaps
    } else {
        best.SetHintStrength(mHintStrength);
        *this = best;
        mHintedCellsValid = false;
    }
}

void Partial::SwapAll(void) {
    ASSERT(mActiveId == Tile::ID_NONE);
    ASSERT(CanSwapAll());

    Tiles::ConstIterator i_tile;
    for (i_tile = mTiles.begin(); i_tile != mTiles.end(); i_tile++) {
        Tile const tile = *i_tile;
        Tile::IdType const id = tile.Id();
        if (!mSwapIds.Contains(id)) {
            mSwapIds.Add(id);
        }
    }

    mHintedCellsValid = false;
}

void Partial::SwapToHand(void) {
    ASSERT(mActiveId != Tile::ID_NONE);
    ASSERT(mSwapIds.Contains(mActiveId));

    mSwapIds.Remove(mActiveId);
    mHintedCellsValid = false;

    ASSERT(!mSwapIds.Contains(mActiveId));
}

/* static */ void Partial::Yields(bool& rCanceled) {
    if (mspYieldFunction != NULL) {
        (*mspYieldFunction)(mspYieldArgument, rCanceled);
    }
}


// inquiry methods

bool Partial::CanRedo(void) const {
    bool result = false;
    if (HasGame()) {
        result = mpGame->CanRedo();
    }

    return result;
}

bool Partial::CanSwapAll(void) const {
    bool result = false;
    if (HasGame()) {
        result = (mpGame->CountStock() >= CountTiles());
    }

    return result;
}

bool Partial::CanUndo(void) const {
    bool result = false;
    if (HasGame()) {
        result = mpGame->CanUndo();
    }

    return result;
}

bool Partial::Contains(Tile::IdType id) const {
    bool const result = mTiles.Contains(id);     

    return result;
}

bool Partial::HasGame(void) const {
    bool const result = (mpGame != NULL);

    return result;
}

bool Partial::IsActive(Tile::IdType id) const {
    bool const result = (mActiveId == id);

    return result;
}

bool Partial::IsEmpty(Cell const& rCell) const {
    ASSERT(rCell.IsValid());

    Tile const* const p_tile = mBoard.GetCell(rCell);
    bool const result = (p_tile == NULL);

    return result;
}

bool Partial::IsGameOver(void) const {
    bool result = false;
    if (HasGame()) {
        result = mpGame->IsOver();
    }

    return result;
}

bool Partial::IsGamePaused(void) const {
    bool result = false;
    if (HasGame() && !IsGameOver()) {
        result = mpGame->IsPaused();
    }

    return result;
}

bool Partial::IsHinted(Cell const& rCell) {
    ASSERT(rCell.IsValid());

    bool result = false;

    if (IsLocalUsersTurn()) {
        if (!mHintedCellsValid) {
            SetHintedCells();
        }
        ASSERT(mHintedCellsValid);
        result = mHintedCells.Contains(rCell);
    }

    return result;
}

bool Partial::IsInHand(Tile::IdType id) const {
    bool result = mTiles.Contains(id);

    if (IsOnBoard(id)) {
        result = false;
    } else if (IsInSwap(id)) {
        result = false;
    }

    return result;
}

bool Partial::IsInSwap(Tile::IdType id) const {
    bool const result = mSwapIds.Contains(id);

    return result;
}

bool Partial::IsLocalUsersTurn(void) const {
    bool result = false;
    if (HasGame() && !IsGameOver()) {
        Hand const hand = Hand(*mpGame);
        result = hand.IsLocalUser();
    }

    return result;
}

bool Partial::IsOnBoard(Tile::IdType id) const {
    bool const result = mBoard.Contains(id);

    return result;
}

bool Partial::IsPass(void) const {
    bool const result = (CountPlayed() + CountSwap() == 0);

    return result;
}

bool Partial::IsValidNextStep(
    Move const& rBase,
    Cell const& rCell,
    Tile const& rTile) const
{
    // Check whether a hypothetical next step would be legal.
    ASSERT(HasGame());

    Move move = rBase;
    move.Add(rTile, rCell);
    UmType reason;
    bool result = mpGame->IsLegalMove(move, reason);

    if (!result && reason == UM_FIRST) {  
        // legal as a partial move.
        result = true;
    }

    return result;
}

bool Partial::MightUse(Cell const& rCell) {
    bool const result = mBoard.MightUse(rCell);

    return result;
}

