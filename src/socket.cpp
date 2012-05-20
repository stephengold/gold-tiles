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

#include <iostream>       // cout
#include "network.hpp"

#ifdef _POSIX
namespace Posix {
# include <errno.h>       // EWOULDBLOCK
# include <netdb.h>
# include <sys/ioctl.h>   // FIONBIO
};
# include "fifo.hpp"
using Posix::getsockname;
using Posix::recv;
using Posix::send;
using Posix::sockaddr;
using Posix::u_long;
typedef sockaddr SOCKADDR;
typedef int SOCKET;
SOCKET const INVALID_SOCKET = -1;
int const SOCKET_ERROR = -1;
int const WSAECONNABORTED = ECONNABORTED;
int const WSAECONNRESET = ECONNRESET;
int const WSAEWOULDBLOCK = EWOULDBLOCK;
# define closesocket close
# define ioctlsocket Posix::ioctl

#elif defined(_WINSOCK2)
# define WIN32_LEAN_AND_MEAN  // to avoid macro redefinitions
# include "gui/win_types.hpp"
namespace Win {
# include <winsock2.h>
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
};
# include "fifo.hpp"
using Win::closesocket;
using Win::getpeername;
using Win::getsockname;
using Win::in_addr;
using Win::ioctlsocket;
using Win::recv;
using Win::send;
using Win::u_long;
using Win::PSOCKADDR_IN;
using Win::SOCKADDR;
using Win::SOCKET;
typedef int socklen_t;
#endif  // defined(_WINSOCK2)


// static data

void* Socket::mspYieldArgument = NULL;
Socket::YieldFunctionType* Socket::mspYieldFunction = NULL;


// lifecycle

// construct an invalid socket
Socket::Socket(void) {
#ifdef _QT
    mpSocket = NULL;
#else  // !defined(_QT)
    mpReadBuffer = NULL;
    Invalidate();
#endif  // !defined(_QT)

    ASSERT(!IsValid());
}

#ifdef _QT
// Construct a Socket out of a QTcpSocket pointer.
Socket::Socket(QTcpSocket* pSocket) {
    ASSERT(pSocket != NULL);

    mpSocket = pSocket;

    ASSERT(IsValid());
}
#else  // !defined(_QT)
// Construct a Socket based on a Winsock handle.
Socket::Socket(HandleType handle) {
    ASSERT(handle != HandleType(INVALID_SOCKET));

    mHandle = handle;
    mpReadBuffer = new Fifo;

    ASSERT(IsValid());
}
#endif  // !defined(_QT)

// The implicitly defined copy constructor is fine.
// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment operator is fine.

#ifndef _QT
Socket::operator Socket::HandleType(void) const {
    return mHandle;
}
#endif  // !defined(_QT)


// misc methods

void Socket::Close(void) {
    ASSERT(IsValid());

#ifdef _QT
    mpSocket->close();
#else   // !defined(_QT)
    SOCKET const socket = SOCKET(mHandle);
    int failure = closesocket(socket);
    ASSERT(failure == 0);
#endif  // defined(_QT)

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
#else   // !defined(_QT)
    bool was_successful = true;
    if (!HasBufferedData()) {
        was_successful = Read();
    }
    if (was_successful) {
        rCharacter = mpReadBuffer->GetByte();
    }
#endif  // !defined(_QT)

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
        if (!was_successful || character == '\n') {
            // canceled or end of line
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
#else  // !defined(_QT)
    mHandle = HandleType(INVALID_SOCKET);
#endif  // !defined(_QT)

    ASSERT(!IsValid());
}

// Get the local address associated with the socket.
Address Socket::Local(void) const {
    ASSERT(IsValid());

#ifdef _QT
    QHostAddress const address = mpSocket->localAddress();
    Address const result(address);
#else  // !defined(_QT)
    SOCKET const socket = SOCKET(mHandle);
    SOCKADDR sockaddr[2];  // a single SOCKADDR isn't sufficient??
    socklen_t length = sizeof(sockaddr);
    int const failure = getsockname(socket, sockaddr, &length);
    ASSERT(failure == 0);
    Address const result(sockaddr[0]);
#endif  // !defined(_QT)

    return result;
}

void Socket::MakeNonblocking(void) {
    ASSERT(IsValid());

#ifndef _QT
    SOCKET const socket = SOCKET(mHandle);
    u_long non_blocking = 1;
    int const failure = ioctlsocket(socket, FIONBIO, &non_blocking);
    ASSERT(failure == 0);
#endif  // !defined(_QT)
}

// Get the address of the peer (host) to which the socket connects.
Address Socket::Peer(void) const {
    ASSERT(IsValid());

#ifdef _QT
    ASSERT(mpSocket->state() == QAbstractSocket::ConnectedState);
    QHostAddress const address = mpSocket->peerAddress();
    ASSERT(address != QHostAddress::Null);
    Address const result(address);
#else  // !defined(_QT)
    SOCKET const socket = SOCKET(mHandle);
    SOCKADDR sockaddr[2]; // a single SOCKADDR isn't sufficient??
    socklen_t length = sizeof(sockaddr);
    int const failure = getpeername(socket, sockaddr, &length);
    ASSERT(failure == 0);
    Address const result(sockaddr[0]);
#endif  // !defined(_QT)

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
#else  // !defined(_QT)
    SOCKET const socket = SOCKET(mHandle);
    char const* const p_buffer = TextType(rString);
    SizeType const buffer_size = rString.Length();
    int const no_flags = 0;
    int const bytes_sent = send(socket, p_buffer, buffer_size, no_flags);
    if (bytes_sent == SOCKET_ERROR) {
        int const error_code = Network::ErrorCode();
        if (error_code == WSAECONNRESET) {
            Address const peer_address = Peer();
            String const peer = peer_address;
            String const error_message = String("Lost network connection to ")
                + peer + " -- discovered while sending data.";
            Network::Notice(error_message);
            return false;
        } else {
            Network::Fail("send");
        }
    }
    ASSERT(SizeType(bytes_sent) == buffer_size);
#endif  // !defined(_QT)

    ASSERT(IsValid());
    return true;
}

bool Socket::PutLine(String const& rLine) {
    String const string = rLine + "\n";
    bool const was_successful = Put(string);

    return was_successful;
}

#ifndef _QT
// Read some more network data into the FIFO buffer.
// Return true if successful, false if canceled.
bool Socket::Read(void) {
    ASSERT(IsValid());

    SOCKET const socket = SOCKET(mHandle);

    char* p_start = NULL;
    SizeType bytes_requested = 0;
    mpReadBuffer->PreReceive(p_start, bytes_requested);
    ASSERT(bytes_requested > 0);

    bool canceled = false;
    int bytes_received = 0;
    while (!canceled) {
        int const flags = 0;
        bytes_received = recv(socket, p_start, bytes_requested, flags);
        if (bytes_received > 0) {
            break;
        } 
        int const error_code = Network::ErrorCode();
        if (bytes_received == 0 
            || error_code == WSAECONNABORTED
            || error_code == WSAECONNRESET)
        {
            Address const peer_address = Peer();
            String const peer = peer_address;
            String const error_message = String("Lost network connection to ")
                + peer + " -- discovered while receiving data.";
            Network::Notice(error_message);
            canceled = true;
            continue;
        } else if (error_code == WSAEWOULDBLOCK) {
            Yields(canceled);
            continue;
        } else {
            Network::Fail("receive");
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
#endif  // !defined(_QT)

// Set up a callback to be invoked when waiting for data from the network.
/* static */  void Socket::SetYield(
    YieldFunctionType* pFunction,
    void* pArgument)
{
    mspYieldArgument = pArgument;
    mspYieldFunction = pFunction;
}

/* static */  void Socket::Yields(bool& rCanceled) {
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
#else  // !defined(_QT)
    bool const result = mpReadBuffer->HasData();
#endif  // !defined(_QT)

    return result;
}

bool Socket::IsValid(void) const {
#ifdef _QT
    bool const result = (mpSocket != NULL);
#else   // !defined(_QT)
    bool const result = (mHandle != HandleType(INVALID_SOCKET)
        && mpReadBuffer != NULL
        && mpReadBuffer->IsValid());
#endif  // !defined(_QT)

    return result;
}
