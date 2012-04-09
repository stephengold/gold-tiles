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

#include "board.hpp"  // HASA Board
#include "gameopt.hpp"// HASA GameOpt
#include "hands.hpp"  // HASA Hands
#include "turns.hpp"  // HASA Turns

class Game {
public:
	// public lifecycle
    Game(GameOpt const &, HandOpts const &);
	// no default constructor
    // ~Game(void);  compiler-generated destructor is OK

	// public operators
    operator Board(void) const;
	operator GameOpt(void) const;
	operator Hand(void) const;   // the playable hand
	operator Hands(void) const;  // all hands

	// misc public methods
    void          ActivateNextHand(void);
    Tiles         ActiveTiles(void) const;
	unsigned      BestRunLength(void) const;
	String        BestRunReport(void) const;
    unsigned      CountStock(void) const;
	String        EndBonus(void);
    void          FinishTurn(Move const &);
	unsigned      HandSize(void) const;
    Hands         InactiveHands(void) const;
    void          PlayGame(void);
	void          Redo(void);
	void          Restart(void);
	int           Seconds(Hand &) const;
	unsigned      SecondsPerHand(void) const;
	void          StartClock(void);
	void          StopClock(void);
    GameStyleType Style(void) const;
	void          TogglePause(void);
	void          Undo(void);

	// public inquiry methods
	bool CanRedo(void) const;
	bool CanUndo(void) const;
	bool HasEmptyCell(Cell const &) const;
	bool HasUnsavedChanges(void) const;
	bool IsClockRunning(void) const;
    bool IsLegalMove(Move const &) const;
    bool IsLegalMove(Move const &, char const *&rReason) const;
    bool IsLegalPartial(Partial const &) const;
	bool IsOutOfTime(void) const;
    bool IsOver(void) const;
	bool IsPaused(void) const;
	bool IsStockEmpty(void) const;

private:
	// private data
    Hands::Iterator miActiveHand;    // whose turn it is
    unsigned         mBestRunLength; // zero after the first turn
	String           mBestRunReport;
    Board            mBoard;         // extensible playing surface
	String           mFilespec;      // associated file for load/save
	String           mFirstTurnMessage;
    Hands            mHands;         // all hands being played
	Turns            mHistory;       // history of turns for undo/redo
	GameOpt const    mOptions;
	Turns::Iterator miRedo;          // current position in the history
    Tiles            mStockBag;      // stock bag from which tiles are drawn
	bool             mUnsavedChanges;

	// private lifecycle
	Game(Game const &);  // not copyable

	// private operators
	Game &operator=(Game const &);  // not assignable

	// misc private methods
    void     AddTurn(Turn const &);
    void     DisplayScores(void) const;
    void     DisplayStatus(void) const;
	void     FindBestRun(void);
    void     FirstTurn(void);
    void     NextTurn(void);
	Strings  WinningHands(void) const;
	unsigned WinningScore(void) const;
};
#endif // !defined(GAME_HPP_INCLUDED)
