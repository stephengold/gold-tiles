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
#include "socket.hpp"

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


// lifecycle

// construct an invalid socket
Socket::Socket(void) {
	mHandle = HandleType(INVALID_SOCKET);
	mpReadBuffer = new Fifo;

	ASSERT(mpReadBuffer != NULL);
}

// construct a server-side socket based on a Winsock handle
Socket::Socket(HandleType handle) {
	mHandle = handle;
	mpReadBuffer = new Fifo;

	ASSERT(mpReadBuffer != NULL);
}

// The compiler-generated copy constructor is fine.

Socket::~Socket(void) {
	// TODO cleanup
}


// operators

// The compiler-generated assignment operator is fine.

Socket::operator Socket::HandleType(void) const {
	return mHandle;
}


// misc methods

char Socket::GetCharacter(void) {
	ASSERT(IsValid());

	if (!HasBufferedData()) {
		Read();
	}
	char const result = mpReadBuffer->GetByte();

	return result;
}

String Socket::GetLine(void) {
	ASSERT(IsValid());

	String result;
	for (;;) {
	    char const ch = GetCharacter();
	    if (ch == '\n') { // end of line
		    break;
		}
		result += ch;
	}

	ASSERT(IsValid());
	return result;
}

String Socket::GetParagraph(void) {
	ASSERT(IsValid());

	String result;
	for (;;) {
	    String const line = GetLine();
	    if (line.IsEmpty()) {
		    break;
		}
		result += line + "\n";
	}

	ASSERT(IsValid());
	return result;
}

// Get the address of socket on the local host.
Address Socket::Local(void) const {
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

void Socket::Put(String const& rString) {
	ASSERT(IsValid());

	SOCKET const socket = SOCKET(mHandle);

	char const* const p_buffer = TextType(rString);
	int const buffer_size = rString.Length();
	int const flags = 0;
    int const bytes_sent = Win::send(socket, p_buffer, buffer_size, flags);
	ASSERT(bytes_sent == buffer_size);

	ASSERT(IsValid());
}

void Socket::PutLine(String const& rLine) {
	String const string = rLine + "\n";
	Put(string);
}

// Refill the buffer.
void Socket::Read(void) {
	ASSERT(IsValid());
	ASSERT(HasBufferSpace());

	SOCKET const socket = SOCKET(mHandle);

	char* p_start = NULL;
	int bytes_requested = 0;
	mpReadBuffer->PreReceive(p_start, bytes_requested);

	int const flags = 0;
	int const bytes_received = Win::recv(socket, p_start, bytes_requested, flags);
	if (bytes_received < 0) {
		int const error_code = Win::WSAGetLastError();
		if (error_code == WSAECONNRESET) {
		    std::cout << "A network connection broke while data was expected." << std::endl;
		} else {
		    std::cout << "Winsock error code " << error_code << " on recv." << std::endl;
		}
		FAIL();
	}

	mpReadBuffer->PostReceive(bytes_received);

	ASSERT(IsValid());
	ASSERT(HasBufferedData());
}


// inquiry methods

bool Socket::HasBufferedData(void) const {
	bool const result = mpReadBuffer->HasData();

	return result;
}

bool Socket::HasBufferSpace(void) const {
	bool const result = mpReadBuffer->HasSpace();

	return result;
}

bool Socket::IsValid(void) const {
	bool const result = (mpReadBuffer != NULL
		              && mpReadBuffer->IsValid() 
		              && mHandle != HandleType(INVALID_SOCKET));

	return result;
}
