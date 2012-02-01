#ifndef LOCUS_HPP_INCLUDED
#define LOCUS_HPP_INCLUDED

// File:    locus.hpp
// Purpose: Locus class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Locus object represents a group of cells in a 2-D grid.
*/

#include <set>
#include <string>
#include "gridref.hpp"

class Locus: public set<GridRef> {
    public:
        bool areAllInSameRow(void) const;
        bool areAllInSameColumn(void) const;
        bool contains(GridRef const &) const;
        void display(void) const;
		bool isEmpty(void) const;
        string toString(void) const;
};

#endif
