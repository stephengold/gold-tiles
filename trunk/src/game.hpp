#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

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
    Tiles _bag;        // stock bag from which tiles are drawn
    Board _board;      // extensible playing surface
    vector<Player> _players;
    vector<Player>::iterator _activePlayer;

    // private methods
    void addTiles(unsigned aIndex, unsigned redundancy, Tile &);
    GridRef chooseSquare(void) const;
    bool isGameOver(void) const;
    void playFirstTurn(Player &);
    void playTiles(Player &, Play const &);
    void playTurn(Player &);
    void printScores(void) const;
    unsigned scorePlay(Play const &) const;

    public:
        Game(void) { assert(false); };
        Game(Game const &) { assert(false); };
        Game &operator=(Game const &) { assert(false); };
        // ~Game(void);  implicitly declared destructor is fine

    Game(unsigned numPlayers,
         string playerNames[],
         ACount numAttributes,
         AValue maxAttributes[],
         unsigned tileRedundancy,
         unsigned handSize);

    Player getActivePlayer(void) const;
    Board getBoard(void) const;
	void play(void);
};

#endif
