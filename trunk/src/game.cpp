// File:    game.cpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "game.hpp"

void Game::addTiles(unsigned attributeIndex, Tile &modelTile) {
	if (attributeIndex < numAttributes) {
		unsigned maxA = maxAttributes[attributeIndex];
		for (unsigned attr = 0; attr < maxA; attr++) {
        	modelTile.setAttribute(attributeIndex, attr);
	        addTiles(attributeIndex + 1, modelTile);
         } 
	} else {
		for (unsigned ci = 0; ci < tileRedundancy; ci++) {
			bag.insert(modelTile);
		}
	}
}

GridRef *Game::chooseSquare(Tile const &tile) const {
    unsigned n, e;
    
    cout << "Enter northing: ";
    cin >> n;
    cout << "Enter easting: ";
    cin >> e;
    
    GridRef *result = new GridRef(n, e);
    
    return result;
}

bool Game::isGameOver(void) const {
	if (!bag.empty()) {
        return false;
    }

    vector<Player>::const_iterator pi;
    for (pi = players.begin(); pi < players.end(); pi++) {
	    if (pi->handIsEmpty()) {
		    return true;
	    }
   }

   return false;
}

bool Game::isValidPlay(list<TileSquare> &play) const{
     // TODO
     
     return false;
}

void Game::playTurn(Player &p) {
     list<TileSquare> play;

     do {
	    board.display();
		printScores();

		// choose tiles to play
		set<Tile> tileChoice = p.chooseTiles();

       // choose squares to play tiles
       play = list<TileSquare>();
       set<Tile>::iterator tile;
	   for (tile = tileChoice.begin(); tile != tileChoice.end(); tile++) {
           Tile *t = new Tile(*tile);
           GridRef *s = this->chooseSquare(*t);
   	       TileSquare ts = TileSquare(t, s);
           play.push_back(ts);
       }
	} while (!isValidPlay(play));
	
	unsigned points = scorePlay(play);
	p.addScore(points);

    unsigned count = play.size();
    list<TileSquare>::iterator ts;
    for (ts = play.begin(); ts != play.end(); ts++) {
	    Tile *t = ts->getTile();
	    GridRef *s = ts->getSquare();
	    board.set(*s, t);
	    p.deleteTile(*t);
    }
    
    p.drawTiles(count, bag);
}

void Game::printScores(void) const {
    vector<Player>::const_iterator player;

    for (player = players.begin(); player < players.end(); player++) {
	    player->printScore();
    }
}

unsigned Game::scorePlay(list<TileSquare> &play) const {
    // TODO
}

Game::Game(
    unsigned numPlayers,
    string playerNames[],
    unsigned numAttr, 
    unsigned maxAttr[],
    unsigned redundancy,
    unsigned numBlankTiles,
    unsigned handSize) {

    // copy game parameters
    numAttributes = numAttr;
    maxAttributes = new unsigned[numAttr];
    for (int i = 0; i < numAttr; i++) {
        maxAttributes[i] = maxAttr[i];
    }
    tileRedundancy = redundancy;

    // generate blank tiles
    for (int bi = 0; bi < numBlankTiles; bi++) {
	    Tile *blankTile = new Tile;
	    bag.insert(*blankTile);
    }

    // generate non-blank tiles
    Tile modelTile;
    unsigned attributeIndex = 0;
    addTiles(attributeIndex, modelTile);

    // create players and deal out hands of tiles
    for (unsigned pi = 0; pi < numPlayers; pi++) {
	    Player *player = new Player(playerNames[pi]);
	    player->drawTiles(handSize, bag);
	    players.push_back(*player);
    }

    // decide which player goes first
    vector<Player>::iterator first, player;
    unsigned bestRunLength = 0;
    for (player = players.begin(); player < players.end(); player++) {
	    unsigned runLength = player->bestRunLength();
	    if (runLength > bestRunLength) {
	   	    bestRunLength = runLength;
		    first = player;
	    }	
    }

    // play!
    player = first;
    while (!isGameOver()) {
	    playTurn(*player);
	    player++;
	    if (player >= players.end()) {
	       player = players.begin();
        }
    }

    // print final score
    printScores();
}
