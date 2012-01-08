#ifndef GAME_HPP
#define GAME_HPP

// File:    game.hpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <list>
#include <string>
#include <vector>

#include "player.hpp"
#include "gridof.hpp"
#include "square.hpp"
#include "tile.hpp"
#include "tilesquare.hpp"

using namespace std;

class Game {
      // private data
      set<Tile> bag;           // stock of tiles remaining to be drawn
      GridOf<Tile> board;      // playing area
      vector<Player> players;

      // private methods
      void addTiles(unsigned aIndex, unsigned redundancy, Tile &);
      GridRef * chooseSquare(const Tile &) const;
      bool isGameOver(void) const;
      bool isValidPlay(list<TileSquare> &) const;
      void playTurn(Player &);
      void printScores(void) const;
      unsigned scorePlay(list<TileSquare> &) const;

      public:
      Game(	unsigned numPlayers,
            string playerNames[],
            unsigned numAttributes, 
            unsigned maxAttributes[],
            unsigned tileRedundancy,
            unsigned numBlankTiles,
            unsigned handSize);
};
#endif
