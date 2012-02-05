#ifndef PLAYERS_HPP_INCLUDED
#define PLAYERS_HPP_INCLUDED

// File:    players.hpp
// Purpose: Players class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <vector>
#include "player.hpp"

class Players: public std::vector<Player> {
public:
    typedef std::vector<Player>::const_iterator ConstIteratorType;
    typedef std::vector<Player>::iterator IteratorType;

	// misc
    void     Next(ConstIteratorType &) const;

	// access
    unsigned Count(void) const;
};

#endif
