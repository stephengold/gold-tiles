// File:     address.cpp
// Location: src
// Purpose:  implement Address class
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

#include "address.hpp"
#include "strings.hpp"
#ifdef _QT
# include <QNetworkInterface>
#elif defined(_WINSOCK2)
# define WIN32_LEAN_AND_MEAN // to avoid macro redefinitions
# include "gui/win_types.hpp"
namespace Win {
# include <ws2tcpip.h> // IPv6 support, including inet_pton
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
# include <iphlpapi.h> // IP help API
# pragma comment(lib, "iphlpapi.lib")  // link with iphlpapi.lib
};
using Win::ADDRESS_FAMILY;
using Win::IN_ADDR;
using Win::IN6_ADDR;
using Win::PIN_ADDR;
using Win::PIN6_ADDR;
using Win::PIP_ADAPTER_ADDRESSES;
using Win::PIP_ADAPTER_UNICAST_ADDRESS;
using Win::PSOCKADDR;
using Win::PSOCKADDR_IN;
using Win::PSOCKADDR_IN6;
#endif // defined(_WINSOCK2)


// static constants

const String Address::DEFAULT("172.27.100.25");
const String Address::LOCALHOST_IPV4("127.0.0.1");
const String Address::LOCALHOST_IPV6("::1");


// lifecycle

Address::Address(void):
#ifdef _WINSOCK2
    mString(DEFAULT)
#elif defined(_QT)
    QHostAddress(QString(DEFAULT))
#endif // defined(_QT)
{
}

// The implicitly defined copy constructor is fine.

Address::Address(String const &rString)
#ifdef _QT
:QHostAddress(QString(rString))
#endif // defined(_QT)
{
#ifdef _WINSOCK2
    if (rString.Contains(':')) {  // Assume it's an IPv6 address.
        IN6_ADDR ipv6_address;
        INT const success = Win::inet_pton(AF_INET6, rString, &ipv6_address);
        if (success != 1) {
            mString.MakeEmpty();
        } else {
            char buffer[64];
            TextType const text = Win::InetNtop(AF_INET6, &ipv6_address, buffer, sizeof(buffer));
            mString = String(text);
        }
    } else { // Assume it's an IPv4 address.
        unsigned long const address = Win::inet_addr(rString);
        if (address == INADDR_NONE || address == 0) {
            mString.MakeEmpty();
        } else {
            IN_ADDR ipv4_address;
            ipv4_address.S_un.S_addr = address;
            TextType const text = Win::inet_ntoa(ipv4_address);
            mString = String(text);
        }
    }
#endif
}

// Construct based on an IPv4 address in network (bigendian) byte order.
Address::Address(unsigned long address)
#ifdef _QT
:QHostAddress(quint32(address))
#endif // defined(_QT)
{
#ifdef _WINSOCK2
    IN_ADDR ipv4_address;
    ipv4_address.S_un.S_addr = address;
    TextType const text = Win::inet_ntoa(ipv4_address);
    mString = String(text);
#endif // defined(_WINSOCK2)
}

#ifdef _WINSOCK2

// Construct based on a pointer to a SOCKADDR structure.
Address::Address(void* p) {
    ASSERT(p != NULL);
    PSOCKADDR const p_address = PSOCKADDR(p);
    ADDRESS_FAMILY const family = p_address->sa_family;

    TextType text = NULL;
    char buffer[64];
    if (family == AF_INET6) {
        PSOCKADDR_IN6 const p_sockaddr_v6 = PSOCKADDR_IN6(p_address);
        PIN6_ADDR const p_ipv6 = &(p_sockaddr_v6->sin6_addr);
        text = Win::InetNtop(family, p_ipv6, buffer, sizeof(buffer));
    } else if (family == AF_INET) {
        PSOCKADDR_IN const p_sockaddr_v4 = PSOCKADDR_IN(p_address);  
        PIN_ADDR const p_ipv4 = &(p_sockaddr_v4->sin_addr);
        text = Win::InetNtop(family, p_ipv4, buffer, sizeof(buffer));
    } else {
        FAIL();
    }
    mString = String(text);
}

#elif defined(_QT)

// Construct from a QHostAddress object.
Address::Address(QHostAddress const& rAddress):
    QHostAddress(rAddress)
{
}

#endif // defined(_QT)

// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment operator is fine.

Address::operator String(void) const {
#ifdef _QT
    return String(toString());
#elif defined(_WINSOCK2)
    return mString;
#endif // defined(_WINSOCK2)
}

Address::operator unsigned long(void) const {
#ifdef _QT
    unsigned long const result = (unsigned long)(toIPv4Address());
#elif defined(_WINSOCK2)
    unsigned long const result = Win::inet_addr(mString);
#endif // defined(_WINSOCK2)

    return result;
}


// misc methods

// List all usable IPv4/IPv6 addresses, excluding localhost.
/* static */ Strings Address::ListAll(void) {
    Strings result;

#ifdef _QT

    QList<QHostAddress> const list = QNetworkInterface::allAddresses();
    QList<QHostAddress>::const_iterator i_address;
    for (i_address = list.constBegin(); i_address != list.constEnd(); i_address++) {
        QHostAddress const q_address = *i_address;
        Address const address(q_address);
        if (!address.IsLocalHost()) {
            // skip localhost
            String const address_text = address;
            result.Append(address_text);
        }
    }

#elif defined(_WINSOCK2)

    ULONG const flags = (GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST);
    PIP_ADAPTER_ADDRESSES p_table = NULL;
    ULONG table_bytes = 16000;  // initial size estimate
    while (p_table == NULL) {
         p_table = PIP_ADAPTER_ADDRESSES(new char[table_bytes]);
         ASSERT(p_table != NULL);

         ULONG const error_code = Win::GetAdaptersAddresses(AF_UNSPEC, flags, NULL, 
             p_table, &table_bytes);
         if (error_code != NO_ERROR) {
             if (error_code == ERROR_NO_DATA) {
                 return result;
             }
             ASSERT(error_code == ERROR_BUFFER_OVERFLOW);
             delete p_table;
             p_table = NULL;
         }
    }

    // Go through the list and append usable IP addresses to the result.
    PIP_ADAPTER_ADDRESSES p_current = p_table;

    while (p_current != NULL) {
        // Only interested in unicast addresses.
        PIP_ADAPTER_UNICAST_ADDRESS p_unicast = p_current->FirstUnicastAddress;
        while (p_unicast != NULL) {
            if ((p_unicast->Flags & IP_ADAPTER_ADDRESS_TRANSIENT) == 0x0) {
                // not transient
                Win::PSOCKADDR const p_address = p_unicast->Address.lpSockaddr;
                Address const address(p_address);
                if (!address.IsLocalHost()) {
                    // not localhost
                    String const address_text = address;
                    result.Append(address_text);
                }
            }
            p_unicast = p_unicast->Next;
        }
        p_current = p_current->Next;
    }

    delete p_table;

#endif // defined(_WINSOCK2)

    return result;
}


// inquiry methods

bool Address::IsLocalHost(void) const {
    bool const result = (mString == LOCALHOST_IPV4 
                      || mString == LOCALHOST_IPV6);

    return result;
}

bool Address::IsValid(void) const {
    bool const result = !mString.IsEmpty();

    return result;
}
