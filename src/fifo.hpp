#ifndef FIFO_HPP_INCLUDED
#define FIFO_HPP_INCLUDED

// File:     fifo.hpp
// Location: src
// Purpose:  declare Fifo class
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
A Fifo object represents a first-in, first-out buffer for receiving
network bytes.

The Fifo class is encapsulates a fixed-size buffer and a pair of counts,
one for valid bytes and one for consumed bytes.
*/


class Fifo {
public:
    // public lifecycle
    Fifo(void);
    // ~Fifo(void);  compiler-generated destructor is OK

    // misc public methods
    char GetByte(void);
    void PreReceive(char *&dest, int &count);
    void PostReceive(int count);
    void Reset(void);

    // public inquiry methods
    bool HasData(void) const;
    bool HasSpace(void) const;
    bool IsValid(void) const;

private:
    // private constants
    static const int SIZE = 4096;

    // private data
    int  mConsumedCnt; // number of consumed bytes in the buffer
    char mReadBuffer[SIZE];
    int  mValidCnt;    // number of valid bytes in buffer, including those consumed

    // private lifecycle
    Fifo(Fifo const&); // not copyable

    // private operators
    Fifo& operator=(Fifo const&); // not assignable
};
#endif // !defined(FIFO_HPP_INCLUDED)
