// File:     turn.cpp
// Location: src
// Purpose:  implement Turn class
// Author:   Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "turn.hpp"


// lifecycle

Turn::Turn(Tiles const& rTiles, String const& rHandName):
    mDraw(rTiles),
	mHandName(rHandName)
{
	// one of the initial draws
	mMustPlay = 0;
	mPoints = 0;
}

Turn::Turn(Move const& rMove, String const& rHandName, unsigned mustPlay):
    mHandName(rHandName),
	mMove(rMove)
{
	// an ordinary turn
	mMustPlay = mustPlay;
	mPoints = 0;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment operator is OK.

Turn::operator Move(void) const {
	return mMove;
}


// misc methods

Tiles Turn::Draw(void) const {
	return mDraw;
}

String Turn::HandName(void) const {
	return mHandName;
}

ScoreType Turn::Points(void) const {
	return mPoints;
}

void Turn::SetDraw(Tiles const& rDraw) {
     mDraw = rDraw;
}

void Turn::SetPoints(ScoreType points) {
     mPoints = points;
}

unsigned Turn::MustPlay(void) const {
    return mMustPlay;
}