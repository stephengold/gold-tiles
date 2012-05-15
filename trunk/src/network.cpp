// File:     network.cpp
// Location: src
// Purpose:  implement Network class
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

#include <iostream>      // std::cout
#include "game.hpp"
#include "handopts.hpp"
#include "network.hpp"
#include "strings.hpp"

#ifdef _WINSOCK2
# define WIN32_LEAN_AND_MEAN  // to avoid macro redefinitions
# include "gui/win_types.hpp"
namespace Win {
# include <ws2tcpip.h>   // for getaddrinfo
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
};
using Win::ADDRINFOA;
using Win::IPPROTO_TCP;
using Win::PADDRINFOA;
using Win::SOCKET;
using Win::WSADATA;
#endif // defined(_WINSOCK2)

#ifdef _GUI
# include "gui/window.hpp"
#endif // defined(_GUI)


// static constants

const String Network::ACCEPT("Accept");
const String Network::DECLINE("Decline");
const TextType Network::LISTEN_PORT_DEFAULT = "27066";


// static data

#ifdef _WINSOCK2
Socket Network::msListenIpv4;
Socket Network::msListenIpv6;
#endif // defined(_WINSOCK2)

String Network::msListenPort;

#ifdef _QT
QTcpServer* Network::mspServer = NULL;
#endif // defined(_QT)

#ifdef _GUI
Window* Network::mspWindow = NULL;
#endif // defined(_GUI)


// lifecycle

Network::Network(TextType listenPort) {
#ifdef _GUI
    mspWindow = NULL;
#endif // defined(_GUI)

    msListenPort = listenPort;
    if (long(listenPort) < PORT_MIN || long(listenPort) > PORT_MAX) {
        msListenPort = LISTEN_PORT_DEFAULT;
    }

#ifdef _WINSOCK2
    // Start Winsock.
    WSADATA wsa_data;
    WORD const version = MAKEWORD(2,2);
    int failure = Win::WSAStartup(version, &wsa_data);
    ASSERT(failure == 0);
    ASSERT(wsa_data.wVersion == version);
#endif // defined(_WINSOCK2)

#ifdef _SERVER
# ifdef _CONSOLE
    bool success = false;
    while (!success) {
        success = StartServer();
        if (!success) {
            Retry("Startup failed.");
        }
    }
# elif defined(_GUI)
    StartServer();
    // Report server startup failures later.
# endif // defined(_GUI)
#endif // defined(_SERVER)
}

Network::~Network(void) {
#ifdef _QT
    delete mspServer;
#elif defined(_WINSOCK2)
    // Cleanly terminate Winsock.
    Win::WSACleanup();
#endif // defined(_WINSOCK2)
}


// misc methods

/* static */ String Network::AddressReport(void) {
    // Get list of addresses.
    Strings const list = Address::ListAll();
    unsigned const cnt = list.Count();

    String result;
    // Generate a brief report.
    if (cnt == 0) {
        result = "This computer has no usable network address.";
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

#ifdef _GUI
    if (mspWindow == NULL) {
        return result;
    }
#endif // defined(_GUI)

#ifdef _QT

    ASSERT(mspServer != NULL);
# ifdef _CONSOLE
    mspServer->waitForNewConnection(-1);
# endif // defined(_CONSOLE)
    if (mspServer->hasPendingConnections()) {
        DoneWaiting();
        QTcpSocket* p_next = mspServer->nextPendingConnection();
        ASSERT(p_next != NULL);
        result = Socket(p_next);
    }

#elif defined(_WINSOCK2)

    SOCKET data_socket = INVALID_SOCKET;

    if (msListenIpv4.IsValid()) {
        SOCKET const listen_socket = SOCKET(Socket::HandleType(msListenIpv4));
        data_socket = Win::accept(listen_socket, NULL, NULL);
        if (data_socket == INVALID_SOCKET) {
            int const error_code = Win::WSAGetLastError();
            ASSERT(error_code == WSAEWOULDBLOCK);
            // There's IPv4 no connection to accept right now.
        }
    }
    if (data_socket == INVALID_SOCKET && msListenIpv6.IsValid()) {
        SOCKET const listen_socket = SOCKET(Socket::HandleType(msListenIpv6));
        data_socket = Win::accept(listen_socket, NULL, NULL);
        if (data_socket == INVALID_SOCKET) {
            int const error_code = Win::WSAGetLastError();
            ASSERT(error_code == WSAEWOULDBLOCK);
            // There's IPv6 no connection to accept right now.
        }
    }

    if (data_socket != INVALID_SOCKET) {
        result = Socket(Socket::HandleType(data_socket));
# ifdef _GUI
        // Put new data socket into non-blocking mode.
        result.MakeNonblocking();
# endif // defined(_GUI)
    }

#endif // defined(_WINSOCK2)

    return result;
}

// CLIENT:  Open a connection to a particular server.
/* static */ bool Network::ConnectToServer(
    Address const& rAddress,
    Game& rGame)
{
    ASSERT(!rGame.IsConnectedToServer(rAddress));
    ASSERT(rGame.AmClient());

    String const server = rAddress;

#ifdef _WINSOCK2
    // Get address info for the client's socket.
    ADDRINFOA address_hints;
    ::ZeroMemory(&address_hints, sizeof(address_hints));
    address_hints.ai_family = AF_UNSPEC;
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;

    PCSTR const server_name = server;
    PADDRINFOA address_list = NULL;
    int const get_addr_failure = Win::getaddrinfo(server_name, msListenPort, 
        &address_hints, &address_list);
    ASSERT(get_addr_failure == 0);
#endif // defined(_WINSOCK2)

    String const server_description = DescribeListenPort()
        + " on " + server;
    String const response_event = String("response from ") + server_description;

    String const retry_message = String("Failed to connect to ")
        + server_description;

    // Open a data socket to the server.

#ifdef _QT

    QTcpSocket* p_socket = new QTcpSocket();
    ASSERT(p_socket != NULL);

    quint16 const port = long(msListenPort);
    for (;;) {
        WaitingFor(response_event);
        p_socket->connectToHost(QHostAddress(rAddress), port);
        bool const success = p_socket->waitForConnected(5 * MSECS_PER_SECOND);
        DoneWaiting();
        if (success) {
            break;
        }
        bool const retry = Network::Retry(retry_message);
        if (!retry) {
           break;
        }
    }

    if (p_socket->state() != QAbstractSocket::ConnectedState) {
        return false;
    }
    Socket socket = Socket(p_socket);

#elif defined(_WINSOCK2)

    SOCKET data_socket = INVALID_SOCKET;

    for (;;) {
        WaitingFor(response_event);
        Socket::HandleType const data_handle = OpenServer(address_list, rAddress);
        DoneWaiting();
        data_socket = SOCKET(data_handle);
        if (data_socket != INVALID_SOCKET) {
            break;
        }
        bool const retry = Network::Retry(retry_message);
        if (!retry) {
            break;
        }
    }
    Win::freeaddrinfo(address_list);

    if (data_socket == INVALID_SOCKET) {
        return false;
    }

    Socket socket = Socket(Socket::HandleType(data_socket));
# ifdef _GUI
    // Put listen socket into non-blocking mode.
    socket.MakeNonblocking();
# endif // defined(_GUI)
#endif // defined(_WINSOCK2)

    // Invite the server to play.
    bool was_successful = InviteServer(socket, rGame);
    if (!was_successful) {
        return false;
    }
    String const description = server + " to accept your invitation";
    WaitingFor(description);

    String response;
    was_successful = socket.GetLine(response);
    DoneWaiting();
    if (!was_successful) {
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
    was_successful = socket.PutLine(stock);

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

// SERVER:  Consider an invitation from a client.
/* static */ Game* Network::ConsiderInvitation(
    Socket& rSocket,
    GameOpt const& rGameOpt,
    HandOpts& rHandOpts)
{
    Address const client_address = rSocket.Peer();
    String const client_string = client_address;
    String question = client_string + " has invited you to play a game with:\n";
    question += rGameOpt.operator String();  // TODO better description
    question += rHandOpts.operator String();
    question += "Do you accept?";
    bool const accept = Question(question);

    Game* p_result = NULL;
    if (!accept) {
        rSocket.PutLine(DECLINE);
        rSocket.Close();
    } else {
        bool const was_successful = rSocket.PutLine(ACCEPT);
        if (was_successful) {
            HandOpts server_hand_opts = rHandOpts;
            Address const server_address = rSocket.Local();
            server_hand_opts.Serverize(client_address, server_address);
            p_result = Game::New(rGameOpt, server_hand_opts, rSocket);
        }
    }

    // The caller should check for NULL before calling p_result->Initialize().
    return p_result;
}

/* static */ String Network::DescribeListenPort(void) {
    String result = "network port " + msListenPort;

    return result;
}

/* static */ void Network::DoneWaiting(void) {
#ifdef _GUI
    ASSERT(mspWindow != NULL);
    mspWindow->DoneWaiting();
#elif defined(_CONSOLE)
    std::cout << " done." << std::endl;
#endif // defined(_CONSOLE)
}

#ifdef _WINSOCK2
/* static */ void Network::Fail(TextType operation) {
     int const error_code = Win::WSAGetLastError();
     std::cout << "Unexpected Winsock error code " << error_code 
                    << " on " << operation << "." << std::endl;
     FAIL();
}
#endif // defined(_WINSOCK2)

// CLIENT:  Invite a server to participate in the current game.
/* static */ bool Network::InviteServer(Socket& rSocket, Game const& rGame) {
    Address const peer = rSocket.Peer();
    String const peer_string = peer;
    std::cout << "Sending invitation to " << peer_string << std::endl;

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
        was_successful = rSocket.Put(hand_opt_string);
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
#elif defined(_CONSOLE)
    std::cout << rMessage << std::endl;
#endif // defined(_CONSOLE)
}

#ifdef _WINSOCK2
// SERVER:  Create and bind a listen socket.
/* static */ Socket Network::OpenListen(int family) {
    // Get address info for the listen socket.
    ADDRINFOA address_hints;
    ::ZeroMemory(&address_hints, sizeof(address_hints));
    address_hints.ai_family = family;
    address_hints.ai_socktype = SOCK_STREAM;
    address_hints.ai_protocol = IPPROTO_TCP;
    address_hints.ai_flags = AI_PASSIVE;

    PCSTR const any_client = NULL;
    PADDRINFOA address_list = NULL;
    int failure = Win::getaddrinfo(any_client, msListenPort,
        &address_hints, &address_list);
    ASSERT(failure == 0);

    SOCKET server_listen = INVALID_SOCKET;
    for (PADDRINFOA p_info = address_list; p_info != NULL; p_info = p_info->ai_next) {
        ASSERT(p_info->ai_socktype == SOCK_STREAM);
        ASSERT(p_info->ai_protocol == IPPROTO_TCP);
        if (p_info->ai_family == family) {
            // Create a listen socket.
            server_listen = Win::socket(family, p_info->ai_socktype, p_info->ai_protocol);
            ASSERT(server_listen != INVALID_SOCKET);

            // Attempt to bind to the socket.
            failure = Win::bind(server_listen, p_info->ai_addr, int(p_info->ai_addrlen));
            if (failure == 0) {
                // success
                break;
            }
            int const error_code = Win::WSAGetLastError();
            if (error_code == WSAEADDRINUSE) {
#ifdef _CONSOLE
                String const message = String("Network port ")
                    + msListenPort + " is busy on this computer.\n"
                    + "Perhaps there is another Gold Tile server running.\n";
                Notice(message);
#endif // defined(_CONSOLE)
                server_listen = INVALID_SOCKET;
                // try next addrinfo
            } else {
                Fail("bind");
            }
        }
    }

    Win::freeaddrinfo(address_list);

    Socket result;
    if (server_listen == INVALID_SOCKET) {
        return result;
    }

    // Put the (bound but unconnected) socket into listen mode.
    failure = Win::listen(server_listen, MAX_CONNECTION_CNT);
    ASSERT(failure == 0);

    result = Socket(Socket::HandleType(server_listen));

    // Put the listen socket into non-blocking mode.
    result.MakeNonblocking();

    return result;
}

// CLIENT:  Attempt to connect to a server.
/* static */ Socket Network::OpenServer(void* p, Address const& rServer) {
    PADDRINFOA const addrinfo_list = PADDRINFOA(p);

    SOCKET client_send = INVALID_SOCKET;
    for (PADDRINFOA p_info = addrinfo_list; p_info != NULL; p_info = p_info->ai_next) {
        // Create a send socket.
        ASSERT(p_info->ai_family == AF_INET || p_info->ai_family == AF_INET6);
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
                String const server_description = DescribeListenPort() 
                    + " on " + server;
                String const message = String("No response from ")
                    + server_description
                    + "\n\nPlease ensure that a Gold Tile server is running on " 
                    + server;
                Notice(message);
                client_send = INVALID_SOCKET;
                // try next addrinfo
        } else {
            Fail("connect");
        }
    } 

    Socket result;
    if (client_send != INVALID_SOCKET) {
        result = Socket(Socket::HandleType(client_send));
    }

    return result;
}
#endif // defined(_WINSOCK2)

// Return true if user chooses "yes", false otherwise.
/* static */ bool Network::Question(String const& rQuestion) {
#ifdef _GUI
    ASSERT(mspWindow != NULL);
    int const accept = mspWindow->QuestionBox(rQuestion, "Gold Tile Game - Network Question");
    bool const result = (accept == IDYES);

    return result;
#elif defined(_CONSOLE)
    std::cout << rQuestion << " (y/n) ";
    String yes_no;
    for (;;) {
        std::cin >> yes_no;
        yes_no.Capitalize();
        if (yes_no.HasPrefix("Y")) {
            break;
        } else if (yes_no.HasPrefix("N")) {
            return false;
        }
    }

    return true;
#endif // defined(_CONSOLE)
}

/* static */ bool Network::Retry(String const& rMessage) {
    bool result = true;

#ifdef _GUI
    ASSERT(mspWindow != NULL);
    result = mspWindow->RetryBox(rMessage, "Gold Tile Game - Network Retry");
#elif defined(_CONSOLE)
    std::cout << rMessage << std::endl;
    std::cout << "Press the Enter key to retry ..." << std::endl;
    String end_of_line;
    std::getline(std::cin, end_of_line);
    // The console version retries until something kills the application,
    // such as a local user typing Ctrl+C.
#endif // defined(_CONSOLE)

    return result;
}

#ifdef _GUI
/* static */ void Network::SetWindow(Window* pWindow) {
    mspWindow = pWindow;
}
#endif // defined(_GUI)

/* static */ bool Network::StartServer(void) {
#ifdef _QT

    ASSERT(mspServer == NULL);

    mspServer = new QTcpServer;
    ASSERT(mspServer != NULL);

    quint16 const port = long(msListenPort);
    bool const success = mspServer->listen(QHostAddress::Any, port);
    if (!success) {
        return false;
    }
    mspServer->setMaxPendingConnections(MAX_CONNECTION_CNT);

#elif defined(_WINSOCK2)

    ASSERT(!msListenIpv4.IsValid());
    ASSERT(!msListenIpv6.IsValid());

    msListenIpv4 = OpenListen(AF_INET);
    msListenIpv6 = OpenListen(AF_INET6);

    if (!msListenIpv4.IsValid() && !msListenIpv6.IsValid()) {
        return false;
    }
    
#endif // defined(_WINSOCK2)

    return true;
}

/* static */ void Network::WaitingFor(String const& rEventDescription) {
#ifdef _GUI
    ASSERT(mspWindow != NULL);
    mspWindow->WaitingFor(rEventDescription);
#elif defined(_CONSOLE)
    std::cout << "Waiting for " << rEventDescription << " ..." << std::endl;
#endif // defined(_CONSOLE)
}


// inquiry methods

/* static */ bool Network::IsServerStarted(void) {
#ifdef _WINSOCK2
    bool const result = (msListenIpv4.IsValid() || msListenIpv6.IsValid());
    return result;
#elif defined(_QT)
    return true;
#endif
}


#ifdef _NETWORK_TEST

// unit test

# ifdef _QT

#  include <QCoreApplication>
#  include <QSocketNotifier>
#  include <QThread>

class MyThread: public QThread {
public:
    void run();
};

void MyThread::run(void) {
    Network network;

    Game::ConsoleGame();
}

int main(int argCnt, char** argValues) {
    QCoreApplication application(argCnt, argValues);

    MyThread thread;
    thread.start();

    return application.exec();
}

# else // !defined(_QT)

int main(int argCnt, char** argValues) {
    argCnt;     // ununsed
    argValues;  // unused

    Network network;

    Game::ConsoleGame();
    return EXIT_SUCCESS;
}

# endif // !defined(_QT)

#endif // defined(_NETWORK_TEST)
