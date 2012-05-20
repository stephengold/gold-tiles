// File:     handopts.cpp
// Location: src
// Purpose:  implement HandOpts class
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

#include <iostream>     // std::cout
#include "handopts.hpp"
#include "network.hpp"
#include "strings.hpp"


// lifecycle

HandOpts::HandOpts(void) {
}

// operators

HandOpt& HandOpts::operator[](SizeType ind) {
    ASSERT(ind < Count());

    return mList[ind];
}

HandOpt const& HandOpts::operator[](SizeType ind) const {
    ASSERT(ind < Count());

    return mList[ind];
}

HandOpts::operator String(void) const {
    String result;

    for (SizeType i_hand = 0; i_hand < Count(); i_hand++) {
        String const hand_opt_string = (*this)[i_hand];
        result += hand_opt_string;
    }

    return result;
}


// misc methods

void HandOpts::Append(HandOpt const& rHandOpt) {
    mList.push_back(rHandOpt);

    ASSERT(!IsEmpty());
}

Strings HandOpts::AllPlayerNames(void) const {
    Strings result;

    for (SizeType i_hand = 0; i_hand < Count(); i_hand++) {
        String const player_name = (*this)[i_hand].PlayerName();
        result.Append(player_name);
    }

    ASSERT(result.Count() <= Count());
    return result;
}

SizeType HandOpts::Count(void) const {
    SizeType const result = SizeType(mList.size());

    return result;
}

String HandOpts::Description(void) const {
    String result;

    for (SizeType i_hand = 0; i_hand < Count(); i_hand++) {
        String const hand_description = (*this)[i_hand].Description();
        result += "The " + ::ordinal(i_hand + 1) 
            + " hand will be played by " + hand_description + ".\n";
    }

    return result;
}

// Return true if successful, false if canceled.
bool HandOpts::GetFromClient(Socket& rClient, SizeType handCnt) {
    MakeEmpty();
    bool success = true;
    for (SizeType i_hand = 0; i_hand < handCnt; i_hand++) {
        String hand_opt_string;
        success = rClient.GetParagraph(hand_opt_string);
        if (!success) {
            break;
        }
        HandOpt const hand_opt = HandOpt(hand_opt_string);
        Append(hand_opt);
    }

    return success;
}

void HandOpts::GetUserChoice(SizeType handCnt) {
    for (SizeType i_hand = 0; i_hand < handCnt; i_hand++) {
        String name;
        while (name.IsEmpty()) {
            std::cout << "Who will play the " 
                << ::ordinal(i_hand + 1)
                << " hand?\n ('computer' or 'network' or the name of a local user) ";
            std::getline(std::cin, name);
            name.Capitalize();
        }

        HandOpt opt;

        if (name == "Computer") {
            // TODO set difficulty
            opt.SetPlayerName(name);
            opt.SetAutomatic();

        } else if (name == "Network") {
            String remote_name;
            while (remote_name.IsEmpty()) {
                std::cout << "Name of network player? ";
                std::getline(std::cin, remote_name);
                remote_name.Capitalize();
            }

            Address address;
            String address_string;
            while (address_string != address.operator String()) {
                std::cout << "Network address of " << remote_name << "'s computer? ";
                std::cin >> address_string;
                address = Address(address_string);
            }
            String end_of_line;
            std::getline(std::cin, end_of_line);

            opt.SetRemote();
            opt.SetAddress(address);
            opt.SetPlayerName(remote_name);

        } else {
            opt.SetLocalUser();
            opt.SetPlayerName(name);
        }
        Append(opt);
    }
}

void HandOpts::MakeEmpty(void) {
    mList.clear();

    ASSERT(IsEmpty());
}

void HandOpts::Serverize(Address const& rClient, Address const& rServer) {
    for (SizeType i_hand = 0; i_hand < Count(); i_hand++) {
        HandOpt &r_hand_opt = (*this)[i_hand];
        r_hand_opt.Serverize(rClient, rServer);
    }
}

void HandOpts::Truncate(SizeType new_length) {
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
