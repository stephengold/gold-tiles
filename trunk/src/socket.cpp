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
# define WIN32_LEAN_AND_MEAN
# include "gui/win_types.hpp"
namespace Win {
# include <winsock2.h>
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
};
using Win::in_addr;
using Win::PSOCKADDR_IN;
using Win::SOCKADDR;
using Win::SOCKET;
#endif // defined(_WINSOCK2)


// static data

void*                      Socket::mspYieldArgument = NULL;
Socket::YieldFunctionType* Socket::mspYieldFunction = NULL;


// lifecycle

// construct an invalid socket
Socket::Socket(void) {
	mpReadBuffer = NULL;
    Invalidate();

    ASSERT(!IsValid());
}

// construct a server-side socket based on a Winsock handle
Socket::Socket(HandleType handle) {
    ASSERT(handle != HandleType(INVALID_SOCKET));

	mHandle = handle;
	mpReadBuffer = new Fifo;

	ASSERT(IsValid());
}

// The compiler-generated copy constructor is fine.
// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment operator is fine.

Socket::operator Socket::HandleType(void) const {
	return mHandle;
}


// misc methods

void Socket::Close(void) {
    ASSERT(IsValid());

	SOCKET const socket = SOCKET(mHandle);
    int failure = Win::closesocket(socket);
    ASSERT(failure == 0);

    Invalidate();

    ASSERT(!IsValid());
}

// Return true if successful, false if canceled.
bool Socket::GetCharacter(char& rCharacter) {
	ASSERT(IsValid());

    bool was_successful = true;
	if (!HasBufferedData()) {
		was_successful = Read();
	}
    if (was_successful) {
	    rCharacter = mpReadBuffer->GetByte();
    }

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

	mHandle = HandleType(INVALID_SOCKET);
    ASSERT(!IsValid());
}

// Get the address of socket on the local host.
Address Socket::Local(void) const {
	ASSERT(IsValid());

	SOCKET const socket = SOCKET(mHandle);
	SOCKADDR sockaddr;
	int length = sizeof(sockaddr);
	int const failure = Win::getsockname(socket, &sockaddr, &length);
    ASSERT(failure == 0);
	ASSERT(sockaddr.sa_family == AF_INET);
	
    PSOCKADDR_IN const s = PSOCKADDR_IN(&sockaddr);
	in_addr const in = s->sin_addr;
	TextType address =  Win::inet_ntoa(in);

	Address const result(address);

	return result;
}

// Get the address of the peer to which the socket is connected.
Address Socket::Peer(void) const {
	ASSERT(IsValid());

	SOCKET const socket = SOCKET(mHandle);
	SOCKADDR sockaddr;
	int length = sizeof(sockaddr);
	int const failure = Win::getpeername(socket, &sockaddr, &length);
    ASSERT(failure == 0);
	ASSERT(sockaddr.sa_family == AF_INET);
	
    PSOCKADDR_IN const s = PSOCKADDR_IN(&sockaddr);
	in_addr const in = s->sin_addr;
	TextType address =  Win::inet_ntoa(in);

	Address const result(address);

	return result;
}

bool Socket::Put(String const& rString) {
	ASSERT(IsValid());

	SOCKET const socket = SOCKET(mHandle);
	char const* const p_buffer = TextType(rString);
	int const buffer_size = rString.Length();
	int const no_flags = 0;
    int const bytes_sent = Win::send(socket, p_buffer, buffer_size, no_flags);
    if (bytes_sent == SOCKET_ERROR) {
	    int const error_code = Win::WSAGetLastError();
        if (error_code == WSAECONNRESET) {
            String const peer(Peer());
            String const error_message = String("Lost network connection")
                + " while sending to " + peer;
            Network::Notice(error_message);
            return false;
        }
        FAIL();
    }

	ASSERT(bytes_sent == buffer_size);
	ASSERT(IsValid());
    return true;
}

bool Socket::PutLine(String const& rLine) {
	String const string = rLine + "\n";
	bool const was_successful = Put(string);

    return was_successful;
}

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
            String const error_message = String("Lost network connection")
                + " -- expected more data from " + peer;
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
	bool const result = mpReadBuffer->HasData();

	return result;
}

bool Socket::IsValid(void) const {
	bool const result = (mHandle != HandleType(INVALID_SOCKET)
                      && mpReadBuffer != NULL
		              && mpReadBuffer->IsValid());

	return result;
}
