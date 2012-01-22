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

// The game is over if (and only if) the stock bag is empty 
// some player has gone out.

bool Game::isGameOver(void) const {
    bool result = false;
    
	if (_bag.empty()) {
        vector<Player>::const_iterator player;
        for (player = _players.begin(); player < _players.end(); player++) {
	        if (player->handIsEmpty()) {
		        result = true;
		        break;
	        }
        }
   }

   D(cout << "Game::isGameOver() returns " << result << endl);
   return result;
}

void Game::playFirstTurn(Player &player) {
    cout << endl;
    D(cout << "Game::playFirstTurn(" << player.toString() << ")" << endl);

    Tiles run = player.longestRun();
	unsigned runLength = run.size();

    Play play;
    while (true) {
    	cout << player.getName() << " plays first and must place " 
			<< plural(runLength, "tile") << " on the (empty) board." << endl;
		play = player.choosePlay();
    	if (play.size() != runLength || play.isSwap() || !_board.isLegalPlay(play)) {
            cout << "That isn't a legal play." << endl;
		    continue;
        } else {
            break;
	    }
	}

    playTiles(player, play);
}

void Game::playTiles(Player &player, Play const &play) {
    D(cout << "Game::playTiles(" << player.toString() << ", " << play.toString() << ")" << endl);
    ASSERT(_board.isLegalPlay(play));

    // remove played/swapped tiles from the player's hand
    Tiles tiles = play.getTiles();
    player.removeTiles(tiles);

    // attempt to draw replacement tiles from the stock bag
    unsigned count = tiles.size();
	if (count > 0) {
        unsigned actual = player.drawTiles(count, _bag);
		ASSERT(actual == count || !play.isSwap());
	}

	if (play.isSwap()) {
		// put swapped tiles back into the bag
		_bag.addTiles(tiles);
		cout << player.toString() << " put " << plural(count, "tile")
			<< " back into the stock bag." << endl;

	} else {
	    // place played tiles on the board
        _board.playTiles(play);
    
        // update the player's score    
  	    unsigned points = _board.scorePlay(play);
	    player.addScore(points);
	}
}

void Game::playTurn(Player &player) {
     D(cout << "Game::playTurn(" << player.getName() << ")" << endl);

	 Play play;
     while (true) {
 	     printScores();
    	 unsigned stock = _bag.size();
         cout << endl
			 << player.getName() << "'s turn, " 
			 << plural(stock, "tile") << " remaining in the stock bag" << endl;
	     _board.display();

		 play = player.choosePlay();
		 if (play.isPureSwap() && play.size() > stock) {
             cout << "There aren't enough tiles in the stock bag." << endl;
             continue;
		 } else if (!_board.isLegalPlay(play)) {
             cout << "That isn't a legal play." << endl;
			 continue;
         } else {
             break;
		 }
	 }

     playTiles(player, play);
}

void Game::printScores(void) const {
    vector<Player>::const_iterator player;

    for (player = _players.begin(); player < _players.end(); player++) {
	    player->displayScore();
    }
}

unsigned Game::scorePlay(Play const &play) const {
     unsigned result = _board.scorePlay(play);

     return result;
}

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
    vector<Player>::iterator player;
    for (player = _players.begin(); player < _players.end(); player++) {
        player->drawTiles(handSize, _bag);
    }
    cout << endl;
}

void Game::play(void) {
    // decide which player goes first
    vector<Player>::iterator player;
    vector<Player>::iterator first;
    unsigned bestRunLength = 0;
    for (player = _players.begin(); player < _players.end(); player++) {
        D(player->displayHand());

        Tiles run = player->longestRun();
	    unsigned runLength = run.size();
        cout << player->getName() << " has a run of " << plural(runLength, "tile") << "." << endl;

	    if (runLength > bestRunLength) {
	   	    bestRunLength = runLength;
		    first = player;
	    }	
    }
    
    // play!
    player = first;
    playFirstTurn(*player);

    while (!isGameOver()) {
	    player++;
	    if (player >= _players.end()) {
	        player = _players.begin();
        }
	    playTurn(*player);
    }

    // print final score
    printScores();
}
