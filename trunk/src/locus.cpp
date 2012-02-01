// File:    locus.hpp
// Purpose: Locus class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "locus.hpp"

// constructors, assignment, and destructor

// The compiler.generated default constructor is fine.
// The compiler-generated copy constructor is fine.
// The compiler-generated assignment method is fine.
// The compiler-generated destructor is fine.

// public methods

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

bool Locus::contains(GridRef const &sq) const {
    bool result = (find(sq) != end());
    
    return result;
}

string Locus::toString(void) const {
    string result;

    result += "{";
    const_iterator gr;
    for (gr = begin(); gr != end(); gr++) {
        if (gr != begin()) {
            result += ", ";
        }
        result += gr->toString();
    }
    result += "}";

    return result;
}
