#ifndef LOCUS_HPP
#define LOCUS_HPP

// File:    locus.hpp
// Purpose: Locus class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "gridof.hpp"
#include <set>

class Locus : public set<GridRef> {
    public:
        bool areAllInSameRow(void) const;
        bool areAllInSameColumn(void) const;
};

#endif
