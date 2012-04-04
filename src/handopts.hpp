#ifndef HANDOPTS_HPP_INCLUDED
#define HANDOPTS_HPP_INCLUDED

// File:    handopts.hpp
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

/*
A HandOpts object represents a list of hand options.

The HandsOpt class encapsulates a vector of HandOpt objects.
*/

#include <vector>       // HASA std::vector
#include "handopt.hpp"  // HASA HandOpt

class HandOpts {
public:
	// public operators
	HandOpt &operator[](unsigned);
    HandOpt const &operator[](unsigned) const;

	// misc public methods
	Strings  AllPlayerNames(void) const;
	void     Append(HandOpt const &);
    unsigned Count(void) const;
	void     GetUserChoice(unsigned hand_cnt);
	void     MakeEmpty(void);
	void     Truncate(unsigned new_length);

	// public inquiry method
	bool IsEmpty(void) const;

private:
	// private date
	std::vector<HandOpt> mList;
};
#endif // !defined(HANDOPTS_HPP_INCLUDED)
