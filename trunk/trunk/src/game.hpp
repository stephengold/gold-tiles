#ifndef GAME_HPP
#define GAME_HPP

// File:    game.hpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <string>
#include <vector>
#include "board.hpp"
#include "player.hpp"
#include "tiles.hpp"

class Game {
    // private data
    Tiles bag;        // stock bag from which tiles are drawn
    Board board;      // extensible playing surface
    vector<Player> players;

    // private methods
    void addTiles(unsigned aIndex, unsigned redundancy, Tile &);
    GridRef chooseSquare(Tile const &) const;
    bool isGameOver(void) const;
    void playFirstTurn(Player &);
    void playTiles(Player &, Play const &);
    void playTurn(Player &);
    void printScores(void) const;
    unsigned scorePlay(Play const &) const;

    public:
    Game(unsigned numPlayers,
         string playerNames[],
         ACount numAttributes,
         AValue maxAttributes[],
         unsigned tileRedundancy,
         unsigned handSize);
};

#endif
