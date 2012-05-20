#ifndef PARTIAL_HPP_INCLUDED
#define PARTIAL_HPP_INCLUDED

// File:     partial.hpp
// Location: src
// Purpose:  declare Partial class
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
A Partial object represents the playable hand's move in progress, including
the location of every tile in the hand.  It also decides which cells should
be "hinted" (that is to say highlighted) for the user's benefit.  It also
implements a Suggest() method for user assistance and automatic play.

The tiles may be located in the playable hand, in the swap area, or on the board.
At any instant, only one tile may be active (in motion).

The Partial class is ...
*/

#include "board.hpp"    // HASA Board
#include "cells.hpp"    // HASA Cells
#include "game.hpp"
#include "indices.hpp"  // HASA Indices
#include "tiles.hpp"    // HASA Tiles

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
    // public types
    typedef void YieldFunctionType(void*, bool& cancel);

    // public lifecycle
    // no default constructor
    // Partial(Partial const&);  implicitly defined copy constructor
    Partial(Game const*, HintType, Fraction const& skipProb);
    // ~Partial(void);

    // public operators
    // Partial& operator=(Partial const&);  implicitly defined assignment operator
    operator HintType(void) const;
    operator Tiles(void) const;

    // misc public methods
    void          Activate(Tile::IdType);
    void          BoardToHand(void);             // move the active tile
    SizeType      CountHand(void) const;
    SizeType      CountHinted(void);
    SizeType      CountPlayed(void) const;
    SizeType      CountSwap(void) const;
    SizeType      CountTiles(void) const;
    void          Deactivate(void);
    Cell          FirstHinted(void);
    GameStyleType GameStyle(void) const;
    Tile::IdType  GetActive(void) const;
    Tile::IdType  GetCellTile(Cell const&) const;
    Move          GetMove(bool includeActive) const;
    void          HandToCell(Cell const&);        // move the active tile
    void          HandToSwap(void);               // move the active tile
    Cell          LocateTile(void) const;
    Cell          LocateTile(Tile::IdType) const;
    ScoreType     Points(void) const;             // points scored so far this turn
    void          Reset(void);
    void          Reset(Fraction const& skipProb);
    void          Reset(Game const*, HintType, Fraction const& skipProb);
    void          SetHintStrength(HintType);
    static void   SetYield(YieldFunctionType*, void* arg);
    void          Suggest(void);
    void          SwapAll(void);
    void          SwapToHand(void);               // move the active tile

    // public inquiry methods
    bool CanRedo(void) const;
    bool CanSwapAll(void) const;
    bool CanUndo(void) const;
    bool HasGame(void) const;
    bool IsActive(Tile::IdType) const;
    bool IsGameOver(void) const;
    bool IsGamePaused(void) const;
    bool IsHinted(Cell const&);
    bool IsInHand(Tile::IdType) const;
    bool IsInSwap(Tile::IdType) const;
    bool IsLocalUsersTurn(void) const;
    bool IsPass(void) const;

protected:
    // protected data
    Game const* mpGame;
    Tiles        mTiles;  // the set of playable tiles

    // misc protected methods
    Board const& BoardReference(void) const;

    // protected inquiry methods
    bool Contains(Tile::IdType) const;
    bool IsEmpty(Cell const&) const;
    bool IsOnBoard(Tile::IdType) const;
    bool MightUse(Cell const&);

private:
    // private data
    Tile::IdType   mActiveId;         // tile actively being dragged (or else Tile::ID_NONE)
    Board          mBoard;
    Cells          mHintedCells;      // cached choice of cells
    bool           mHintedCellsValid;
    HintType       mHintStrength;
    SizeType       mPlayedTileCnt;    // number of tiles played to the board
    Fraction       mSkipProbability;  // reduces thoroughness of Suggest() method
    Indices        mSwapIds;          // indices of all tiles in the swap area
    static void* mspYieldArgument;
    static YieldFunctionType*
                 mspYieldFunction;

    // misc private methods
    void        AddValidNextUses(Move const&, Tile const&, Cells const&);
    void        FindBestMove(Partial&, ScoreType&) const;
    void        SetHintedCells(void);
    static void Yields(bool& cancel);

    // private inquiry methods
    bool IsValidNextStep(Move const&, Cell const&, Tile const&) const;
};
#endif  // !defined(PARTIAL_HPP_INCLUDED)
