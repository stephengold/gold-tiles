#ifndef NETWORK_HPP_INCLUDED
#define NETWORK_HPP_INCLUDED

// File:     network.hpp
// Location: src
// Purpose:  declare Network class
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
A Network object mediates the interactions between the application
instance and the network.  It enables an instance to act as either 
a client or a server.

The Network class is implemented as a collection of sockets, 
one for the instance to listen on when it is a server (invitee)
and others for it to send to when it is a client (inviter).
*/

#include <map>         // HASA std::map
#include "address.hpp" // HASA Address
#include "socket.hpp"  // HASA Socket
#ifdef _QT
# include <QTcpServer>
#endif // defined(_QT)


class Network {
public:
    // public constants
    static const String   ACCEPT;
    static const String   DECLINE;
    static const TextType SERVER_LISTEN_PORT;

    // public lifecycle
    Network(void);
    ~Network(void);

    // misc public methods
    static String AddressReport(void);
    static Socket CheckForConnection(void);
    static bool   ConnectToServer(Address const&, Game&);
    static Game*  ConsiderInvitation(Socket&, GameOpt const&, HandOpts&);
    static void   Notice(String const&);
    static bool   Question(String const&);
#ifdef _GUI
    static void   SetWindow(Window*);
#endif // defined(_GUI)

    // public inquiry methods
    static bool IsServerStarted(void);

private:
    // private constants
    static const int MAX_CONNECTION_CNT = 1;

    // private data
#ifdef _WINSOCK2
    static Socket       msListenIpv4;
    static Socket       msListenIpv6;
#elif defined(_QT)
    static QTcpServer* mspServer;
#endif // defined(_QT)
#ifdef _GUI
    static Window*     mspWindow;
#endif // defined(_GUI)

    // private lifecycle
    Network(Network const&); // not copyable

    // private operators
    Network& operator=(Network const&); // not assignable

    // misc private methods
    static void   DoneWaiting(void);
    static bool   InviteServer(Socket&, Game const&);
    static Socket OpenListen(int family);
    static Socket OpenServer(void* addrinfo_list, Address const& server);
    static bool   Retry(String const&);
    static bool   StartServer(void);
    static void   WaitingFor(String const&);
};
#endif // !defined(NETWORK_HPP_INCLUDED)

