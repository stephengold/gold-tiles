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
#include "hands.hpp"
#include "move.hpp"
#include "partial.hpp"

enum GameStyleType {
	GAME_STYLE_DEBUG = 0,    // allows peeking, undo, all hints; clock is optional
	GAME_STYLE_PRACTICE = 1, // no peeking; allows undo, all hints; clock is optional
	GAME_STYLE_FRIENDLY = 2, // no peeking, no undo; allows all hints; clock is optional
	GAME_STYLE_CHALLENGE = 3 // no peeking, no undo, no hints; time limits
};

class Game {
public:
	// lifecycle
    Game(Strings handNames,
		 unsigned tileRedundancy = TILE_REDUNDANCY_DEFAULT, 
		 unsigned handSize = HAND_SIZE_DEFAULT, 
		 unsigned secondsPerHand = TIME_UNLIMITED);
	// no default constructor
	// no copy constructor
    // ~Game(void);  compiler-generated destructor is OK

	// operators
	Game &operator=(Game const &) { FAIL(); };
    operator Board(void) const;
	operator Hand(void) const;

	// misc public methods
    void     ActivateNextHand(void);
    Tiles    ActiveTiles(void) const;
    unsigned CountStock(void) const;
    void     FinishTurn(Move const &);
    void     GoingOutBonus(void);
	unsigned HandSize(void) const;
    Hands    InactiveHands(void) const;
    void     PlayGame(void);
	unsigned Redundancy(void) const;
	int      Seconds(Hand &) const;
	unsigned SecondsPerHand(void) const;
	void     StartClock(void);
	void     StopClock(void);

	// public inquiry methods
	bool     HasEmptyCell(Cell const &) const;
	bool     HasUnsavedChanges(void) const;
    bool     IsLegalMove(Move const &) const;
    bool     IsLegalMove(Move const &, char const *&rReason) const;
    bool     IsLegalPartial(Partial const &) const;
    bool     IsOver(void) const;
	bool     IsPaused(void) const;
	bool     IsStockEmpty(void) const;

	// constants
    static const unsigned HAND_SIZE_DEFAULT = 6;
    static const unsigned TILE_REDUNDANCY_DEFAULT = 1;
	static const unsigned TIME_UNLIMITED = 0;

private:
    Hands::IteratorType miActiveHand;    // whose turn it is
    unsigned             mBestRunLength; // zero after the first turn
    Board                mBoard;         // extensible playing surface
	String               mFilename;      // name of associated file for load/save
    Hands                mHands;         // all hands being played
	unsigned             mHandSize;      // max tiles per hand
	unsigned             mRedundancy;    // number of instances of each possible tile
	unsigned             mSecondsPerHand; // 0 indicates count up instead of down
    Tiles                mStockBag;      // stock bag from which tiles are drawn
	bool                 mUnsavedChanges;

	// misc private methods
    void     AddTiles(AIndexType, Tile &);
    Cell     ChooseCell(void) const;
    void     DisplayScores(void) const;
    void     FirstTurn(void);
    void     NextTurn(void);
    unsigned ScoreMove(Move const &) const;
};

#endif
