#ifndef HANDOPT_HPP_INCLUDED
#define HANDOPT_HPP_INCLUDED

// File:     handopt.hpp
// Location: src
// Purpose:  declare HandOpt class
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
A HandOpt object represents the configuration options for a Hand.

The HandOpt class encapsulates the hand's name and type, plus optional
characteristics such as its IP address.
*/

#include "address.hpp"     // HASA Address

enum GameStyleType {
    GAME_STYLE_NONE,
    GAME_STYLE_DEBUG,     // allows peeking, undo, all hints; clock is optional
    GAME_STYLE_PRACTICE,  // no peeking; allows undo, all hints; clock is optional
    GAME_STYLE_FRIENDLY,  // no peeking, no undo; allows all hints; clock is optional
    GAME_STYLE_CHALLENGE, // no peeking, no undo, no hints; time limits
#ifdef _DEBUG
    GAME_STYLE_DEFAULT = GAME_STYLE_DEBUG,
#else  // !defined(_DEBUG)
    GAME_STYLE_DEFAULT = GAME_STYLE_PRACTICE
#endif  // !defined(_DEBUG)
};

typedef int32_t SecondsType;  // up to 78 years

class HandOpt {
public:
    // public types
    typedef uint16_t LevelType;

    // public constants
    static const LevelType LEVEL_DEFAULT = 10;
    static const LevelType LEVEL_MAX = 10;
    static const LevelType LEVEL_MIN = 2;

    // public lifecycle
    HandOpt(void);
    // HandOpt(HandOpt const&);  implicitly defined copy constructor
    explicit HandOpt(String const&);
    HandOpt(GameStyleType, Strings const& playerNames);
    HandOpt(String const& playerName, bool autom, Fraction const& skipProb, 
        bool rem, Address const&);
    // ~HandOpt(void);  implicitly defined destructor

    // public operators
    // HandOpt& operator=(HandOpt const&);  implicitly defined assignment operator
    operator Address(void) const;
    operator String(void) const;

    // misc public methods
    String    Description(void) const;
    LevelType Level(void) const;
    String    PlayerName(void) const;
    void      Serverize(Address const& client, Address const& server);
    void      SetAddress(Address const&);
    void      SetAutomatic(void);
    void      SetLevel(LevelType);
    void      SetLocalUser(void);
    void      SetPlayerName(String const&);
    void      SetRemote(void);
    Fraction  SkipProbability(void) const;

    // public inquiry methods
    bool IsAutomatic(void) const;
    bool IsLocalUser(void) const;
    bool IsRemote(void) const;
    bool IsValid(void) const;

private:
    // private data
    Address  mAddress;          // for remote hands only, else ignored
    bool     mAutomaticFlag;
    String   mPlayerName;
    bool     mRemoteFlag;
    Fraction mSkipProbability;  // for automatic hands only, else 0.0

    // private methods
    void      SetSkipProbability(Fraction const&);
};

// global utility functions
String        game_style_to_string(GameStyleType);
GameStyleType string_to_game_style(String const&);

#endif  // !defined(HANDOPT_HPP_INCLUDED)
