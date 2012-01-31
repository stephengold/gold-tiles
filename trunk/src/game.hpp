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
    vector<Player>::iterator _activePlayer;
    Tiles _bag;        // stock bag from which tiles are drawn
    unsigned _bestRunLength; // zero after the first turn
    Board _board;      // extensible playing surface
    vector<Player> _players;

    // private methods
    void addTiles(unsigned aIndex, unsigned redundancy, Tile &);
    GridRef chooseSquare(void) const;
    void playFirstTurn(void);
    void playTurn(void);
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
    
    void activateNextPlayer(void);
    Player getActivePlayer(void) const;
    vector<Player> getInactivePlayers(void) const;
    Board getBoard(void) const;
    unsigned getStock(void) const;
    bool isOver(void) const;
    bool isValidPlay(Play const &) const;
	void play(void);
    void playTiles(Play const &);
};

#endif
