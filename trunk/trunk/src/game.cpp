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
			bag.insert(clo);
		}
	}
}

GridRef Game::chooseSquare(const Tile &tile) const {
    int row, column;
    
    cout << "Enter row number: " << endl;
    cin >> row;
    cout << "Enter column number: " << endl;
    cin >> column;
    
    GridRef result = GridRef(row, column);
    
    return result;
}

// The game is over when the stock bag is empty and
// at least one player has gone out.

bool Game::isGameOver(void) const {
    bool result = false;
    
	if (bag.empty()) {
        vector<Player>::const_iterator player;
        for (player = players.begin(); player < players.end(); player++) {
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
    cout << endl << player.getName() << " plays first." << endl;

    Tiles tiles = player.longestRun();
    GridRef square; // start playing at the origin
    Direction playDirection = East; // play eastward (left to right)

    // assign a square for each tile
    Play play;
    Tiles::const_iterator tile;
    for (tile = tiles.begin(); tile != tiles.end(); tile++) {
       D(cout << "Will play " << tile->toString()
           << " at " << square.toString() << "." << endl);
       ASSERT(board.isEmpty(square));
       TileSquare ts(*tile, square);
       play.insert(ts);
       square = GridRef(square, playDirection, 1);
    }

    playTiles(player, play);
}

void Game::playTiles(Player &player, Play const &play) {
    ASSERT(board.isLegalPlay(play));
    ASSERT(bag.isValid());

	// place tiles on the board
    board.playTiles(play);
    
    // how many points for this play?
  	unsigned points = board.scorePlay(play);

    // remove played tiles from the player's hand
    Tiles tiles = play.getTiles();
    ASSERT(tiles.isValid());
    player.removeTiles(tiles);

    // update the player's score    
	player.addScore(points);

    // attempt to draw replacement tiles from the stock bag
    unsigned count = play.size();
    player.drawTiles(count, bag);
    ASSERT(bag.isValid());
}

void Game::playTurn(Player &player) {
     D(cout << "Game::playTurn(" << player.getName() << ")" << endl);
     Play play;
     while (true) {
	     board.display();
 	     printScores();
         cout << endl << "It is " << player.getName() << "'s turn." << endl;
         player.displayHand();

		 // choose tiles to play
		 play = player.choosePlay();

         if (board.isLegalPlay(play)) {
             D(cout << "That's a legal play." << endl);
             break;
         }
         cout << "That isn't a legal play." << endl;
	 }

     playTiles(player, play);
}

void Game::printScores(void) const {
    vector<Player>::const_iterator player;

    for (player = players.begin(); player < players.end(); player++) {
	    player->displayScore();
    }
}

unsigned Game::scorePlay(Play const &play) const {
     unsigned result = board.scorePlay(play);

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
    unsigned cnt = bag.size();
    D(cout << "Added " << plural(cnt, "tile") << " to the stock bag." << endl);
    
    // create players
    for (unsigned pi = 0; pi < numPlayers; pi++) {
	    Player player(playerNames[pi]);
	    players.push_back(player);
    }

    // deal each player a hand of tiles
    vector<Player>::iterator player;
    for (player = players.begin(); player < players.end(); player++) {
        player->drawTiles(handSize, bag);
    }

    // decide which player goes first
    vector<Player>::iterator first;
    unsigned bestRunLength = 0;
    for (player = players.begin(); player < players.end(); player++) {
        cout << endl;
        player->displayHand();
        set<Tile> run = player->longestRun();
	    unsigned runLength = run.size();
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
	    if (player >= players.end()) {
	       player = players.begin();
        }
	    playTurn(*player);
    }

    // print final score
    printScores();
}
