// File:    players.cpp
// Purpose: Players class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "players.hpp"

void Players::Next(ConstIteratorType &riCurrentPlayer) const {
	riCurrentPlayer++;
    if (riCurrentPlayer >= end()) {
        riCurrentPlayer = begin();
    }
};