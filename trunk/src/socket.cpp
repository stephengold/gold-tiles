// File:     socket.cpp
// Location: src
// Purpose:  implement Socket class
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

#include <iostream>    // cout
#include "address.hpp"
#include "network.hpp"
#ifdef _WINSOCK2
# include "fifo.hpp"
# define WIN32_LEAN_AND_MEAN
# include "gui/win_types.hpp"
namespace Win {
# include <winsock2.h>
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
};
using Win::in_addr;
using Win::u_long;
using Win::PSOCKADDR_IN;
using Win::SOCKADDR;
using Win::SOCKET;
#endif // defined(_WINSOCK2)


// static data

void* Socket::mspYieldArgument = NULL;
Socket::YieldFunctionType* Socket::mspYieldFunction = NULL;


// lifecycle

// construct an invalid socket
Socket::Socket(void) {
#ifdef _WINSOCK2
    mpReadBuffer = NULL;
    Invalidate();
#elif defined(_QT)
    mpSocket = NULL;
#endif // defined(_QT)

    ASSERT(!IsValid());
}

#ifdef _WINSOCK2
// Construct a Socket based on a Winsock handle.
Socket::Socket(HandleType handle) {
    ASSERT(handle != HandleType(INVALID_SOCKET));

    mHandle = handle;
    mpReadBuffer = new Fifo;

    ASSERT(IsValid());
}
#elif defined(_QT)
// Construct a Socket out of a QTcpSocket pointer.
Socket::Socket(QTcpSocket* pSocket) {
    ASSERT(pSocket != NULL);

    mpSocket = pSocket;

    ASSERT(IsValid());
}
#endif // defined(_QT)

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment operator is fine.

#ifdef _WINSOCK2
Socket::operator Socket::HandleType(void) const {
    return mHandle;
}
#endif // defined(_WINSOCK2)


// misc methods

void Socket::Close(void) {
    ASSERT(IsValid());

#ifdef _QT
    mpSocket->close();
#elif defined(_WINSOCK2)
    SOCKET const socket = SOCKET(mHandle);
    int failure = Win::closesocket(socket);
    ASSERT(failure == 0);
#endif // defined(_WINSOCK2)

    Invalidate();

    ASSERT(!IsValid());
}

// Return true if successful, false if canceled.
bool Socket::GetCharacter(char& rCharacter) {
    ASSERT(IsValid());

#ifdef _QT
    while (mpSocket->bytesAvailable() < 1) {
        mpSocket->waitForReadyRead(100);
    }
    bool const was_successful = mpSocket->getChar(&rCharacter);
    ASSERT(was_successful);
#elif defined(_WINSOCK2)
    bool was_successful = true;
    if (!HasBufferedData()) {
        was_successful = Read();
    }
    if (was_successful) {
        rCharacter = mpReadBuffer->GetByte();
    }
#endif // defined(_WINSOCK2)

    return was_successful;
}

// Return true if successful, false if canceled.
bool Socket::GetLine(String& rString) {
    ASSERT(IsValid());

    rString.MakeEmpty();
    bool was_successful = true;
    for (;;) {
        char character;
        was_successful = GetCharacter(character);
        if (!was_successful || character == '\n') { // canceled or end of line
            break;
        }
        rString += character;
    }

    return was_successful;
}

// Return true if successful, false if canceled.
bool Socket::GetParagraph(String& rString) {
    ASSERT(IsValid());

    rString.MakeEmpty();
    bool was_successful = true;
    for (;;) {
        String line;
        was_successful = GetLine(line);
        if (!was_successful || line.IsEmpty()) { // canceled or blank line
            break;
        }
        rString += line + "\n";
    }

    return was_successful;
}

void Socket::Invalidate(void) {
    // TODO: close? free buffer?
#ifdef _QT
    mpSocket = NULL;
#elif defined(_WINSOCK2)
    mHandle = HandleType(INVALID_SOCKET);
#endif // defined(_WINSOCK2)

    ASSERT(!IsValid());
}

// Get the address of socket on the local host.
Address Socket::Local(void) const {
    ASSERT(IsValid());

#ifdef _QT
    QHostAddress const address = mpSocket->localAddress();
#elif defined(_WINSOCK2)
    SOCKET const socket = SOCKET(mHandle);
    SOCKADDR sockaddr;
    int length = sizeof(sockaddr);
    int const failure = Win::getsockname(socket, &sockaddr, &length);
    ASSERT(failure == 0);
    ASSERT(sockaddr.sa_family == AF_INET);

    PSOCKADDR_IN const s = PSOCKADDR_IN(&sockaddr);
    in_addr const in = s->sin_addr;
    TextType const address = Win::inet_ntoa(in);
#endif // defined(_WINSOCK2)

    Address const result(address);

    return result;
}

void Socket::MakeNonblocking(void) {
    ASSERT(IsValid());

#ifdef _WINSOCK2
    SOCKET const socket = SOCKET(mHandle);
    u_long non_blocking = 1;
    int const failure = Win::ioctlsocket(socket, FIONBIO, &non_blocking);
    ASSERT(failure == 0);
#endif // defined(_WINSOCK2)
}

// Get the address of the peer (host) to which the socket connects.
Address Socket::Peer(void) const {
    ASSERT(IsValid());

#ifdef _QT
    ASSERT(mpSocket->state() == QAbstractSocket::ConnectedState);
    QHostAddress const address = mpSocket->peerAddress();
    ASSERT(address != QHostAddress::Null);
#elif defined(_WINSOCK2)
    SOCKET const socket = SOCKET(mHandle);
    SOCKADDR sockaddr;
    int length = sizeof(sockaddr);
    int const failure = Win::getpeername(socket, &sockaddr, &length);
    ASSERT(failure == 0);
    ASSERT(sockaddr.sa_family == AF_INET);

    PSOCKADDR_IN const s = PSOCKADDR_IN(&sockaddr);
    in_addr const in = s->sin_addr;
    TextType const address =  Win::inet_ntoa(in);
#endif // defined(_WINSOCK2)

    Address const result(address);

    return result;
}

bool Socket::Put(String const& rString) {
    ASSERT(IsValid());

#ifdef _QT
    TextType const data = rString;
    qint64 bytes_sent = mpSocket->write(data);
    if (bytes_sent == -1) {
        FAIL();
    }
#elif defined(_WINSOCK2)
    SOCKET const socket = SOCKET(mHandle);
    char const* const p_buffer = TextType(rString);
    int const buffer_size = rString.Length();
    int const no_flags = 0;
    int const bytes_sent = Win::send(socket, p_buffer, buffer_size, no_flags);
    if (bytes_sent == SOCKET_ERROR) {
        int const error_code = Win::WSAGetLastError();
        if (error_code == WSAECONNRESET) {
            String const peer(Peer());
            String const error_message = String("Lost network connection to ")
                + peer + " discovered while sending data.";
            Network::Notice(error_message);
            return false;
        }
        FAIL();
    }
    ASSERT(bytes_sent == buffer_size);
#endif // defined(_WINSOCK2)

    ASSERT(IsValid());
    return true;
}

bool Socket::PutLine(String const& rLine) {
    String const string = rLine + "\n";
    bool const was_successful = Put(string);

    return was_successful;
}

#ifdef _WINSOCK2
// Read some more network data into the FIFO buffer.
// Return true if successful, false if canceled.
bool Socket::Read(void) {
    ASSERT(IsValid());

    SOCKET const socket = SOCKET(mHandle);

    char* p_start = NULL;
    int bytes_requested = 0;
    mpReadBuffer->PreReceive(p_start, bytes_requested);
    ASSERT(bytes_requested > 0);

    bool canceled = false;
    int bytes_received = 0;
    while (!canceled) {
        int const flags = 0;
        bytes_received = Win::recv(socket, p_start, bytes_requested, flags);
        if (bytes_received > 0) {
            break;
        } 
        int const error_code = Win::WSAGetLastError();        
        if (bytes_received == 0 
            || error_code == WSAECONNRESET 
            || error_code == WSAECONNABORTED)
        {
            String const peer(Peer());
            String const error_message = String("Lost network connection to ")
                + peer + " discovered while receiving data.";
            Network::Notice(error_message);
            canceled = true;
            continue;
        } else if (error_code == WSAEWOULDBLOCK) {
            Yields(canceled);
            continue;
        } else {
            std::cout << "Winsock error code " << error_code << " on recv." << std::endl;
            FAIL();
        }
    }

    bool const was_successful = !canceled;
    if (was_successful) {
        ASSERT(bytes_received > 0);
        mpReadBuffer->PostReceive(bytes_received);
        ASSERT(HasBufferedData());
        ASSERT(IsValid());
    } else {
        Invalidate();
    }

    return was_successful;
}
#endif // defined(_WINSOCK2)

// Set up a callback to be invoked when waiting for data from the network.
/* static */ void Socket::SetYield(
    YieldFunctionType* pFunction,
    void* pArgument)
{
    mspYieldArgument = pArgument;
    mspYieldFunction = pFunction;
}

/* static */ void Socket::Yields(bool& rCanceled) {
    if (mspYieldFunction != NULL) {
        (*mspYieldFunction)(mspYieldArgument, rCanceled);
    }
}


// inquiry methods

bool Socket::HasBufferedData(void) const {
    ASSERT(IsValid());

#ifdef _QT
    qint64 const byte_cnt = mpSocket->bytesAvailable();
    bool const result = (byte_cnt > 0);
#elif defined(_WINSOCK2)
    bool const result = mpReadBuffer->HasData();
#endif // defined(_WINSOCK2)

    return result;
}

bool Socket::IsValid(void) const {
#ifdef _QT
    bool const result = (mpSocket != NULL);
#elif defined(_WINSOCK2)
    bool const result = (mHandle != HandleType(INVALID_SOCKET)
        && mpReadBuffer != NULL
        && mpReadBuffer->IsValid());
#endif // defined(_WINSOCK2)

    return result;
}
