#ifndef POLY_HPP_INCLUDED
#define POLY_HPP_INCLUDED

// File:    poly.hpp
// Purpose: Poly and FractionPair classes for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <vector>
#include <windows.h>

class FractionPair {
    float _x, _y;
    public:
        FractionPair(void);
        //FractionPair(FractionPair const &);
        FractionPair(float x, float y);
        //FractionPair &operator=(FractionPair const &);
        //~FractionPair(void);
        
        float getX(void) const;
        float getY(void) const;
};

class Poly: public vector<FractionPair> {
     public:
         void add(float x, float y);
         void getPoints(POINT points[], unsigned numPoints, 
                        Rect const &bounds) const;     
};

#endif
#endif
