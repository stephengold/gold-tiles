#ifndef HANDOPT_HPP_INCLUDED
#define HANDOPT_HPP_INCLUDED

// File:    handopt.hpp
// Purpose: HandOpt class header file
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
A HandOpt object represents the configuration options for a Hand.

The HandOpt class encapsulates the hand's name and type, plus optional
characteristics such as its IP address.
*/

#include "string.hpp"  // HASA String

enum GameStyleType {
    GAME_STYLE_NONE,
	GAME_STYLE_DEBUG,     // allows peeking, undo, all hints; clock is optional
	GAME_STYLE_PRACTICE,  // no peeking; allows undo, all hints; clock is optional
	GAME_STYLE_FRIENDLY,  // no peeking, no undo; allows all hints; clock is optional
	GAME_STYLE_CHALLENGE, // no peeking, no undo, no hints; time limits
#ifdef _DEBUG
	GAME_STYLE_DEFAULT = GAME_STYLE_DEBUG,
#else // !defined(_DEBUG)
	GAME_STYLE_DEFAULT = GAME_STYLE_PRACTICE
#endif // !defined(_DEBUG)
};

typedef unsigned long IpAddressType;

class HandOpt {
public:
	// public lifecycle
	HandOpt(GameStyleType, Strings const &playerNames);
	HandOpt(String const &playerName);
	HandOpt(String const &playerName, bool autop, double skipProb, bool rem, IpAddressType);
	// no default constructor
    // HandOpt(HandOpt const &);  compiler-generated copy constructor is OK
    // ~HandOpt(void);  compiler-generated destructor is OK

	// public operators
    // HandOpt &operator=(HandOpt const &);  compiler-generated assignment operator is OK
	operator IpAddressType(void) const;

	// misc public methods
	String PlayerName(void) const;
	void   SetAutomatic(void);
	void   SetLocalUser(void);
	void   SetPlayerName(String const &);
	void   SetRemote(void);
	void   SetSkipProbability(double);
	double SkipProbability(void) const;

	// public inquiry methods
	bool HasValidName(void) const;
	bool IsAutomatic(void) const;
	bool IsLocalUser(void) const;
	bool IsRemote(void) const;

private:
	// private data
	bool   mAutomaticFlag;
	IpAddressType 
		   mIpAddress;       // for remote hands only, else ignored
	String mPlayerName;
	bool   mRemoteFlag;
	double mSkipProbability; // for automatic hands only, else 0.0
};
#endif  // !defined(HANDOPT_HPP_INCLUDED)