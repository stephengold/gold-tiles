// File:    indices.cpp
// Purpose: Indices class
// Author:  Stephen Gold sgold@sonic.net
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

#include "indices.hpp"
#include "project.hpp"


// misc methods

void Indices::Add(IndexType index) {
	ASSERT(!Contains(index));

	insert(index);

	ASSERT(Contains(index));
}

void Indices::AddRemove(IndexType index, bool addFlag) {
	bool have_index = Contains(index);
	if (have_index && !addFlag) {
		Remove(index);
	} else if (!have_index && addFlag) {
		Add(index);
	}

	ASSERT(addFlag == Contains(index));
}

unsigned Indices::Count(void) const {
	unsigned result = size();

	return result;
}

void Indices::MakeEmpty(void) {
	clear();
}

void Indices::Remove(IndexType index) {
	ASSERT(Contains(index));

	IteratorType i_index = find(index);
    ASSERT(i_index != end());
	ASSERT(*i_index == index);

	erase(i_index);

	ASSERT(!Contains(index));
}


// inquiry methods
bool Indices::Contains(IndexType index) const {
	bool result = (find(index) != end());

	return result;
}
