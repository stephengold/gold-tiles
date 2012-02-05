#ifndef FRACTIONPAIR_HPP_INCLUDED
#define FRACTIONPAIR_HPP_INCLUDED

// File:    fractionpair.hpp
// Purpose: FractionPair class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A FractionPair object represents a point in a unit square centered
 on (0.5, 0.5).
*/

#include "project.hpp"

class FractionPair {
public:
	// lifecycle
	FractionPair(void) { ASSERT(false); }
    FractionPair(float x, float y);
    //FractionPair(FractionPair const &); compiler-generated copy constructor is OK
    //~FractionPair(void);

	// operators
	//FractionPair &operator=(FractionPair const &); compiler-generated assignment method is OK
     
	// access
    float X(void) const;
    float Y(void) const;

private:
    float mX, mY;
};

#endif