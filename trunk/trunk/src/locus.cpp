// File:    locus.hpp
// Purpose: Locus class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "locus.hpp"

bool Locus::areAllInSameRow(void) const {
    bool result = true;
    
    if (size() > 1) {
        const_iterator s = begin();
        int row = s->getRow();
        for ( ; s != end(); s++) {
            if (s->getRow() != row) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

bool Locus::areAllInSameColumn(void) const {
    bool result = true;
    
    if (size() > 1) {
        const_iterator s = begin();
        int column = s->getColumn();
        for ( ; s != end(); s++) {
            if (s->getColumn() != column) {
                result = false;
                break;
            }     
        }
    }

    return result;
}

