// File:    game.cpp
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

#include <iostream>
#include "board.hpp"
#include "game.hpp"
#include "partial.hpp"
#include "strings.hpp"


// lifecycle

Game::Game(
    Strings const &rPlayerNames,
	Indices const &rAutoFlags,
    GameStyleType style,
    unsigned tileRedundancy,
    unsigned handSize,
	unsigned secondsPerHand)
{
	ASSERT(!rPlayerNames.IsEmpty());
	ASSERT(style != GAME_STYLE_NONE);
	ASSERT(tileRedundancy >= 1);
	ASSERT(handSize >= 1);

    // save parameters
	mStyle = style;
	mRedundancy = tileRedundancy;
	mHandSize = handSize;
	mSecondsPerHand = secondsPerHand;

	// add tiles to the stock bag
	for (unsigned i = 0; i < mRedundancy; i++) {
        // generate all possible tiles
		mStockBag.Restock();
	}
    std::cout << "Placed " << plural(CountStock(), "tile") << " in the stock bag." << std::endl;
    
    // construct hands and generate a unique name for each one
    Strings unique = rPlayerNames.Unique();
	Strings::ConstIterator i_player;
	unsigned i = 0;
	for (i_player = rPlayerNames.Begin(); i_player != rPlayerNames.End(); i_player++) {
		String player_name = *i_player;

		String hand_name = player_name;
		if (rPlayerNames.Count(player_name) > 1) {
		    hand_name = unique.InventUnique(player_name, "'s ", " hand");
		    unique.Append(hand_name);
		}

		Hand hand(hand_name, player_name, rAutoFlags.Contains(i));
	    mHands.push_back(hand);
		i++;
    }

    // deal tiles to each hand from the stock bag
    Hands::Iterator i_hand;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
        Tiles tiles = i_hand->DrawTiles(mHandSize, mStockBag);

		// record the deal in mHistory
		String name = i_hand->Name();
		Turn turn(tiles, name);
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

Game::operator Hand(void) const {
    Hand result = *miActiveHand;
    
    return result;
}

Game::operator Hands(void) const {
	return mHands;
}


// misc methods

void Game::ActivateNextHand(void) {
	ASSERT(!IsClockRunning());

    // skip over hands which have resigned
    mHands.NextWorking(miActiveHand);
	mUnsavedChanges = true;

	ASSERT(!IsClockRunning());
}

// get copies of the tiles in the active hand
Tiles Game::ActiveTiles(void) const {
    Tiles result = Tiles(*miActiveHand);
    
    return result;
}

void Game::AddTurn(Turn const &rTurn) {
    while (miRedo != mHistory.end()) {
        miRedo = mHistory.erase(miRedo);
    }
    mHistory.push_back(rTurn);
    miRedo = mHistory.end();

	mUnsavedChanges = true;
}

unsigned Game::CountStock(void) const {
    unsigned result = mStockBag.Count();
    
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
    unsigned stock = CountStock();
    std::cout << std::endl
        << miActiveHand->Name() << "'s turn, " 
		<< plural(stock, "tile") << " remaining in the stock bag"
        << std::endl << std::endl << String(mBoard) << std::endl;
}

void Game::FindBestRun(void) {
    Hands::Iterator i_hand;
    mBestRunLength = 0;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
		String hand_name = i_hand->Name();
        Tiles run = i_hand->LongestRun();
	    unsigned run_length = run.Count();
        std::cout << hand_name << " has a run of " << plural(run_length, "tile") 
			      << "." << std::endl;

	    if (run_length > mBestRunLength) {
	   	    mBestRunLength = run_length;
		    miActiveHand = i_hand;
	    }
    }
}

void Game::FinishTurn(Move const &rMove) {
    ASSERT(mBoard.IsValidMove(rMove));
    ASSERT(IsClockRunning());
    
    StopClock();
    
    String hand_name = miActiveHand->Name();
    Turn turn(rMove, hand_name, mBestRunLength);

	if (rMove.IsResign()) {
		miActiveHand->Resign(mStockBag);

	} else {
        // remove played/swapped tiles from the hand
        Tiles tiles = Tiles(rMove);
        miActiveHand->RemoveTiles(tiles);

        // attempt to draw replacement tiles from the stock bag
        unsigned count = tiles.Count();
	    if (count > 0) {
            Tiles draw = miActiveHand->DrawTiles(count, mStockBag);
            ASSERT(draw.Count() == count || !rMove.InvolvesSwap());
		    turn.SetDraw(draw);
	    }

	    if (rMove.InvolvesSwap()) {
		    // return swapped tiles to the stock bag
		    mStockBag.AddTiles(tiles);
		    std::cout << miActiveHand->Name() << " put " << plural(count, "tile")
			    << " back into the stock bag." << std::endl;

	    } else {
	        // place played tiles on the board
            mBoard.PlayMove(rMove);
    
            // update the hand's score    
  	        unsigned points = mBoard.ScoreMove(rMove);
	        miActiveHand->AddScore(points);
	        turn.SetPoints(points);
		}
	}
	
	AddTurn(turn);
	
	//  If it was the first turn, it no longer is.
    mBestRunLength = 0;

	// There are now unsaved changes.
	mUnsavedChanges = true;
    ASSERT(!IsClockRunning());
}

void Game::FirstTurn(void) {
    ASSERT(IsPaused());
    std::cout << std::endl;

    Move move;
    StartClock();
    for (;;) {
  	    std::cout << miActiveHand->Name() << " plays first and must place " 
			<< plural(mBestRunLength, "tile") << " on the (empty) board." 
            << std::endl;
        if (miActiveHand->IsAutomatic()) {
			Tiles run = miActiveHand->LongestRun();
            move = Move(run);
		    std::cout << miActiveHand->Name() << " played " << String(move) << std::endl;
			break;
        } else {
		    move = miActiveHand->ChooseMove();
			char const *reason;
    	    if (IsLegalMove(move, reason)) {
                break;
            }
			String title;
			String message = Board::ReasonMessage(reason, title);
			std::cout << message << std::endl << std::endl;
        }
	}
    ASSERT(IsLegalMove(move));

    ASSERT(!IsPaused());
    FinishTurn(move);
    ASSERT(!IsClockRunning());
}

void Game::GoingOutBonus(void) {
    ASSERT(IsOver());
    
	// the active hand scores a point for each tile in every other hand
    Hands::ConstIterator i_hand = miActiveHand;
	mHands.Next(i_hand);
    
    while (i_hand != miActiveHand) {
        Tiles hand = Tiles(*i_hand);
        unsigned pointsInHand = hand.Count(); // TODO
        miActiveHand->AddScore(pointsInHand);
		mHands.Next(i_hand);
    }
	mUnsavedChanges = true;
}

unsigned Game::HandSize(void) const {
	ASSERT(mHandSize > 0);

	return mHandSize;
}

Hands Game::InactiveHands(void) const {
	// return a list of inactive hands (including those which have resigned)
    Hands result;

    Hands::ConstIterator i_hand = miActiveHand;
	mHands.Next(i_hand);
    
    while (i_hand != miActiveHand) {
        result.push_back(*i_hand);
        mHands.Next(i_hand);
    }
    
    return result;
}

void Game::NextTurn(void) {
    ASSERT(IsPaused());

	StartClock();
    DisplayStatus();

    Move move;
    if (miActiveHand->IsAutomatic()) {
		ASSERT(!CanRedo());
        Partial partial(this, HINT_NONE);
        partial.Suggest();
        move = partial.GetMove(false);
		std::cout << miActiveHand->Name() << " played " << String(move) << std::endl;

	} else {
		ASSERT(miActiveHand->IsLocalPlayer());
	    for (;;) {
		    move = miActiveHand->ChooseMove();
			char const *reason;
    	    if (IsLegalMove(move, reason)) {
                break;
            }
			String title;
			String message = Board::ReasonMessage(reason, title);
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

    // display final scores
    DisplayScores();
}

void Game::Redo(void) {
	ASSERT(!IsClockRunning());
	ASSERT(CanRedo());

	Turn turn = *miRedo;
	miRedo++;

	ASSERT(miActiveHand->Name() == turn.HandName());

	Move move = Move(turn);	
	if (move.IsResign()) {
		miActiveHand->Resign(mStockBag);

	} else {
        // remove played/swapped tiles from the hand
        Tiles tiles = Tiles(move);
        miActiveHand->RemoveTiles(tiles);

        // draw replacement tiles from the stock bag
        Tiles draw = turn.Draw();
		miActiveHand->AddTiles(draw);
		mStockBag.RemoveTiles(draw);

	    if (move.InvolvesSwap()) {
		    // return swapped tiles to the stock bag
		    mStockBag.AddTiles(tiles);

	    } else {
	        // place played tiles on the board
            mBoard.PlayMove(move);
    
            // update the hand's score
			unsigned points = turn.Points();
			ASSERT(points == mBoard.ScoreMove(move));
	        miActiveHand->AddScore(points);
		}
	}

	//  If it was the first turn, it no longer is.
    mBestRunLength = 0;

	ActivateNextHand();

	ASSERT(!IsClockRunning());
}

unsigned Game::Redundancy(void) const {
	ASSERT(mRedundancy > 0);

	return mRedundancy;
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
		Tiles hand_tiles = Tiles(*i_hand);
		mStockBag.AddTiles(hand_tiles);
		i_hand->Restart();
	}

	// redo the initial draws
	miRedo = mHistory.begin();
	for (unsigned i_hand = 0; i_hand < mHands.Count(); i_hand++) {
		ASSERT(miRedo->Points() == 0);

		Move move = Move(*miRedo);
		ASSERT(move.IsPass());

		String hand_name = miRedo->HandName();
		miActiveHand = mHands.Find(hand_name);

		Tiles draw_tiles = miRedo->Draw();
		ASSERT(draw_tiles.Count() == mHandSize);

		miActiveHand->AddTiles(draw_tiles);
		mStockBag.RemoveTiles(draw_tiles);

		miRedo++;
	}

    // the hand with the best "run" gets the first turn
	FindBestRun();

	ASSERT(IsPaused());
	ASSERT(!CanUndo());
}

int Game::Seconds(Hand &rHand) const {
	// read the clock of a particular hand
	int result = rHand.Seconds(); 
	if (mSecondsPerHand > 0) {
		result = mSecondsPerHand - result;
		if (result < 0) {
			// TODO
		}
	}

	return result;
}

unsigned Game::SecondsPerHand(void) const {
	return mSecondsPerHand;
}

void Game::StartClock(void) {
	ASSERT(!IsOver());
	miActiveHand->StartClock();
}

void Game::StopClock(void) {
	miActiveHand->StopClock();
}

GameStyleType Game::Style(void) const {
    return mStyle;
}

void Game::Undo(void) {
	ASSERT(!IsClockRunning());
	ASSERT(CanUndo());

	miRedo--;
	Turn turn = *miRedo;

	String hand_name = turn.HandName();
	miActiveHand = mHands.Find(hand_name);

	//  Roll back the first-move info.
    mBestRunLength = turn.BestRun();

	Move move = Move(turn);
    Tiles tiles = Tiles(move);
	if (move.IsResign()) {
		miActiveHand->Unresign(mStockBag, tiles);

	} else {
        // return drawn tiles to the stock bag
        Tiles draw = turn.Draw();
		miActiveHand->RemoveTiles(draw);
		mStockBag.AddTiles(draw);

		// add played/swapped tiles back into the hand
        miActiveHand->AddTiles(tiles);

	    if (move.InvolvesSwap()) {
		    // remove swapped tiles from the stock bag
		    mStockBag.RemoveTiles(tiles);

	    } else {
	        // remove played tiles from the board
            mBoard.UnplayMove(move);
    
            // update the hand's score
			unsigned points = turn.Points();
	        miActiveHand->SubtractScore(points);
		}
	}
}


// inquiry methods

bool Game::CanRedo(void) const {
	bool result = (miRedo != mHistory.end());

	return result;
}

bool Game::CanUndo(void) const {
	unsigned turn = mHistory.Index(miRedo);
	bool result = (turn > mHands.Count());

	return result;
}

bool Game::HasEmptyCell(Cell const &rCell) const {
    bool result = mBoard.HasEmptyCell(rCell);
    
    return result; 
}

bool Game::HasUnsavedChanges(void) const {
	return mUnsavedChanges;
}

bool Game::IsClockRunning(void) const {
    bool result = miActiveHand->IsClockRunning();
     
    return result;
}

bool Game::IsLegalMove(Move const &rMove) const {
    char const *reason;
	bool result = IsLegalMove(rMove, reason);

	return result;
}

bool Game::IsLegalMove(Move const &rMove, char const *&rReason) const {
    unsigned stock = CountStock();
    bool result = true;
    
	if (!mBoard.IsValidMove(rMove, rReason)) {
	    result = false;
	} else if (mBestRunLength > 0 && !rMove.IsResign()
		&& (rMove.Count() != mBestRunLength || rMove.InvolvesSwap())) {
	    rReason = "FIRST";
        result = false;
    } else if (rMove.IsPureSwap() && rMove.Count() > stock) {
	    rReason = "STOCK";
        result = false;
    }

    return result;
}

bool Game::IsOver(void) const {
    bool result = false;
    
    // The game is over if and only if: 
	//    (1) the stock bag is empty and one of the hands has gone out 
	// or (2) all hands have resigned.

	if (IsStockEmpty() && mHands.HasAnyGoneOut()) {
        result = true;
    } else if (mHands.HaveAllResigned()) {
		result = true;
	}

    return result;
}

bool Game::IsPaused(void) const {
	bool result = !IsClockRunning() && !IsOver();

	return result;
}

bool Game::IsStockEmpty(void) const {
	bool result = (CountStock() == 0);

	return result;
}
