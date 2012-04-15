#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

// File:     game.hpp
// Location: src
// Purpose:  declare Game class
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
 A Game object represents the state of a game in progress.

 The Game class owns various Tile objects, which are stored in three
 places:  a Board object, a Hands object, and a Tiles object (for the stock bag).
 The game history is stored using a Turns object and an iterator to
 indicate the current position therein.  The options used to create
 the game are saved in a GameOpt object.

*/

#include "board.hpp"  // HASA Board
#include "gameopt.hpp"// HASA GameOpt
#include "hands.hpp"  // HASA Hands
#include "turns.hpp"  // HASA Turns

enum EndingType {
	ENDING_NOT_OVER_YET,
	ENDING_WENT_OUT,
	ENDING_ALL_RESIGNED,
	ENDING_STUCK
};

class Game {
public:
	// public lifecycle
    Game(GameOpt const&, HandOpts const&);
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
	String        BestRunReport(void) const;
    unsigned      CountStock(void) const;
	String        EndBonus(void);
    void          FinishTurn(Move const&);
	unsigned      HandSize(void) const;
	unsigned      MustPlay(void) const;
    void          PlayGame(void);
	void          Redo(void);
	void          Restart(void);
	int           Seconds(Hand&) const;
	unsigned      SecondsPerHand(void) const;
	void          StartClock(void);
	void          StopClock(void);
    GameStyleType Style(void) const;
	void          TogglePause(void);
	void          Undo(void);
	Indices       UndoTiles(void) const;
    Hands         UnplayableHands(void) const;

	// public inquiry methods
	bool CanRedo(void) const;
	bool CanUndo(void) const;
	bool HasEmptyCell(Cell const&) const;
	bool HasUnsavedChanges(void) const;
	bool IsClockRunning(void) const;
    bool IsLegalMove(Move const&) const;
    bool IsLegalMove(Move const&, UmType& reason) const;
    bool IsLegalPartial(Partial const&) const;
	bool IsOutOfTime(void) const;
    bool IsOver(void) const;
	bool IsPaused(void) const;
	bool IsStockEmpty(void) const;

private:
	// private constants
	static const unsigned STUCK_THRESHOLD = 7;  // turns before game is declared "stuck" 

	// private data
	String           mBestRunReport;
    Board            mBoard;         // extensible playing surface
	String           mFilespec;      // associated file for load/save
	String           mFirstTurnMessage;
    Hands            mHands;         // all hands being played
	Turns            mHistory;       // history of turns for undo/redo
    unsigned         mMustPlay;      // min number of tiles to play, zero after the first turn
	GameOpt const    mOptions;
    Hands::Iterator miPlayableHand;  // whose turn it is
	Turns::Iterator miRedo;          // current position in the history
    Tiles            mStockBag;      // stock bag from which tiles are drawn
	bool             mUnsavedChanges;

	// private lifecycle
	Game(Game const&);  // not copyable

	// private operators
	Game& operator=(Game const&);  // not assignable

	// misc private methods
    void       AddTurn(Turn const&);
    void       DisplayScores(void) const;
    void       DisplayStatus(void) const;
	EndingType Ending(void) const;
	void       FindBestRun(void);
    void       FirstTurn(void);
    void       NextTurn(void);
	Strings    WinningHands(void) const;
	unsigned   WinningScore(void) const;
};
#endif // !defined(GAME_HPP_INCLUDED)
