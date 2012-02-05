#ifndef POLY_HPP_INCLUDED
#define POLY_HPP_INCLUDED

// File:    poly.hpp
// Purpose: Poly and FractionPair classes for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Poly object represents a closed polygon which can be scaled to
 fit inside a given rectangle.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <vector>
#include <windows.h>
#include "fractionpair.hpp"

class Poly: public std::vector<FractionPair> {
public:
	typedef std::vector<FractionPair>::const_iterator ConstIteratorType;

	// misc
    void     Add(double x, double y);

	// access
	unsigned Count(void) const;
    void     GetPoints(POINT points[], unsigned numPoints, 
                        Rect const &rBounds) const;    
};

#endif
#endif
