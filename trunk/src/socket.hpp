#ifndef SOCKET_HPP_INCLUDED
#define SOCKET_HPP_INCLUDED

// File:     socket.hpp
// Location: src
// Purpose:  declare Socket class
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
A Socket object represents a network connection between a client and
a server which is capable of passing String objects back and forth
using "get" and "put" operations.

The Socket class is encapsulates a handle (Winsock socket) and a Fifo
for incoming data.
*/

#include "fifo.hpp"   // HASA String
#include "string.hpp" // USES String


class Socket {
public:
	// public types
	typedef void* HandleType;

	// public lifecycle
	Socket(void);
	explicit Socket(HandleType);
    // Socket(Socket const&);  compiler-generated copy constructor is OK
    ~Socket(void);

	// public operators
    // Socket& operator=(Socket const&);  compiler-generated assigment operator is OK
	operator HandleType(void) const;

	// misc public methods
	char    GetCharacter(void);
	String  GetLine(void);
	String  GetParagraph(void);
	Address Local(void) const;
	Address Peer(void) const;
	void    Put(String const&);
	void    PutLine(String const&);
	void    Read(void);

	// public inquiry methods
	bool HasBufferedData(void) const;
	bool HasBufferSpace(void) const;
	bool IsValid(void) const;

private:
    // private data
	HandleType mHandle;
	Fifo*     mpReadBuffer;
};
#endif // !defined(SOCKET_HPP_INCLUDED)
