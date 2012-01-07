#ifndef GAME_HPP
#define GAME_HPP

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
      set<Tile> bag;
      GridOf<Tile> board;
      unsigned *maxAttributes;
      unsigned numAttributes;
      vector<Player> players;
      unsigned tileRedundancy;

      // private methods
      void addTiles(unsigned attributeIndex, Tile &);
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
