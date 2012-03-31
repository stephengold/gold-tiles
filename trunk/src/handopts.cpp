// File:    handopts.cpp
// Purpose: HandOpts class
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

#include "handopts.hpp"
#include "strings.hpp"


// operators

HandOpt &HandOpts::operator[](unsigned ind) {
	ASSERT(ind < Count());

	return mList[ind];
}

HandOpt const &HandOpts::operator[](unsigned ind) const {
	ASSERT(ind < Count());

	return mList[ind];
}


// misc methods

void HandOpts::Append(HandOpt const &rHandOpt) {
	mList.push_back(rHandOpt);

	ASSERT(!IsEmpty());
}

Strings HandOpts::AllPlayerNames(void) const {
	Strings result;

	for (unsigned i_hand = 0; i_hand < Count(); i_hand++) {
		String const player_name = (*this)[i_hand].PlayerName();
		result.Append(player_name);
	}

	ASSERT(result.Count() <= Count());
	return result;
}

unsigned HandOpts::Count(void) const {
    unsigned const result = mList.size();

	return result;
}

void HandOpts::MakeEmpty(void) {
	mList.clear();

	ASSERT(IsEmpty());
}

void HandOpts::Truncate(unsigned new_length) {
	ASSERT(Count() >= new_length);

	while (Count() > new_length) {
        mList.pop_back();
	}

	ASSERT(Count() == new_length);
}


// inquiry methods

bool HandOpts::IsEmpty(void) const {
	bool result = (Count() == 0);

	return result;
}