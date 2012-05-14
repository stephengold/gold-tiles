// File:     game.cpp
// Location: src
// Purpose:  implement Game class
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

#include <iostream>
#include "direction.hpp"
#include "game.hpp"
#include "handopts.hpp"
#include "partial.hpp"
#include "strings.hpp"


// lifecycle

Game::Game(GameOpt const& rGameOpt, HandOpts const& rHandOptions):
	mOptions(rGameOpt)
{
	ASSERT(!rHandOptions.IsEmpty());

	// intialize static data of the Cell, Combo, and Tile classes
	Cell::SetStatic(mOptions);
	Combo::SetStatic(mOptions);
	Tile::SetStatic(mOptions);

	// add tiles to the stock bag
	for (unsigned i = 0; i < mOptions.TilesPerCombo(); i++) {
        // generate all possible tiles
		mStockBag.Restock();
	}
    std::cout << "Placed " << plural(CountStock(), "tile") << " in the stock bag." << std::endl;
    
    // generate list of unique player names
    Strings names = rHandOptions.AllPlayerNames();

    // construct hands and generate a unique name for each
	unsigned i = 0;
	for (unsigned i_hand = 0; i_hand < rHandOptions.Count(); i_hand++) {
		HandOpt options = rHandOptions[i_hand];

		String hand_name = options.PlayerName();
		unsigned const cnt = names.Count(hand_name);
		if (cnt > 1) {
		    hand_name = names.InventUnique(hand_name, "'s ", " hand");
		    names.Append(hand_name);
		}

		Hand const hand(hand_name, rHandOptions[i]);
	    mHands.Append(hand);
		i++;
    }

    // deal tiles to each hand from the stock bag
	unsigned const hand_size = mOptions.HandSize();
    Hands::Iterator i_hand;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
        Tiles const tiles = i_hand->DrawTiles(hand_size, mStockBag);

		// record the deal in mHistory
		String const name = i_hand->Name();
		Turn const turn(tiles, name);
		mHistory.push_back(turn);
    }
    std::cout << std::endl;

    // the hand with the best "run" gets the first turn
	FindBestRun();

	// set iterator to end of history
    miRedo = mHistory.end();

	// pretend this game has been saved, for convenience
	mUnsavedChanges = false;

	ASSERT(IsPaused());
	ASSERT(!CanUndo());
	ASSERT(!CanRedo());
}


// operators

Game::operator Board(void) const {
    return mBoard;
}

Game::operator GameOpt(void) const {
	return mOptions;
}

Game::operator Hand(void) const {
    Hand const result = *miPlayableHand;
    
    return result;
}

Game::operator Hands(void) const {
	return mHands;
}


// misc methods

void Game::ActivateNextHand(void) {
	ASSERT(!IsClockRunning());

    // skip over hands which have resigned
    mHands.NextWorking(miPlayableHand);
	mUnsavedChanges = true;

	ASSERT(!IsClockRunning());
}

// get copies of the tiles in the playable hand
Tiles Game::ActiveTiles(void) const {
    Tiles const result = Tiles(*miPlayableHand);
    
    return result;
}

void Game::AddTurn(Turn const& rTurn) {
    ASSERT(!IsClockRunning());

	while (miRedo != mHistory.end()) {
        miRedo = mHistory.erase(miRedo);
    }
    mHistory.push_back(rTurn);
    miRedo = mHistory.end();

	mUnsavedChanges = true;
}

String Game::BestRunReport(void) const {
	return mBestRunReport;
}

unsigned Game::CountStock(void) const {
    unsigned const result = mStockBag.Count();
    
    return result;
}

void Game::DisplayScores(void) const {
    Hands::ConstIterator i_hand;

	std::cout << std::endl;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
	    i_hand->DisplayScore();
    }
}

void Game::DisplayStatus(void) const {
    DisplayScores();
    unsigned const stock = CountStock();
    std::cout << std::endl
        << miPlayableHand->Name() << "'s turn, " 
		<< plural(stock, "tile") << " remaining in the stock bag"
        << std::endl << std::endl << String(mBoard) << std::endl;
}

String Game::EndBonus(void) {
    ASSERT(IsOver());

	String result;
	EndingType ending = Ending();

	if (ending == ENDING_WENT_OUT) {
	    // start writing the report
	    result += miPlayableHand->Name();
        result += " went out with ";
	    result += plural(miPlayableHand->Score(), "point");
	    result += ".\n\n";
    
	    // the playable hand scores a point for each tile in every other hand
        Hands::ConstIterator i_hand = miPlayableHand;
	    mHands.Next(i_hand);
        while (i_hand != miPlayableHand) {
            Tiles hand = Tiles(*i_hand);
            unsigned const tiles_in_hand = hand.Count();
		    ScoreType const points_in_hand = ScoreType(tiles_in_hand);  // TODO?

		    if (points_in_hand > 0) {
                miPlayableHand->AddScore(points_in_hand);

		        result += "Add ";
		        result += plural(points_in_hand, "point");
		        result += " for the tile";
		        result += plural(tiles_in_hand);
		        result += " held by ";
		        result += i_hand->Name();
		        result += ".\n";
		    }

		    mHands.Next(i_hand);
        }
	    result += "\n";
	}

	result += miPlayableHand->Name();
	result += " ended up with ";
	result += plural(miPlayableHand->Score(), "point");
	result += ".\n\n";

	if (mHands.Count() > 1) {
	    Strings const winning_hands = WinningHands();
	    if (winning_hands.Count() == 1) {
		    String const winner = winning_hands.First();
    	    result += winner;
	        result += " won the game.\n";
	    } else {
		    ASSERT(!winning_hands.IsEmpty());
		    String const winners(winning_hands, " and ");
    	    result += winners;
	        result += " tied for first place.\n";
		}
	}

	mUnsavedChanges = true;

	return result;
}

EndingType Game::Ending(void) const {
    EndingType result = ENDING_NOT_OVER_YET;  
/*
	 The game is over if and only if: 
	    (1) the stock bag is empty and a hand has gone out 
	 or (2) all hands have resigned
	 or (3) none of the last 7 moves placed any tiles on the board. 
 */
	if (IsStockEmpty() && mHands.HasAnyGoneOut()) {
        result = ENDING_WENT_OUT;
    } else if (mHands.HaveAllResigned()) {
		result = ENDING_ALL_RESIGNED;
	} else if (mHistory.IndexLastPlay() + STUCK_THRESHOLD == mHistory.Index(miRedo)) {
		result = ENDING_STUCK;
	}

    return result;
}

void Game::FindBestRun(void) {
    mMustPlay = 0;
	mBestRunReport = "";
    Hands::Iterator i_hand;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
        Tiles const run = i_hand->LongestRun();
	    unsigned const run_length = run.Count();

        mBestRunReport += i_hand->Name();
		mBestRunReport += " has a run of ";
		mBestRunReport += plural(run_length, "tile");
		mBestRunReport += ".\n";

	    if (run_length > mMustPlay) {
	   	    mMustPlay = run_length;
		    miPlayableHand = i_hand;
	    }
    }
	mBestRunReport += "\n";

	// make sure the tiles to be played will fit on the board
	Cell::LimitPlay(mMustPlay);

	mFirstTurnMessage = miPlayableHand->Name();
	mFirstTurnMessage += " plays first and must place "; 
	mFirstTurnMessage += plural(mMustPlay, "tile");
	mFirstTurnMessage += " on the (empty) board.\n";

	mBestRunReport += mFirstTurnMessage;
	std::cout << mBestRunReport;
}

void Game::FinishTurn(Move const& rMove) {
    ASSERT(mBoard.IsValidMove(rMove));
    ASSERT(IsClockRunning());
    
    StopClock();
    
    String const hand_name = miPlayableHand->Name();
    Turn turn(rMove, hand_name, mMustPlay);

	if (rMove.IsResign()) {
		miPlayableHand->Resign(mStockBag);

	} else {
        // remove played/swapped tiles from the hand
        Tiles const tiles = Tiles(rMove);
        miPlayableHand->RemoveTiles(tiles);

        // attempt to draw replacement tiles from the stock bag
        unsigned const count = tiles.Count();
	    if (count > 0) {
            Tiles draw = miPlayableHand->DrawTiles(count, mStockBag);
            ASSERT(draw.Count() == count || !rMove.InvolvesSwap());
		    turn.SetDraw(draw);
	    }

	    if (rMove.InvolvesSwap()) {
		    // return swapped tiles to the stock bag
		    mStockBag.AddTiles(tiles);
		    std::cout << miPlayableHand->Name() << " put " << plural(count, "tile")
			    << " back into the stock bag." << std::endl;

	    } else {
	        // place played tiles on the board
            mBoard.PlayMove(rMove);
    
            // update the hand's score    
  	        ScoreType const points = mBoard.ScoreMove(rMove);
	        miPlayableHand->AddScore(points);
	        turn.SetPoints(points);
		}
	}
	
	AddTurn(turn);
	
	//  If it was the first turn, it no longer is.
    mMustPlay = 0;

	// There are now unsaved changes.
	mUnsavedChanges = true;
    ASSERT(!IsClockRunning());
}

void Game::FirstTurn(void) {
    ASSERT(IsPaused());
    std::cout << std::endl;

    Move move;
    StartClock();
    if (miPlayableHand->IsAutomatic()) {
		ASSERT(!CanRedo());
		double const skip_probability = 0.0;
        Partial partial(this, HINT_NONE, skip_probability);
        partial.Suggest();
        move = partial.GetMove(false);
		std::cout << miPlayableHand->Name() << " played " << String(move) << std::endl;
	} else {
        for (;;) {
		    move = miPlayableHand->ChooseMove();
			UmType reason;
    	    if (IsLegalMove(move, reason)) {
                break;
            }
			String title;
			String const message = Board::ReasonMessage(reason, title);
			std::cout << message << std::endl << std::endl;
			std::cout << mFirstTurnMessage;
        }
	}
    ASSERT(IsLegalMove(move));

    ASSERT(!IsPaused());
    FinishTurn(move);
    ASSERT(!IsClockRunning());
}

unsigned Game::HandSize(void) const {
	unsigned const result = mOptions.HandSize();

	return result;
}

unsigned Game::MustPlay(void) const {
	return mMustPlay;
}

void Game::NextTurn(void) {
    ASSERT(IsPaused());

	StartClock();
    DisplayStatus();

    Move move;
    if (miPlayableHand->IsAutomatic()) {
		ASSERT(!CanRedo());
		double skip_probability = miPlayableHand->SkipProbability();
        Partial partial(this, HINT_NONE, skip_probability);
        partial.Suggest();
        move = partial.GetMove(false);
		std::cout << miPlayableHand->Name() << " played " << String(move) << std::endl;

	} else {
		ASSERT(miPlayableHand->IsLocalUser());
	    for (;;) {
		    move = miPlayableHand->ChooseMove();
			UmType reason;
    	    if (IsLegalMove(move, reason)) {
                break;
            }
			String title;
			String const message = Board::ReasonMessage(reason, title);
			std::cout << message << std::endl << std::endl;
			DisplayStatus();
        }
	}
    ASSERT(IsLegalMove(move));

    ASSERT(!IsPaused());
    FinishTurn(move);
    ASSERT(!IsClockRunning());
}

void Game::PlayGame(void) {
    FirstTurn();

    while (!IsOver()) {
        ActivateNextHand();
	    NextTurn();
    }

	String const report = EndBonus();
	std::cout << report;

    // display final scores
    DisplayScores();
}

void Game::Redo(void) {
	ASSERT(!IsClockRunning());
	ASSERT(CanRedo());

	Turn const turn = *miRedo;
	miRedo++;

	ASSERT(miPlayableHand->Name() == turn.HandName());

	Move const move = Move(turn);	
	if (move.IsResign()) {
		miPlayableHand->Resign(mStockBag);

	} else {
        // remove played/swapped tiles from the hand
        Tiles const tiles = Tiles(move);
        miPlayableHand->RemoveTiles(tiles);

        // draw replacement tiles from the stock bag
        Tiles const draw = turn.Draw();
		miPlayableHand->AddTiles(draw);
		mStockBag.RemoveTiles(draw);

	    if (move.InvolvesSwap()) {
		    // return swapped tiles to the stock bag
		    mStockBag.AddTiles(tiles);

	    } else {
	        // place played tiles on the board
            mBoard.PlayMove(move);
    
            // update the hand's score
			ScoreType const points = turn.Points();
			ASSERT(points == mBoard.ScoreMove(move));
	        miPlayableHand->AddScore(points);
		}
	}

	//  If it was the first turn, it no longer is.
    mMustPlay = 0;

	ActivateNextHand();

	ASSERT(!IsClockRunning());
}

void Game::Restart(void) {
	ASSERT(!IsClockRunning());

	// return all played tiles to the stock bag
	Tiles played_tiles = Tiles(mBoard);
	mStockBag.AddTiles(played_tiles);
	mBoard.MakeEmpty();

	// return all hand tiles to the stock bag
	Hands::Iterator i_hand;
	for (i_hand = mHands.begin(); i_hand != mHands.end(); i_hand++) {
		Tiles const hand_tiles = Tiles(*i_hand);
		mStockBag.AddTiles(hand_tiles);
		i_hand->Restart();
	}

	// redo the initial draws
	miRedo = mHistory.begin();
	for (unsigned i_hand = 0; i_hand < mHands.Count(); i_hand++) {
		ASSERT(miRedo->Points() == 0);

		Move const move = Move(*miRedo);
		ASSERT(move.IsPass());

		String const hand_name = miRedo->HandName();
		miPlayableHand = mHands.Find(hand_name);

		Tiles const draw_tiles = miRedo->Draw();
		ASSERT(draw_tiles.Count() == HandSize());

		miPlayableHand->AddTiles(draw_tiles);
		mStockBag.RemoveTiles(draw_tiles);

		miRedo++;
	}

    // the hand with the best "run" gets the first turn
	FindBestRun();

	ASSERT(IsPaused());
	ASSERT(!CanUndo());
}

int Game::Seconds(Hand& rHand) const {
	// read the clock of a particular hand
	int result = rHand.Seconds();

	if (mOptions.HasTimeLimit()) {
		// counting down
    	unsigned const time_limit = SecondsPerHand();
		result = time_limit - result;
	}

	return result;
}

unsigned Game::SecondsPerHand(void) const {
	unsigned const result = mOptions.SecondsPerHand();

	return result;
}

void Game::StartClock(void) {
	ASSERT(!IsOver());
	ASSERT(!IsClockRunning());

	miPlayableHand->StartClock();
}

void Game::StopClock(void) {
	ASSERT(IsClockRunning());

	miPlayableHand->StopClock();
}

GameStyleType Game::Style(void) const {
	GameStyleType const result = GameStyleType(mOptions);

    return result;
}

void Game::TogglePause(void) {
    ASSERT(!IsOver());

	if (IsClockRunning()) {
		StopClock();
	} else {
		StartClock();
	}
}

void Game::Undo(void) {
	ASSERT(!IsClockRunning());
	ASSERT(CanUndo());

	miRedo--;
	Turn const turn = *miRedo;

	String const hand_name = turn.HandName();
	miPlayableHand = mHands.Find(hand_name);

	//  Roll back the must-play info.
    mMustPlay = turn.MustPlay();

	Move const move = turn;
    Tiles const tiles = move;
	if (move.IsResign()) {
		miPlayableHand->Unresign(mStockBag, tiles);

	} else {
        // return drawn tiles to the stock bag
        Tiles const draw = turn.Draw();
		miPlayableHand->RemoveTiles(draw);
		mStockBag.AddTiles(draw);

		// add played/swapped tiles back into the hand
        miPlayableHand->AddTiles(tiles);

	    if (move.InvolvesSwap()) {
		    // remove swapped tiles from the stock bag
		    mStockBag.RemoveTiles(tiles);

	    } else {
	        // remove played tiles from the board
            mBoard.UnplayMove(move);
    
            // update the hand's score
			ScoreType const points = turn.Points();
	        miPlayableHand->SubtractScore(points);
		}
	}
}

Indices Game::UndoTiles(void) const {
	ASSERT(CanUndo());

	Turns::ConstIterator previous = miRedo;
	previous--;
	Move const move = Turn(*previous);

	Indices result;
	if (move.IsPlay()) {
	    result = Tiles(move);
	}

	return result;
}

Hands Game::UnplayableHands(void) const {
	// return a list of unplayable hands (including those which have resigned)
    Hands result;

    Hands::ConstIterator i_hand = miPlayableHand;
	mHands.Next(i_hand);
    
    while (i_hand != miPlayableHand) {
        result.push_back(*i_hand);
        mHands.Next(i_hand);
    }
    
    return result;
}

Strings Game::WinningHands(void) const {
	unsigned const winning_score = WinningScore();

	Strings result;

	Hands::ConstIterator i_hand;
	for (i_hand = mHands.begin(); i_hand != mHands.end(); i_hand++) {
		unsigned const score = i_hand->Score();
		if (score == winning_score) {
			String const name = i_hand->Name();
			result.Append(name);
		}
	}

	return result;
}

ScoreType Game::WinningScore(void) const {
	ScoreType const result = mHands.MaxScore();

	return result;
}


// inquiry methods

bool Game::CanRedo(void) const {
	bool const result = (miRedo != mHistory.end());

	return result;
}

bool Game::CanUndo(void) const {
	unsigned const turn = mHistory.Index(miRedo);
	bool const result = (turn > mHands.Count());

	return result;
}

bool Game::HasEmptyCell(Cell const& rCell) const {
    bool const result = mBoard.HasEmptyCell(rCell);
    
    return result; 
}

bool Game::HasUnsavedChanges(void) const {
	return mUnsavedChanges;
}

bool Game::IsClockRunning(void) const {
    bool const result = miPlayableHand->IsClockRunning();
     
    return result;
}

bool Game::IsLegalMove(Move const& rMove) const {
    UmType reason;
	bool const result = IsLegalMove(rMove, reason);

	return result;
}

bool Game::IsLegalMove(Move const& rMove, UmType& rReason) const {
    unsigned const stock = CountStock();
    bool result = true;
	unsigned const tiles_played = rMove.CountTilesPlayed();
    
	if (!mBoard.IsValidMove(rMove, rReason)) {
	    result = false;

	} else if (mMustPlay > 0
		    && !rMove.IsResign()
		    && tiles_played != mMustPlay)
	{
		// first turn but didn't resign, nor play the correct number of tiles
		ASSERT(tiles_played < mMustPlay);
	    rReason = UM_FIRST;
        result = false;

    } else if (rMove.IsPureSwap() && rMove.Count() > stock) {
		// swap but not enough tiles in stock
	    rReason = UM_STOCK;
        result = false;
    }

    return result;
}

bool Game::IsOutOfTime(void) const {
    bool result = false;

	if (mOptions.HasTimeLimit()) {
		MsecIntervalType const have_msec = MSECS_PER_SECOND * SecondsPerHand();
	    MsecIntervalType const used_msec = miPlayableHand->Milliseconds();

		result = (used_msec >= have_msec);
	}

	return result;
}

bool Game::IsOver(void) const {
	EndingType const ending = Ending();
	bool const result = (ending != ENDING_NOT_OVER_YET);

	return result;
}

bool Game::IsPaused(void) const {
	bool const result = !IsClockRunning() && !IsOver();

	return result;
}

bool Game::IsStockEmpty(void) const {
	bool const result = (CountStock() == 0);

	return result;
}