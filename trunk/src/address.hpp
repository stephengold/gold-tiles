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

The Address class encapsulates a String.
Currently, only IPv4 addresses are supported.
*/

#include "project.hpp" // _WINSOCK2
#include "string.hpp"  // HASA String
#ifdef _QT
# include <QHostAddress>  // ISA QHostAddress
#endif // defined(_QT)


class Address
#ifdef _QT
: public QHostAddress
#endif // defined(_QT)
{
public:
    // public lifecycle
    Address(void);
    // Address(Address const&);  implicitly defined copy constructor
    explicit Address(String const&);
    explicit Address(unsigned long);
#ifdef _QT
    explicit Address(QHostAddress const&);
#endif // defined(_QT)
    // ~Address(void);  implicitly defined destructor

    // public operators
    // Address &operator=(Address const&);  implicitly defined assignment operator
    operator String(void) const;
    operator unsigned long(void) const;

    // misc public methods
    static Strings ListAll(void);

private:
    // private types
    typedef unsigned char NetType;

    // private constants
    static const NetType LOCALHOST_NET = 127;
    static const String DEFAULT;

    // private data
    String mString;

    // misc private methods
    NetType Net(void) const;

    // private inquiry methods
    bool IsLocalHost(void) const;
};
#endif // !defined(ADDRESS_HPP_INCLUDED)
