#ifndef ADDRESS_HPP_INCLUDED
#define ADDRESS_HPP_INCLUDED

// File:     address.hpp
// Location: src
// Purpose:  declare Address class
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
An Address object represents a network address.

The Address class encapsulates a String.  Both IPv4 and IPv6
addresses are supported.
*/

#ifdef _QT
# include <QHostAddress>  // ISA QHostAddress
#endif  // defined(_QT)
#include "project.hpp"    // _WINSOCK2
#include "string.hpp"     // HASA String

#ifdef _POSIX
namespace Posix {
    struct sockaddr;
}
using Posix::sockaddr;

#elif defined(_WINSOCK2)
namespace Win {
    struct sockaddr;
}
using Win::sockaddr;
#endif  // defined(_WINSOCK2)

class Address
#ifdef _QT
:   public QHostAddress
#endif  // defined(_QT)
{
public:
    // public lifecycle
    Address(void);
    // Address(Address const&);  implicitly defined copy constructor
    explicit Address(String const&);
    explicit Address(uint32_t);  // IPv4 only
#ifdef _QT
    explicit Address(QHostAddress const&);
#else  // !defined(_QT)
    explicit Address(sockaddr const&);
#endif  // !defined(_QT)
    // ~Address(void);  implicitly defined destructor

    // public operators
    // Address &operator=(Address const&);  implicitly defined assignment operator
    operator String(void) const;
    operator uint32_t(void) const;

    // misc public methods
    static Strings ListAll(void);

    // misc inquiry methods
    bool IsValid(void) const;

private:
    // private constants
    static const String DEFAULT;
#ifndef _QT
    static const String LOCALHOST_IPV4;
    static const String LOCALHOST_IPV6;

    // private data
    String mString;
#endif  // !defined(_QT)

    // private inquiry methods
    bool IsLocalHost(void) const;
};
#endif  // !defined(ADDRESS_HPP_INCLUDED)
