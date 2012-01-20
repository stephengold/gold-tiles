#ifndef LOCUS_HPP
#define LOCUS_HPP

// File:    locus.hpp
// Purpose: Locus class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*
 A Locus object describes a group of cells in a 2-D grid.
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
