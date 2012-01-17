// File:    game.cpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "board.hpp"
#include "game.hpp"

// create tiles and add them to the stock bag (recursive)
void Game::addTiles(
    unsigned attributeIndex,
    unsigned tileRedundancy,
    Tile &modelTile) {
             
	if (attributeIndex < Tile::getNumAttributes()) {
		unsigned maxA = Tile::getMaxAttribute(attributeIndex);
		for (unsigned attr = 0; attr <= maxA; attr++) {
        	modelTile.setAttribute(attributeIndex, attr);
	        addTiles(attributeIndex + 1, tileRedundancy, modelTile);
         }
	} else {
		for (unsigned ci = 0; ci < tileRedundancy; ci++) {
            Tile clo = modelTile.clone();
			bag.insert(clo);
		}
	}
}

GridRef Game::chooseSquare(const Tile &tile) const {
    int row, column;
    
    cout << "Enter row number: ";
    cin >> row;
    cout << "Enter column number: ";
    cin >> column;
    
    return GridRef(row, column);
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
#if 0   
   cout << "Game::isGameOver() returns " << result << endl;
#endif
   return result;
}

bool Game::isLegalPlay(Play const &play) const {
    // check for repeated tiles or squares
    if (play.repeatsTile()) {
        return false;
    } 
    if (play.repeatsSquare()) {
        return false;
    }

    // get the set of squares on the board to be played
    Locus squares = play.getSquares();
     
    // make sure the squares are all clear
    if (!board.areAllEmpty(squares)) {
        return false;
    }
    
    // make sure the squares lie in a single row or column
    bool singleRow = squares.areAllInSameRow();
    bool singleColumn = squares.areAllInSameColumn();
    if (!singleRow && !singleColumn) {
        return false;
    }

    // make a copy of the board and place the tiles on it
    Board newBoard(board);
    newBoard.placeTiles(play);
    
    // make sure there are no gaps between played tiles
    if (singleRow) {
        if (!newBoard.isContiguousRow(squares)) {
            return false;
        }
    } else {
        if (!newBoard.isContiguousColumn(squares)) {
            return false;
        }
    }
    
    // check compatibility of contiguous tiles in each row and column played
    if (!newBoard.areAllRowsCompatible(squares)) {
        return false;
    }
    if (!newBoard.areAllColumnsCompatible(squares)) {
        return false;
    }
    
    return true;
}

void Game::playFirstTurn(Player &player) {
    cout << endl << player.getName() << " plays first." << endl;
    
    set<Tile> tiles = player.longestRun();
    GridRef square; // start playing at the origin
    Direction playDirection = East; // play eastward (left to right)

    // assign a square for each tile
    Play play;
    set<Tile>::const_iterator tile;
    for (tile = tiles.begin(); tile != tiles.end(); tile++) {
#if 0
       cout << "Will play " << tile->toString() << " at " << square.toString() << "." << endl;
#endif
       ASSERT(board.isEmpty(square));
       TileSquare ts(*tile, square);
       play.insert(ts);
       square = GridRef(square, playDirection, 1);
    }
    
    playTiles(player, play);
}

void Game::playTiles(Player &player, Play const &play) {
    // how many points for this play?
  	unsigned points = scorePlay(play);

	// place tiles on the board
    board.placeTiles(play);
    board.validate("after place");
    
    // remove played tiles from the player's hand
    Tiles tiles = play.getTiles();
    player.removeTiles(tiles);

    // update the player's score    
	player.addScore(points);
	
    // attempt to draw replacement tiles from the stock bag
    unsigned count = play.size();
    player.drawTiles(count, bag);
}

void Game::playTurn(Player &player) {
#if 0
     cout << "Game::playTurn(" << player.getName() << ")" << endl;
#endif     
     Play play;
     do {
	    board.display();
		printScores();
        cout << endl << "It is " << player.getName() << "'s turn." << endl;
        pause();

		// choose tiles to play
		set<Tile> tileChoice = player.chooseTiles();

        // choose square to play each tile
        play.clear();
        set<Tile>::iterator tile;
	    for (tile = tileChoice.begin(); tile != tileChoice.end(); tile++) {
            Tile t(*tile);
            GridRef s = chooseSquare(t);
   	        TileSquare ts(t, s);
            play.insert(ts);
        }
        
        if (isLegalPlay(play)) {
            cout << "That is a legal play." << endl;
            break;
        }
        cout << "That is not a legal play." << endl;
	} while (true);

    playTiles(player, play);    	
}

void Game::printScores(void) const {
    vector<Player>::const_iterator player;

    for (player = players.begin(); player < players.end(); player++) {
	    player->printScore();
    }
}

unsigned Game::scorePlay(Play const &play) const {
    return 0;
}

Game::Game(
    unsigned numPlayers,
    string playerNames[],
    unsigned numAttr, 
    unsigned maxAttr[],
    unsigned tileRedundancy,
    unsigned handSize) {

    // copy game parameters
    Tile::setStatic(numAttr, maxAttr);

    // generate non-blank tiles
    unsigned attributeIndex = 0;
    Tile modelTile;
    addTiles(attributeIndex, tileRedundancy, modelTile);

    // create players and deal each player a hand of tiles
    for (unsigned pi = 0; pi < numPlayers; pi++) {
	    Player player(playerNames[pi]);
	    player.drawTiles(handSize, bag);
	    players.push_back(player);
    }

    // decide which player goes first
    vector<Player>::iterator first, player;
    unsigned bestRunLength = 0;
    for (player = players.begin(); player < players.end(); player++) {
        cout << endl;
        player->printHand();
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
