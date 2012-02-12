#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

// File:    game.hpp
// Purpose: Game class
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

#include "board.hpp"
#include "move.hpp"
#include "partial.hpp"
#include "players.hpp"

class Game {
public:
	// lifecycle
    Game(Strings playerNames,
             ACountType attributeCnt,
             AValueType maxAttributeValues[],
             unsigned tileRedundancy,
             unsigned handSize);
	// no default constructor
	// no copy constructor
    // ~Game(void);  compiler-generated destructor is OK

	// operators
	Game &operator=(Game const &) { ASSERT(false); };
    operator Board(void) const;

	// misc public methods
    void     ActivateNextPlayer(void);
    Tiles    ActiveHand(void) const;
    Player   ActivePlayer(void) const;
    unsigned CountStock(void) const;
    void     FinishTurn(Move const &);
    void     GoingOutBonus(void);
    Players  InactivePlayers(void) const;
    void     PlayGame(void);

	// public inquiry methods
	bool     HasEmptyCell(Cell const &) const;
	bool     HasUnsavedChanges(void) const;
    bool     IsLegalMove(Move const &) const;
    bool     IsLegalMove(Move const &, char const *&rReason) const;
    bool     IsLegalPartial(Partial const &) const;
    bool     IsOver(void) const;
	bool     IsStockEmpty(void) const;

private:
    Players::IteratorType miActivePlayer; // whose turn it is
    unsigned               mBestRunLength; // zero after the first turn
    Board                  mBoard;         // extensible playing surface
	String                 mFilename;      // name of associated file for load/save
    Players                mPlayers;       // who is playing
    Tiles                  mStockBag;      // stock bag from which tiles are drawn
	bool                   mUnsavedChanges;

	// misc methods
    void     AddTiles(AIndexType, unsigned redundancy, Tile &);
    Cell     ChooseCell(void) const;
    void     DisplayScores(void) const;
    void     FirstTurn(void);
    void     NextTurn(void);
    unsigned ScoreMove(Move const &) const;
};

#endif
