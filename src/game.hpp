#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

// File:    game.hpp
// Purpose: Game class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "board.hpp"
#include "move.hpp"
#include "players.hpp"

class Game {
public:
	// lifecycle
    Game(void) { ASSERT(false); };
    Game(Strings player_names,
             ACountType attribute_cnt,
             AValueType max_attribute_values[],
             unsigned tile_redundancy,
             unsigned hand_size);
    Game(Game const &) { ASSERT(false); };
    // ~Game(void);  compiler-generated destructor is OK

	// operators
	Game &operator=(Game const &) { ASSERT(false); };

	// misc
    void     ActivateNextPlayer(void);
    void     FinishTurn(Move const &);
    void     GoingOutBonus(void);
    void     PlayGame(void);

	// access
    Player   ActivePlayer(void) const;
    operator Board(void) const;
    unsigned CountStock(void) const;
    Players  InactivePlayers(void) const;

	// inquiry
    bool     IsLegalMove(Move const &) const;
    bool     IsOver(void) const;

private:
    Players::IteratorType miActivePlayer; // whose turn it is
    unsigned              mBestRunLength; // zero after the first turn
    Board                 mBoard;         // extensible playing surface
    Players               mPlayers;       // who is playing
    Tiles                 mStockBag;      // stock bag from which tiles are drawn

	// misc
    void     AddTiles(AIndexType, unsigned redundancy, Tile &);
    Cell     ChooseCell(void) const;
    void     DisplayScores(void) const;
    void     FirstTurn(void);
    void     NextTurn(void);
    unsigned ScoreMove(Move const &) const;
};

#endif
