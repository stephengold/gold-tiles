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
#include "socket.hpp"
#include "strings.hpp"


// lifecycle

HandOpt::HandOpt(void) {
    mAutomaticFlag = false;
    mRemoteFlag = false;
    mSkipProbability = 0.0;
}

HandOpt::HandOpt(String const& rString) {
    mSkipProbability = 0.0;

    Strings const lines(rString, "\n");
    Strings::ConstIterator i_line;
    for (i_line = lines.Begin(); i_line != lines.End(); i_line++) {
        String const line = *i_line;
        Strings const fields(line, "=");
        ASSERT(fields.Count() == 2); // TODO recovery
        String const name = fields.First();
        String const value = fields.Second();
        if (name == "PlayerName") {
            mPlayerName = value;
        } else if (name == "AutomaticFlag") {
            mAutomaticFlag = bool(value);
        } else if (name == "RemoteFlag") {
            mRemoteFlag = bool(value);
        } else if (name == "SkipProbability") {
            mSkipProbability = double(value);
        } else if (name == "Address") {
            mAddress = Address(value);
        } else {
            FAIL(); // TODO recovery
        }
    }
}

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
    mRemoteFlag = false;
    mSkipProbability = 0.0;
}

HandOpt::HandOpt(
    String const& rPlayerName,
    bool isAutomatic,
    double skipProbability,
    bool isRemote,
    Address const& rAddress)
    :
mAddress(rAddress)
{
    mAutomaticFlag = isAutomatic;
    SetPlayerName(rPlayerName);
    mRemoteFlag = isRemote;
    mSkipProbability = skipProbability;
}

// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.

HandOpt::operator Address(void) const {
    return mAddress;
}

HandOpt::operator String(void) const {
    String result;

    result += "PlayerName=" + mPlayerName + "\n";
    result += "AutomaticFlag=" + String(mAutomaticFlag) + "\n";
    result += "RemoteFlag=" + String(mRemoteFlag) + "\n";
    if (IsAutomatic()) {
        result += "SkipProbability=" + String(mSkipProbability) + "\n";
    }
    if (IsRemote()) {
        result += "Address=" + String(mAddress) + "\n";
    }

    result += "\n";

    return result;
}


// misc methods

String HandOpt::PlayerName(void) const {
    return mPlayerName;
}

void HandOpt::Serverize(Address const& rClient, Address const& rServer) {
    if (!IsRemote()) {
        mAddress = rClient;
        mRemoteFlag = true;
    } else if (mAddress == rServer) {
        mRemoteFlag = false;
    }
    mAutomaticFlag = false;
}

void HandOpt::SetAddress(Address const& rAddress) {
    mAddress = rAddress;
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


// global utility functions

String game_style_to_string(GameStyleType style) {
    String result;
    switch (style) {
    case GAME_STYLE_DEBUG:
        result = "debug";
        break;
    case GAME_STYLE_PRACTICE:
        result = "practice";
        break;
    case GAME_STYLE_FRIENDLY:
        result = "friendly";
        break;
    case GAME_STYLE_CHALLENGE:
        result = "challenge";
        break;
    default:
        FAIL();
    }

    return result;
}

GameStyleType string_to_game_style(String const& rString) {
    GameStyleType result = GAME_STYLE_NONE;
    if (rString == "debug") {
        result = GAME_STYLE_DEBUG;
    } else if (rString == "practice") {
        result = GAME_STYLE_PRACTICE;
    } else if (rString == "friendly") {
        result = GAME_STYLE_FRIENDLY;
    } else if (rString == "challenge") {
        result = GAME_STYLE_CHALLENGE;
    } else {
        FAIL();
    }
    return result;
}
