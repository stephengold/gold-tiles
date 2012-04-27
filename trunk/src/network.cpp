// File:     network.cpp
// Location: src
// Purpose:  networking functions for the Gold Tile Game
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

#include <iostream>      // cout
#include "game.hpp"
#include "handopts.hpp"
#include "network.hpp"
#include "strings.hpp"

#ifdef _WINSOCK2
# define WIN32_LEAN_AND_MEAN
# include "gui/win_types.hpp"
namespace Win {
# include <winsock2.h>
# include <ws2tcpip.h>
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
};
using Win::ADDRINFOA;
using Win::IPPROTO_TCP;
using Win::PADDRINFOA;
using Win::SOCKET;
using Win::u_long;
using Win::WSADATA;
#endif // defined(_WINSOCK2)


// static constants

const String   Network::ACCEPT("Accept");
const String   Network::DECLINE("Decline");
const TextType Network::SERVER_LISTEN_PORT = "27066";


// static data

Socket       Network::msListen;


// lifecycle

Network::Network(void) {
#ifdef WIN32
    // Start Winsock.
    WSADATA wsa_data;
    int failure = Win::WSAStartup(MAKEWORD(2,2), &wsa_data);
    ASSERT(failure == 0);
#endif // defined(WIN32)

#ifdef _SERVER
    bool success = false;
    while (!success) {
        success = StartServer();
        if (!success) {
#ifdef _GUI
            FAIL(); // TODO retry
#else // !defined(_GUI)
            std::cout << "Press the Enter key to retry ..." << std::endl;
            String end_of_line;
            std::getline(std::cin, end_of_line);
#endif // !defined(_GUI)
        }
    }
#endif // defined(_SERVER)
}

/* static */ bool Network::StartServer(void) {
    ASSERT(!msListen.IsValid());

    // Get address info for the server's listening socket.
    ADDRINFOA address_hints;
    ::ZeroMemory(&address_hints, sizeof(address_hints));
    address_hints.ai_family = AF_INET; // for now, just use IPv4
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;
    address_hints.ai_flags = AI_PASSIVE;

    PCSTR const any_client = NULL;
    PADDRINFOA address_list = NULL;
    int failure = Win::getaddrinfo(any_client, SERVER_LISTEN_PORT, 
        &address_hints, &address_list);
    ASSERT(failure == 0);

    msListen = OpenListen(address_list);

    Win::freeaddrinfo(address_list);
    if (!msListen.IsValid()) {
        return false;
    }

    SOCKET server_listen = SOCKET(Socket::HandleType(msListen));

    // Put the (bound but unconnected) socket into listen mode.
    int const max_backlog = 1; // only one connection at a time
    failure = Win::listen(server_listen, max_backlog);
    ASSERT(failure == 0);

#ifndef _CONSOLE
    // Put socket into non-blocking mode.
    u_long nonblocking = 1;
    failure = Win::ioctlsocket(server_listen, FIONBIO, &nonblocking);
    ASSERT(failure == 0);
#endif // !defined(_CONSOLE)

    return true;
}

Network::~Network(void) {
#ifdef WIN32
    // Cleanly terminate Winsock.
    Win::WSACleanup();
#endif // defined(WIN32)
}


// misc methods

/* static */ String Network::AddressReport(void) {
    // Get list of addresses.
    Strings const list = Address::ListAll();
    unsigned const cnt = list.Count();

    String result;
    // Generate a brief report.
    if (cnt == 0) {
        result = "This computer no usable network address.";
    } else {
        if (cnt == 1) {
            result = "This computer has one usable network address";
        } else {
            ASSERT(cnt > 1);
            result = "This computer has " + String(cnt) + " usable network addresses";
        }
        result += ":\n " + String(list, "\n ");
    }

    return result;
}

// SERVER: Check for a new connection request.
/* static */ Socket Network::CheckForConnection(void) {
    Socket result;

    Socket::HandleType const handle = msListen;
    SOCKET const listen_socket = SOCKET(handle);
    SOCKET const data_socket = Win::accept(listen_socket, NULL, NULL);
    if (data_socket == INVALID_SOCKET) {
        int const error_code = Win::WSAGetLastError();
        ASSERT(error_code == WSAEWOULDBLOCK);
        // There's no connection yet.
    } else {
        // Put new data socket into blocking mode.
        u_long blocking = 0;
        int const failure = Win::ioctlsocket(data_socket, FIONBIO, &blocking);
        ASSERT(failure == 0);
        result = Socket(Socket::HandleType(data_socket));
    }

    return result;
}

// CLIENT:  Open a connection to a particular server.
/* static */ void Network::ConnectToServer(
    Address const& rAddress,
    Game& rGame)
{
    ASSERT(!rGame.IsConnectedToServer(rAddress));
    ASSERT(rGame.AmClient());

    // Get an address for the client's send socket.
    ADDRINFOA address_hints;
    ::ZeroMemory(&address_hints, sizeof(address_hints));
    address_hints.ai_family = AF_INET; // for now, just use IPv4
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;

    String const server = rAddress;
    PCSTR const server_name = server;
    PADDRINFOA address_list = NULL;
    int const failure = Win::getaddrinfo(server_name, SERVER_LISTEN_PORT, 
        &address_hints, &address_list);
    ASSERT(failure == 0);

    std::cout << "Connecting to port " << SERVER_LISTEN_PORT 
        << " on " << server << "." << std::endl;

    // Open the send socket.
    SOCKET client_send = INVALID_SOCKET;
    while (client_send == INVALID_SOCKET) {
        Socket::HandleType const socket = OpenServer(address_list, rAddress);
        client_send = SOCKET(socket);
        if (client_send == INVALID_SOCKET) {
#ifdef _GUI
            FAIL(); // TODO retry
#else // !defined(_GUI)
            std::cout << "Press the Enter key to retry ..." << std::endl;
            String end_of_line;
            std::getline(std::cin, end_of_line);
#endif // !defined(_GUI)
        }
    }
    Win::freeaddrinfo(address_list);

    // Save the open socket to all relevant hand objects.
    Socket socket = Socket(Socket::HandleType(client_send));
    rGame.AddServer(rAddress, socket);
    ASSERT(rGame.IsConnectedToServer(rAddress));

    // Invite the server to play.
    InviteServer(socket, rGame);
    std::cout << "Waiting for " << server << " to accept your invitation ..." << std::endl;
    String const response = socket.GetLine();
    if (response == Network::ACCEPT) {
        std::cout << server << " accepted." << std::endl;

        // Put the entire stock of tiles.
        String const stock = rGame.StockBagString();
        socket.PutLine(stock);

    } else if (response == Network::DECLINE) {
        std::cout << server << " declined." 
            << std::endl;
        FAIL(); // TODO recovery

    } else {
        std::cout << "Unexpected response: " << response.Quote() << "." 
            << std::endl;
        FAIL(); // TODO recovery
    }
}

// SERVER:  Attempt to bind to a listen socket.
/* static */ Socket Network::OpenListen(void *p) {
    PADDRINFOA const addrinfo_list = PADDRINFOA(p);

    SOCKET server_listen = INVALID_SOCKET;
    for (PADDRINFOA p_info = addrinfo_list; p_info != NULL; p_info = p_info->ai_next) {
        // Create a listen socket.
        ASSERT(p_info->ai_family == AF_INET);
        ASSERT(p_info->ai_socktype == SOCK_STREAM);
        ASSERT(p_info->ai_protocol == IPPROTO_TCP);
        server_listen = Win::socket(p_info->ai_family, p_info->ai_socktype, p_info->ai_protocol);
        ASSERT(server_listen != INVALID_SOCKET);

        // Attempt to bind to the socket.
        int const failure = Win::bind(server_listen, p_info->ai_addr, int(p_info->ai_addrlen));
        if (failure == 0) {
            // success
            break;
        }
        int const error_code = Win::WSAGetLastError();
        if (error_code == WSAEADDRINUSE) {
            std::cout << "Port " << SERVER_LISTEN_PORT 
                << " is busy on this computer.\nIs there a Gold Tile server already running?"
                << std::endl;
            server_listen = INVALID_SOCKET;
            // try next addrinfo
        } else {
            std::cout << "Unexpected Winsock error code " << error_code 
                << " on bind." << std::endl;
            FAIL();
        }
    }

    Socket const result = Socket(Socket::HandleType(server_listen));

    return result;
}

// CLIENT:  Attempt to connect to a server.
/* static */ Socket Network::OpenServer(void* p, Address const& rServer) {
    PADDRINFOA const addrinfo_list = PADDRINFOA(p);

    SOCKET client_send = INVALID_SOCKET;
    for (PADDRINFOA p_info = addrinfo_list; p_info != NULL; p_info = p_info->ai_next) {
        // Create a send socket.
        ASSERT(p_info->ai_family == AF_INET);
        ASSERT(p_info->ai_socktype == SOCK_STREAM);
        ASSERT(p_info->ai_protocol == IPPROTO_TCP);
        client_send = Win::socket(p_info->ai_family, p_info->ai_socktype, p_info->ai_protocol);
        ASSERT(client_send != INVALID_SOCKET);	

        // Attempt to connect to server.
        int const failure = Win::connect(client_send, p_info->ai_addr, int(p_info->ai_addrlen));
        if (failure == 0) {
            // success
            break;
        }
        int const error_code = Win::WSAGetLastError();
        if (error_code == WSAECONNREFUSED) {
            std::cout << "No response.  Make sure a Gold Tile server is running on " 
                << String(rServer) << std::endl;
            client_send = INVALID_SOCKET;
            // try next addrinfo
        } else {
            std::cout << "Unexpected Winsock error code " << error_code 
                << " on connect." << std::endl;
            FAIL();
        }
    } 

    Socket const result = Socket(Socket::HandleType(client_send));

    return result;
}

// CLIENT:  Invite a server to participate in the current game.
/* static */ void Network::InviteServer(Socket& rSocket, Game const& rGame) {
    Address const peer = rSocket.Peer();
    std::cout << "Sending invitation to server at " << String(peer) << std::endl;

    GameOpt const game_opt = rGame;
    String const game_opt_string = game_opt;
    rSocket.Put(game_opt_string);

    Hands const hands = rGame;
    Hands::ConstIterator i_hand;
    for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
        HandOpt const hand_opt = *i_hand;
        String const hand_opt_string = hand_opt;
        rSocket.Put(hand_opt_string);
    }
}
