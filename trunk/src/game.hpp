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
#include "players.hpp"

class Game {
public:
	// lifecycle
    Game(void) { ASSERT(false); };
    Game(Strings player_names,
             ACountType attribute_cnt,
             AValueType max_attribute_values[],
             unsigned tile_redundancy,
             unsigned hand_size);
    Game(Game const &) { ASSERT(false); };
    // ~Game(void);  compiler-generated destructor is OK

	// operators
	Game &operator=(Game const &) { ASSERT(false); };
    operator Board(void) const;

	// misc methods
    void     ActivateNextPlayer(void);
    Player   ActivePlayer(void) const;
    unsigned CountStock(void) const;
    void     FinishTurn(Move const &);
    void     GoingOutBonus(void);
    Players  InactivePlayers(void) const;
    void     PlayGame(void);

	// inquiry methods
    bool     IsLegalMove(Move const &) const;
    bool     IsLegalMove(Move const &, char const *&rReason) const;
    bool     IsOver(void) const;
	bool     IsStockEmpty(void) const;

private:
    Players::IteratorType miActivePlayer; // whose turn it is
    unsigned               mBestRunLength; // zero after the first turn
    Board                  mBoard;         // extensible playing surface
    Players                mPlayers;       // who is playing
    Tiles                  mStockBag;      // stock bag from which tiles are drawn

	// misc methods
    void     AddTiles(AIndexType, unsigned redundancy, Tile &);
    Cell     ChooseCell(void) const;
    void     DisplayScores(void) const;
    void     FirstTurn(void);
    void     NextTurn(void);
    unsigned ScoreMove(Move const &) const;
};

#endif
