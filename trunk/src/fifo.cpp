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

#include "fifo.hpp"
#include "project.hpp" // ASSERT()


// lifecycle

// construct an empty buffer
Fifo::Fifo(void) {
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

	char const result = mReadBuffer[mConsumedCnt];
	mConsumedCnt++;

	ASSERT(IsValid());
	return result;
}

void Fifo::PostReceive(int byteCnt) {
	ASSERT(IsValid());
	ASSERT(byteCnt >= 0);

	mValidCnt += byteCnt;

	ASSERT(IsValid());
}

void Fifo::PreReceive(char *&rpStart, int &rByteCnt) {
	ASSERT(IsValid());
	ASSERT(HasSpace());

	if (mConsumedCnt == mValidCnt) {
		Reset();
	}

	rpStart = mReadBuffer + mValidCnt;
	rByteCnt = SIZE - mValidCnt;

	ASSERT(rByteCnt > 0);
	ASSERT(rpStart != NULL);
}


// inquiry methods

bool Fifo::HasData(void) const {
	bool const result = (mConsumedCnt < mValidCnt);

	return result;
}

bool Fifo::HasSpace(void) const {
	bool const result = (mValidCnt < SIZE || mConsumedCnt == mValidCnt);

	return result;
}

bool Fifo::IsValid(void) const {
	bool result = true;

	if (mConsumedCnt < 0
	 ||	mConsumedCnt > mValidCnt
	 || mValidCnt < 0
	 || mValidCnt > SIZE)
	{
		result = false;
	}

	return result;
}
