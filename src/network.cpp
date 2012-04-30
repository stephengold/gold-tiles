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

#ifdef _GUI
# include "gui/window.hpp"
#endif // defined(_GUI)


// static constants

const String   Network::ACCEPT("Accept");
const String   Network::DECLINE("Decline");
const TextType Network::SERVER_LISTEN_PORT = "27066";


// static data

Socket   Network::msListen;
#ifdef _GUI
Window* Network::mspWindow = NULL;
#endif // defined(_GUI)


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
            Retry("Startup failed.");
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

#ifdef _GUI
    // Put listen socket into non-blocking mode.
    u_long nonblocking = 1;
    failure = Win::ioctlsocket(server_listen, FIONBIO, &nonblocking);
    ASSERT(failure == 0);
#endif // defined(_GUI)

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

    SOCKET const listen_socket = SOCKET(Socket::HandleType(msListen));
    SOCKET const data_socket = Win::accept(listen_socket, NULL, NULL);
    if (data_socket == INVALID_SOCKET) {
        int const error_code = Win::WSAGetLastError();
        ASSERT(error_code == WSAEWOULDBLOCK);
        // There's no connection yet.
    } else {
#ifdef _GUI
        // Put new data socket into non-blocking mode.
        u_long non_blocking = 1;
        int const failure = Win::ioctlsocket(data_socket, FIONBIO, &non_blocking);
        ASSERT(failure == 0);
#endif // defined(_GUI)
        result = Socket(Socket::HandleType(data_socket));
    }

    return result;
}

// CLIENT:  Open a connection to a particular server.
/* static */ bool Network::ConnectToServer(
    Address const& rAddress,
    Game& rGame)
{
    ASSERT(!rGame.IsConnectedToServer(rAddress));
    ASSERT(rGame.AmClient());

    // Get address info for the client's send socket.
    ADDRINFOA address_hints;
    ::ZeroMemory(&address_hints, sizeof(address_hints));
    address_hints.ai_family = AF_INET; // for now, just use IPv4
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;

    String const server = rAddress;
    PCSTR const server_name = server;
    PADDRINFOA address_list = NULL;
    int const get_addr_failure = Win::getaddrinfo(server_name, SERVER_LISTEN_PORT, 
        &address_hints, &address_list);
    ASSERT(get_addr_failure == 0);

    String const server_description = String("port ") + SERVER_LISTEN_PORT 
        + " on " + server;
    String const attempt_message = String("Connecting to ") 
        + server_description;
    Network::Notice(attempt_message);

    // Open a data socket to the server.
    SOCKET data_socket = INVALID_SOCKET;
    while (data_socket == INVALID_SOCKET) {
        Socket::HandleType const data_handle = OpenServer(address_list, rAddress);
        data_socket = SOCKET(data_handle);
        if (data_socket == INVALID_SOCKET) {
            String const retry_message = String("Failed to connect to ") 
                + server_description;
            bool const retry = Network::Retry(retry_message);
            if (!retry) {
                break;
            }
        }
    }
    Win::freeaddrinfo(address_list);

    if (data_socket == INVALID_SOCKET) {
        return false;
    }

#ifdef _GUI
    // Put listen socket into non-blocking mode.
    u_long nonblocking = 1;
    int const ioctl_failure = Win::ioctlsocket(data_socket, FIONBIO, &nonblocking);
    ASSERT(ioctl_failure == 0);
#endif // defined(_GUI)

    // Invite the server to play.
    Socket socket = Socket(Socket::HandleType(data_socket));
    bool success = InviteServer(socket, rGame);
    if (!success) {
        return false;
    }
    String const status = String("Waiting for ") + server 
        + " to accept your invitation ...";
    Network::Notice(status);

    String response;
    success = socket.GetLine(response);
    if (!success) {
        return false;
    }
    
    if (response == Network::DECLINE) {
        String const report = server + " declined.";
        Network::Notice(report);
        return false;
    }

    if (response != Network::ACCEPT) {
        String const report = server + " sent an unexpected response: " 
            + response.Quote() + ".";
        Network::Notice(report);
        return false;
    }

    // Put the entire stock of tiles.
    String const stock = rGame.StockBagString();
    bool const was_successful = socket.PutLine(stock);

    if (was_successful) {
        // Notify the user(s).
        String const report = server + " accepted.";
        Network::Notice(report);
    }

    // Save the open socket to all relevant hand objects.
    rGame.AddServer(rAddress, socket);
    ASSERT(rGame.IsConnectedToServer(rAddress));

    return was_successful;
}

// CLIENT:  Invite a server to participate in the current game.
/* static */ bool Network::InviteServer(Socket& rSocket, Game const& rGame) {
    Address const peer = rSocket.Peer();
    std::cout << "Sending invitation to " << String(peer) << std::endl;

    GameOpt const game_opt = rGame;
    String const game_opt_string = game_opt;
    bool was_successful = rSocket.Put(game_opt_string);
    if (!was_successful) {
        return false;
    }

    Hands const hands = rGame;
    Hands::ConstIterator i_hand;
    for (i_hand = hands.begin(); i_hand != hands.end(); i_hand++) {
        HandOpt const hand_opt = *i_hand;
        String const hand_opt_string = hand_opt;
        bool was_successful = rSocket.Put(hand_opt_string);
        if (!was_successful) {
            return false;
        }
    }

    return true;
}

/* static */ void Network::Notice(String const& rMessage) {
#ifdef _GUI
    ASSERT(mspWindow != NULL);
    String const text = rMessage + "\nClick the OK button to continue.";
    mspWindow->InfoBox(text, "Gold Tile Game - Network Notice");
#else // !defined(_GUI)
    std::cout << rMessage << std::endl;
#endif // !defined(_GUI)
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
        server_listen = Win::socket(p_info->ai_family, 
            p_info->ai_socktype, p_info->ai_protocol);
        ASSERT(server_listen != INVALID_SOCKET);

        // Attempt to bind to the socket.
        int const failure = Win::bind(server_listen, 
            p_info->ai_addr, int(p_info->ai_addrlen));
        if (failure == 0) {
            // success
            break;
        }
        int const error_code = Win::WSAGetLastError();
        if (error_code == WSAEADDRINUSE) {
            String const message = String("Port ")
                + SERVER_LISTEN_PORT + " on this computer is busy.\n"
                + "Perhaps there is another Gold Tile server running.";
            Notice(message);
            server_listen = INVALID_SOCKET;
            // try next addrinfo
        } else {
            std::cout << "Unexpected Winsock error code " << error_code 
                << " on bind." << std::endl;
            FAIL();
        }
    }

    Socket result;
    if (server_listen != INVALID_SOCKET) {
        result = Socket(Socket::HandleType(server_listen));
    }

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
        if (error_code == WSAECONNREFUSED
            || error_code == WSAETIMEDOUT) {
                String const server(rServer);
                String const server_description = String("port ") + SERVER_LISTEN_PORT 
                    + " on " + server;
                String const message = String("No response from ")
                    + server_description
                    + "\n\nPlease make sure that a Gold Tile server is running on " 
                    + server;
                Notice(message);
                client_send = INVALID_SOCKET;
                // try next addrinfo
        } else {
            std::cout << "Unexpected Winsock error code " << error_code 
                << " on connect." << std::endl;
            FAIL();
        }
    } 

    Socket result;
    if (client_send != INVALID_SOCKET) {
        result = Socket(Socket::HandleType(client_send));
    }

    return result;
}

// Return true if user chooses "yes", false otherwise.
/* static */ bool Network::Question(String const& rQuestion) {
#ifdef _GUI
    ASSERT(mspWindow != NULL);
    int const accept = mspWindow->QuestionBox(rQuestion, "Gold Tile Game - Network Question");
    bool const result = (accept == IDYES);

    return result;
#else // !defined(_GUI)
    std::cout << rQuestion << " (y/n) ";
    String yesno;
    for (;;) {
        std::cin >> yesno;
        yesno.Capitalize();
        if (yesno.HasPrefix("Y")) {
            break;
        } else if (yesno.HasPrefix("N")) {
            return false;
        }
    }

    return true;
#endif // !defined(_GUI)
}

/* static */ bool Network::Retry(String const& rMessage) {
    bool result = true;

#ifdef _GUI
    ASSERT(mspWindow != NULL);
    result = mspWindow->RetryBox(rMessage, "Gold Tile Game - Network Retry");
#else // !defined(_GUI)
    std::cout << rMessage << std::endl;
    std::cout << "Press the Enter key to retry ..." << std::endl;
    String end_of_line;
    std::getline(std::cin, end_of_line);
    // The console version retries until something kills the application,
    // such as a local user pressing Ctrl+C.
#endif // !defined(_GUI)

    return result;
}

#ifdef _GUI
/* static */ void Network::SetWindow(Window* pWindow) {
    mspWindow = pWindow;
}
#endif // defined(_GUI)

