// File:    game.cpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include "board.hpp"
#include "game.hpp"
#include "strings.hpp"

// lifecycle
Game::Game(
    Strings playerNames,
    ACountType attributeCnt,
    AValueType pMaxAttributeValues[],
    unsigned tileRedundancy,
    unsigned handSize)
{
    // copy game parameters
    Tile::SetStatic(attributeCnt, pMaxAttributeValues);

    // generate all possible tiles
    unsigned attribute_index = 0;
    Tile model_tile;
    AddTiles(attribute_index, tileRedundancy, model_tile);
    D(std::cout << "Placed " << plural(CountStock(), "tile") << " in the stock bag." << std::endl);
    
    // create players
	Strings::ConstIteratorType i_player;
	for (i_player = playerNames.Begin(); i_player != playerNames.End(); i_player++) {
		String name = *i_player;
	    Player player(name);
	    mPlayers.push_back(player);
    }

    // deal each player a hand of tiles
    Players::IteratorType player, bestPlayer;
    for (player = mPlayers.begin(); player < mPlayers.end(); player++) {
        player->DrawTiles(handSize, mStockBag);
    }
    std::cout << std::endl;

    // decide which player goes first
    unsigned bestRunLength = 0;
    for (player = mPlayers.begin(); player < mPlayers.end(); player++) {
        D(player->DisplayHand());

        Tiles run = player->LongestRun();
	    unsigned run_length = run.Count();
        std::cout << player->Name() << " has a run of " << plural(run_length, "tile") 
			      << "." << std::endl;

	    if (run_length > bestRunLength) {
	   	    bestRunLength = run_length;
		    bestPlayer = player;
	    }
    }
    miActivePlayer = bestPlayer;
    mBestRunLength = bestRunLength;
}

// methods

void Game::ActivateNextPlayer(void) {
    mPlayers.Next(miActivePlayer);
}

Player Game::ActivePlayer(void) const {
    Player result = *miActivePlayer;
    
    return result;
}

// create tiles and add them to the stock bag
void Game::AddTiles(  // recursive
    unsigned attributeIndex,
    unsigned tileRedundancy,
    Tile &modelTile) {

    ACountType na = Tile::AttributeCnt();
	if (attributeIndex < na) {
		AValueType max = Tile::ValueMax(attributeIndex);
		for (AValueType attr = 0; attr <= max; attr++) {
        	modelTile.SetAttribute(attributeIndex, attr);
	        AddTiles(attributeIndex + 1, tileRedundancy, modelTile);
         }
	} else {
        ASSERT(attributeIndex == na);
		for (unsigned ci = 0; ci < tileRedundancy; ci++) {
            Tile clo = modelTile.Clone();
			mStockBag.insert(clo);
		}
	}
}

Game::operator Board(void) const {
    Board result = mBoard;
    
    return result;
}

unsigned Game::CountStock(void) const {
    unsigned result = mStockBag.Count();
    
    return result;
}

void Game::DisplayScores(void) const {
    Players::const_iterator i_player;

    for (i_player = mPlayers.begin(); i_player < mPlayers.end(); i_player++) {
	    i_player->DisplayScore();
    }
}

void Game::FinishTurn(Move const &move) {
    D(std::cout << "Game::FinishTurn(" << (String)move << ")" << std::endl);
    ASSERT(mBoard.IsLegalMove(move));

    // remove played/swapped tiles from the player's hand
    Tiles tiles = move.GetTiles();
    miActivePlayer->RemoveTiles(tiles);

    // attempt to draw replacement tiles from the stock bag
    unsigned count = tiles.Count();
	if (count > 0) {
        unsigned actual = miActivePlayer->DrawTiles(count, mStockBag);
		ASSERT(actual == count || !move.InvolvesSwap());
	}

	if (move.InvolvesSwap()) {
		// put swapped tiles back into the bag
		mStockBag.AddTiles(tiles);
		std::cout << miActivePlayer->Name() << " put " << plural(count, "tile")
			<< " back into the stock bag." << std::endl;

	} else {
	    // place played tiles on the board
        mBoard.PlayMove(move);
    
        // update the player's score    
  	    unsigned points = mBoard.ScoreMove(move);
	    miActivePlayer->AddScore(points);
	}
	
	//  If it was the first turn, it no longer is.
    mBestRunLength = 0;
}

void Game::FirstTurn(void) {
    std::cout << std::endl;
    D(std::cout << "Game::FirstTurn(" << miActivePlayer->Name() << ")" << std::endl);

    Move move;
    while (true) {
    	std::cout << miActivePlayer->Name() << " plays first and must place " 
			<< plural(mBestRunLength, "tile") << " on the (empty) board." << std::endl;
		move = miActivePlayer->ChooseMove();
    	if (IsLegalMove(move)) {
            break;
        }
	}

    FinishTurn(move);
}

void Game::GoingOutBonus(void) {
    Players::ConstIteratorType i_player = miActivePlayer;
	mPlayers.Next(i_player);
    
    while (i_player != miActivePlayer) {
        Tiles hand = Tiles(*i_player);
        unsigned pointsInHand = hand.Count(); // TODO
        miActivePlayer->AddScore(pointsInHand);
		mPlayers.Next(i_player);
    }
}

Players Game::InactivePlayers(void) const {
    Players result;

    Players::ConstIteratorType i_player = miActivePlayer;
	mPlayers.Next(i_player);
    
    while (i_player != miActivePlayer) {
        result.push_back(*i_player);
        mPlayers.Next(i_player);
    }
    
    return result;
}

bool Game::IsLegalMove(Move const &move) const {
    unsigned stock = CountStock();
    bool result = true;
    
    if (mBestRunLength > 0 && (move.Count() != mBestRunLength || move.InvolvesSwap())) {
       std::cout << "You must play at least " << mBestRunLength << " tiles on this turn." << std::endl;
       result = false;
    } else if (move.IsPureSwap() && move.Count() > stock) {
       std::cout << "There aren't enough tiles in the stock bag." << std::endl;
       result = false;
	} else if (!mBoard.IsLegalMove(move)) {
       std::cout << "That isn't a legal play." << std::endl;
	   result = false;
    }

    return result;
}

bool Game::IsOver(void) const {
    bool result = false;
    
    // The game is over if (and only if) the stock bag is empty 
    // and a player has gone out.

	if (IsStockEmpty()) {
        Players::const_iterator player;
        for (player = mPlayers.begin(); player < mPlayers.end(); player++) {
	        if (player->IsEmptyHanded()) {
		        result = true;
		        break;
	        }
        }
   }

   D(std::cout << "Game::IsOver() returns " << result << std::endl);
   return result;
}

bool Game::IsStockEmpty(void) const {
	bool result = mStockBag.IsEmpty();

	return result;
}

void Game::NextTurn(void) {
     D(std::cout << "Game::NextTurn()" << std::endl);

	 Move move;
     while (true) {
 	     DisplayScores();
    	 unsigned stock = CountStock();
         std::cout << std::endl
			 << miActivePlayer->Name() << "'s turn, " 
			 << plural(stock, "tile") << " remaining in the stock bag" << std::endl
	         << std::endl << (String)mBoard << std::endl;

		 move = miActivePlayer->ChooseMove();
		 if (IsLegalMove(move)) {
             break;
         }
	 }

     FinishTurn(move);
}

void Game::PlayGame(void) {
    FirstTurn();

    while (!IsOver()) {
        ActivateNextPlayer();
	    NextTurn();
    }

    // display final scores
    DisplayScores();
}

unsigned Game::ScoreMove(Move const &move) const {
     unsigned result = mBoard.ScoreMove(move);

     return result;
}
