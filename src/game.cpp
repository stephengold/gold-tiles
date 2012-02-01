// File:    game.cpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <iostream>
#include <string>
#include "board.hpp"
#include "game.hpp"
#include "locus.hpp"
#include "play.hpp"
#include "player.hpp"


Game::Game(
    unsigned numPlayers,
    string playerNames[],
    ACount numAttr,
    AValue maxAttr[],
    unsigned tileRedundancy,
    unsigned handSize) {

    // copy game parameters
    Tile::setStatic(numAttr, maxAttr);

    // generate all possible tiles
    unsigned attributeIndex = 0;
    Tile modelTile;
    addTiles(attributeIndex, tileRedundancy, modelTile);
    D(cout << "Placed " << plural(_bag.size(), "tile") << " to the stock bag." << endl);
    
    // create players
    for (unsigned pi = 0; pi < numPlayers; pi++) {
	    Player player(playerNames[pi]);
	    _players.push_back(player);
    }

    // deal each player a hand of tiles
    Players::iterator player;
    Players::iterator bestPlayer;
    for (player = _players.begin(); player < _players.end(); player++) {
        player->drawTiles(handSize, _bag);
    }
    cout << endl;

    // decide which player goes first
    unsigned bestRunLength = 0;
    for (player = _players.begin(); player < _players.end(); player++) {
        D(player->displayHand());

        Tiles run = player->longestRun();
	    unsigned runLength = run.size();
        cout << player->getName() << " has a run of " << plural(runLength, "tile") << "." << endl;

	    if (runLength > bestRunLength) {
	   	    bestRunLength = runLength;
		    bestPlayer = player;
	    }
    }
    _activePlayer = bestPlayer;
    _bestRunLength = bestRunLength;
}

void Game::activateNextPlayer(void) {
    _activePlayer++;
    if (_activePlayer >= _players.end()) {
        _activePlayer = _players.begin();
    }
}

// create tiles and add them to the stock bag
void Game::addTiles(  // recursive
    unsigned attributeIndex,
    unsigned tileRedundancy,
    Tile &modelTile) {

    ACount na = Tile::getNumAttributes();
	if (attributeIndex < na) {
		AValue maxA = Tile::getMaxAttribute(attributeIndex);
		for (AValue attr = 0; attr <= maxA; attr++) {
        	modelTile.setAttribute(attributeIndex, attr);
	        addTiles(attributeIndex + 1, tileRedundancy, modelTile);
         }
	} else {
        ASSERT(attributeIndex == na);
		for (unsigned ci = 0; ci < tileRedundancy; ci++) {
            Tile clo = modelTile.clone();
			_bag.insert(clo);
		}
	}
}

// accessors

Player Game::getActivePlayer(void) const {
    Player result = *_activePlayer;
    
    return result;
}
Board Game::getBoard(void) const {
    Board result = _board;
    
    return result;
}

Players Game::getInactivePlayers(void) const {
    Players result;

    Players::const_iterator player = _activePlayer + 1;
    if (player >= _players.end()) {
        player = _players.begin();
    }
    
    while (player != _activePlayer) {
        result.push_back(*player);
        player++;
        if (player >= _players.end()) {
            player = _players.begin();
        }  
    }
    
    return result;
}

void Game::goingOutBonus(void) {
    Players::const_iterator player = _activePlayer + 1;
    if (player >= _players.end()) {
        player = _players.begin();
    }
    
    while (player != _activePlayer) {
        Tiles hand = player->getHand();
        unsigned pointsInHand = hand.size();
        _activePlayer->addScore(pointsInHand);
        player++;
        if (player >= _players.end()) {
            player = _players.begin();
        }  
    }
}

unsigned Game::getStock(void) const {
    unsigned result = _bag.size();
    
    return result;
}

// The game is over if (and only if) the stock bag is empty 
// some player has gone out.

bool Game::isOver(void) const {
    bool result = false;
    
	if (_bag.empty()) {
        Players::const_iterator player;
        for (player = _players.begin(); player < _players.end(); player++) {
	        if (player->handIsEmpty()) {
		        result = true;
		        break;
	        }
        }
   }

   D(cout << "Game::isOver() returns " << result << endl);
   return result;
}

bool Game::isValidPlay(Play const &play) const {
    unsigned stock = getStock();
    bool result = true;
    
    if (_bestRunLength > 0 && (play.size() != _bestRunLength || play.isSwap())) {
       cout << "You must play at least " << _bestRunLength << " tiles on this turn." << endl;
       result = false;
    } else if (play.isPureSwap() && play.size() > stock) {
       cout << "There aren't enough tiles in the stock bag." << endl;
       result = false;
	} else if (!_board.isLegalPlay(play)) {
       cout << "That isn't a legal play." << endl;
	   result = false;
    }

    return result;
}

void Game::play(void) {
    // play!
    playFirstTurn();

    while (!isOver()) {
        activateNextPlayer();
	    playTurn();
    }

    // print final score
    printScores();
}

void Game::playFirstTurn(void) {
    cout << endl;
    D(cout << "Game::playFirstTurn(" << _activePlayer->toString() << ")" << endl);

    Play play;
    while (true) {
    	cout << _activePlayer->getName() << " plays first and must place " 
			<< plural(_bestRunLength, "tile") << " on the (empty) board." << endl;
		play = _activePlayer->choosePlay();
    	if (isValidPlay(play)) {
            break;
        }
	}

    playTiles(play);
}

void Game::playTiles(Play const &play) {
    D(cout << "Game::playTiles(" << play.toString() << ")" << endl);
    ASSERT(_board.isLegalPlay(play));

    // remove played/swapped tiles from the player's hand
    Tiles tiles = play.getTiles();
    _activePlayer->removeTiles(tiles);

    // attempt to draw replacement tiles from the stock bag
    unsigned count = tiles.size();
	if (count > 0) {
        unsigned actual = _activePlayer->drawTiles(count, _bag);
		ASSERT(actual == count || !play.isSwap());
	}

	if (play.isSwap()) {
		// put swapped tiles back into the bag
		_bag.addTiles(tiles);
		cout << _activePlayer->toString() << " put " << plural(count, "tile")
			<< " back into the stock bag." << endl;

	} else {
	    // place played tiles on the board
        _board.playTiles(play);
    
        // update the player's score    
  	    unsigned points = _board.scorePlay(play);
	    _activePlayer->addScore(points);
	}
	
	//  If it was the first turn, it no longer is.
    _bestRunLength = 0;
}

void Game::playTurn(void) {
     D(cout << "Game::playTurn()" << endl);

	 Play play;
     while (true) {
 	     printScores();
    	 unsigned stock = getStock();
         cout << endl
			 << _activePlayer->getName() << "'s turn, " 
			 << plural(stock, "tile") << " remaining in the stock bag" << endl;
	     _board.display();

		 play = _activePlayer->choosePlay();
		 if (isValidPlay(play)) {
             break;
         }
	 }

     playTiles(play);
}

void Game::printScores(void) const {
    Players::const_iterator player;

    for (player = _players.begin(); player < _players.end(); player++) {
	    player->displayScore();
    }
}

unsigned Game::scorePlay(Play const &play) const {
     unsigned result = _board.scorePlay(play);

     return result;
}
