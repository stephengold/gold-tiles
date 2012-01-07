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
      unsigned *maxAttributes; // number of values (excluding blank) for each tile attribute
      unsigned numAttributes;  // number of attributes for each tile
      vector<Player> players;
      unsigned tileRedundancy; // number of copies of each tile

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
