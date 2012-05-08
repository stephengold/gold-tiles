#ifndef HANDOPTS_HPP_INCLUDED
#define HANDOPTS_HPP_INCLUDED

// File:     handopts.hpp
// Location: src
// Purpose:  declare HandOpts class
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

/*
A HandOpts object represents a list of hand options.

The HandsOpt class encapsulates a vector of HandOpt objects.
*/

#include <vector>       // HASA std::vector
#include "handopt.hpp"  // HASA HandOpt


class HandOpts {
public:
    // public lifecycle
    HandOpts(void);
    // HandOpts(HandOpts const&);  implicitly defined copy constructor
    // ~HandOpts(void);  implicitly defined destructor

    // public operators
    // HandOpts& operator=(HandOpts const&);  implicitly defined assignment operator 
    HandOpt& operator[](unsigned);
    HandOpt const& operator[](unsigned) const;
    operator String(void) const;

    // misc public methods
    Strings  AllPlayerNames(void) const;
    void     Append(HandOpt const&);
    unsigned Count(void) const;
    bool     GetFromClient(Socket&, unsigned handCnt);
    void     GetUserChoice(unsigned handCnt);
    void     MakeEmpty(void);
    void     Serverize(Address const& client, Address const& server);
    void     Truncate(unsigned new_length);

    // public inquiry method
    bool IsEmpty(void) const;

private:
    // private data
    std::vector<HandOpt> mList;
};
#endif // !defined(HANDOPTS_HPP_INCLUDED)
