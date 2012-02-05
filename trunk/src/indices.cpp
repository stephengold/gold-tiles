// File:    indices.cpp
// Purpose: implement Indices class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "indices.hpp"
#include "project.hpp"

// methods

void Indices::Add(int index) {
	ASSERT(!Contains(index));

	insert(index);
}

bool Indices::Contains(int index) const {
	bool result = (find(index) != end());

	return result;
}