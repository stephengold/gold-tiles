// File:     handopt.cpp
// Location: src
// Purpose:  implement HandOpt class
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

#include "handopt.hpp"
#include "strings.hpp"


// lifecycle

HandOpt::HandOpt(GameStyleType gameStyle, Strings const& rPlayerNames) {
	switch (gameStyle) {
		case GAME_STYLE_PRACTICE:
			SetPlayerName("Player");
			break;
		case GAME_STYLE_DEBUG:
			SetPlayerName("Tester");
			break;
		case GAME_STYLE_FRIENDLY:
		case GAME_STYLE_CHALLENGE: {
			String const name = rPlayerNames.InventUnique("Player");
			SetPlayerName(name);
			break;
		}
		default:
			FAIL();
	}

	mAutomaticFlag = false;
    mIpAddress = 0;
	mRemoteFlag = false;
	mSkipProbability = 0.0;
}

HandOpt::HandOpt(String const& rPlayerName) {
	mAutomaticFlag = false;
	mIpAddress = 0;
	SetPlayerName(rPlayerName);
	mRemoteFlag = false;
	mSkipProbability = 0.0;
}

HandOpt::HandOpt(
	String const& rPlayerName,
	bool isAutomatic,
	double skipProbability,
	bool isRemote,
	IpAddressType address)
{
	mAutomaticFlag = isAutomatic;
	mIpAddress = address;
	SetPlayerName(rPlayerName);
	mRemoteFlag = isRemote;
	mSkipProbability = skipProbability;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.

HandOpt::operator IpAddressType(void) const {
	return mIpAddress;
}


// misc methods

String HandOpt::PlayerName(void) const {
	return mPlayerName;
}

void HandOpt::SetAutomatic(void) {
	mAutomaticFlag = true;
	mRemoteFlag = false;
	ASSERT(IsAutomatic());
}

void HandOpt::SetLocalUser(void) {
	mAutomaticFlag = false;
	mRemoteFlag = false;
	ASSERT(IsLocalUser());
}

void HandOpt::SetRemote(void) {
	mRemoteFlag = true;
	ASSERT(IsRemote());
}

void HandOpt::SetPlayerName(String const& rPlayerName) {
	mPlayerName = rPlayerName;
	mPlayerName.Capitalize();
}

void HandOpt::SetSkipProbability(double skipProbability) {
	ASSERT(skipProbability >= 0.0);
	ASSERT(skipProbability < 1.0);

	mSkipProbability = skipProbability;
}

double HandOpt::SkipProbability(void) const {
	ASSERT(mSkipProbability >= 0.0);
	ASSERT(mSkipProbability < 1.0);

	return mSkipProbability;
}


// inquiry methods

bool HandOpt::HasValidName(void) const {
    bool const result = !mPlayerName.IsEmpty();

	return result;
}

bool HandOpt::IsAutomatic(void) const {
	bool const result = (mAutomaticFlag && !mRemoteFlag);

	return result;
}

bool HandOpt::IsLocalUser(void) const {
	bool const result = (!mAutomaticFlag && !mRemoteFlag);

	return result;
}

bool HandOpt::IsRemote(void) const {
	return mRemoteFlag;
}
