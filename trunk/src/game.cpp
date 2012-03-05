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
	ASSERT(tileRedundancy >= 1);
	ASSERT(handSize >= 1);
	ASSERT(style != GAME_STYLE_NONE);

    // copy game parameters
	mStyle = style;
	mRedundancy = tileRedundancy;
	mHandSize = handSize;
	mSecondsPerHand = secondsPerHand;

    // generate all possible tiles
    unsigned attribute_index = 0;
    Tile model_tile;
    AddTiles(attribute_index, model_tile);
    D(std::cout << "Placed " << plural(CountStock(), "tile") << " in the stock bag." << std::endl);
    
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
        i_hand->DrawTiles(mHandSize, mStockBag);
    }
    std::cout << std::endl;

    // the hand with the longest "run" gets the first turn
    mBestRunLength = 0;
    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
        Tiles run = i_hand->LongestRun();
	    unsigned run_length = run.Count();
        std::cout << i_hand->Name() << " has a run of " << plural(run_length, "tile") 
			      << "." << std::endl;

	    if (run_length > mBestRunLength) {
	   	    mBestRunLength = run_length;
		    miActiveHand = i_hand;
	    }
    }

	mUnsavedChanges = false; // pretend this game is saved, for convenience
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
	ASSERT(!miActiveHand->IsClockRunning());

    mHands.NextWorking(miActiveHand);
	mUnsavedChanges = true;

	ASSERT(!miActiveHand->IsClockRunning());
}

// get the tiles in the active hand
Tiles Game::ActiveTiles(void) const {
    Tiles result = Tiles(*miActiveHand);
    
    return result;
}

// create tiles and add them to the stock bag
void Game::AddTiles(  // recursive
    unsigned attributeIndex,
    Tile &modelTile) {

    ACountType na = Tile::AttributeCnt();
	if (attributeIndex < na) {
		AValueType max = Tile::ValueMax(attributeIndex);
		for (AValueType attr = 0; attr <= max; attr++) {
        	modelTile.SetAttribute(attributeIndex, attr);
	        AddTiles(attributeIndex + 1, modelTile);
         }
	} else {
        ASSERT(attributeIndex == na);
		for (unsigned ci = 0; ci < mRedundancy; ci++) {
            Tile clo = modelTile.Clone();
			mStockBag.Add(clo);
		}
	}
}

unsigned Game::CountStock(void) const {
    unsigned result = mStockBag.Count();
    
    return result;
}

void Game::DisplayScores(void) const {
    Hands::ConstIterator i_hand;

    for (i_hand = mHands.begin(); i_hand < mHands.end(); i_hand++) {
	    i_hand->DisplayScore();
    }
}

void Game::FinishTurn(Move const &move) {
    ASSERT(mBoard.IsValidMove(move));

	if (move.IsResign()) {
		miActiveHand->Resign(mStockBag);

	} else {
        // remove played/swapped tiles from the hand
        Tiles tiles = Tiles(move);
        miActiveHand->RemoveTiles(tiles);

        // attempt to draw replacement tiles from the stock bag
        unsigned count = tiles.Count();
	    if (count > 0) {
            unsigned actual = miActiveHand->DrawTiles(count, mStockBag);
		    ASSERT(actual == count || !move.InvolvesSwap());
	    }

	    if (move.InvolvesSwap()) {
		    // return swapped tiles to the stock bag
		    mStockBag.AddTiles(tiles);
		    std::cout << miActiveHand->Name() << " put " << plural(count, "tile")
			    << " back into the stock bag." << std::endl;

	    } else {
	        // place played tiles on the board
            mBoard.PlayMove(move);
    
            // update the hand's score    
  	        unsigned points = mBoard.ScoreMove(move);
	        miActiveHand->AddScore(points);
		}
	}
	
	//  If it was the first turn, it no longer is.
    mBestRunLength = 0;

	// There are unsaved changes now.
	mUnsavedChanges = true;
}

void Game::FirstTurn(void) {
    std::cout << std::endl;

    Move move;
    for (;;) {
    	std::cout << miActiveHand->Name() << " plays first and must place " 
			<< plural(mBestRunLength, "tile") << " on the (empty) board." << std::endl;
		move = miActiveHand->ChooseMove();
    	if (IsLegalMove(move)) {
            break;
        }
	}

    FinishTurn(move);
}

void Game::GoingOutBonus(void) {
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
     D(std::cout << "Game::NextTurn()" << std::endl);

	 Move move;
     for (;;) {
 	     DisplayScores();
    	 unsigned stock = CountStock();
         std::cout << std::endl
			 << miActiveHand->Name() << "'s turn, " 
			 << plural(stock, "tile") << " remaining in the stock bag" << std::endl
	         << std::endl << (String)mBoard << std::endl;

		 move = miActiveHand->ChooseMove();
		 if (IsLegalMove(move)) {
             break;
         }
	 }

     FinishTurn(move);
}

void Game::PlayGame(void) {
	StartClock();
    FirstTurn();
	StopClock();

    while (!IsOver()) {
        ActivateNextHand();
		StartClock();
	    NextTurn();
		StopClock();
    }

    // display final scores
    DisplayScores();
}

unsigned Game::Redundancy(void) const {
	ASSERT(mRedundancy > 0);

	return mRedundancy;
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


// inquiry methods

bool Game::HasEmptyCell(Cell const &rCell) const {
    bool result = mBoard.HasEmptyCell(rCell);
    
    return result; 
}

bool Game::HasUnsavedChanges(void) const {
	return mUnsavedChanges;
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
        std::cout << "Not a valid move."
			<< std::endl;
	    result = false;
	} else if (mBestRunLength > 0 
		&& (rMove.Count() != mBestRunLength || rMove.InvolvesSwap())) {
        std::cout << "On the first turn, you must play as many tiles as possible.  Keep looking!"
			<< std::endl;
	    rReason = "FIRST";
        result = false;
    } else if (rMove.IsPureSwap() && rMove.Count() > stock) {
        std::cout << "You can't swap more tiles than the number remaining in the stock bag." 
			<< std::endl;
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
	bool result = !miActiveHand->IsClockRunning() && !IsOver();

	return result;
}

bool Game::IsStockEmpty(void) const {
	bool result = (CountStock() == 0);

	return result;
}
