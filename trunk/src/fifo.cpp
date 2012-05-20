// File:     fifo.cpp
// Location: src
// Purpose:  implement Fifo class
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

#include <climits>
#include "fifo.hpp"
#include "project.hpp" // ASSERT()


// lifecycle

// construct an empty buffer
Fifo::Fifo(void) {
    mSize = INITIAL_SIZE;
    mpBuffer = new char[mSize];
    ASSERT(mpBuffer != NULL);

    Reset();
}

void Fifo::Reset(void) {
    mConsumedCnt = 0;
    mValidCnt = 0;

    ASSERT(IsValid());
    ASSERT(HasSpace());
}


// misc methods

char Fifo::GetByte(void) {
    ASSERT(IsValid());
    ASSERT(HasData());

    char const result = mpBuffer[mConsumedCnt];
    mConsumedCnt++;

    ASSERT(IsValid());
    return result;
}

void Fifo::PostReceive(SizeType byteCnt) {
    ASSERT(IsValid());
    ASSERT(byteCnt >= 0);

    mValidCnt += byteCnt;

    ASSERT(IsValid());
}

void Fifo::PreReceive(char*& rpStart, SizeType& rByteCnt) {
    ASSERT(IsValid());

    if (!HasSpace()) {
        mSize *= 2;
        char* p_buffer = new char[mSize];
        for (SizeType i_byte = mConsumedCnt; i_byte < mValidCnt; i_byte++) {
            p_buffer[i_byte] = mpBuffer[i_byte];
        }
        delete mpBuffer;
    }

    if (mConsumedCnt == mValidCnt) {
        Reset();
    }

    rpStart = mpBuffer + mValidCnt;
    SizeType byte_cnt = mSize - mValidCnt;
    rByteCnt = byte_cnt;

    ASSERT(rByteCnt > 0);
    ASSERT(rpStart != NULL);
}


// inquiry methods

bool Fifo::HasData(void) const {
    bool const result = (mConsumedCnt < mValidCnt);

    return result;
}

bool Fifo::HasSpace(void) const {
    bool const result = (mValidCnt < mSize || mConsumedCnt == mValidCnt);

    return result;
}

bool Fifo::IsValid(void) const {
    bool result = true;

    if (mConsumedCnt < 0
        || mConsumedCnt > mValidCnt
        || mValidCnt < 0
        || mValidCnt > mSize
        || mSize < 0)
    {
        result = false;
    }

    return result;
}
