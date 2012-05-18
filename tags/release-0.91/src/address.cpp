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

#ifdef _POSIX
namespace Posix {
# include <arpa/inet.h>  // struct in_addr
# include <ifaddrs.h>    // ifaddrs()
};
# include <net/if.h>     // IFF_UP

using Posix::freeifaddrs;
using Posix::getifaddrs;
using Posix::ifaddrs;
using Posix::sockaddr;

typedef Posix::sa_family_t ADDRESS_FAMILY;
typedef Posix::in_addr IN_ADDR, *PIN_ADDR;
typedef Posix::in6_addr IN6_ADDR, *PIN6_ADDR;
typedef sockaddr *PSOCKADDR;
typedef Posix::sockaddr_in *PSOCKADDR_IN;
typedef Posix::sockaddr_in6 *PSOCKADDR_IN6;
typedef int INT;
# define InetNtop inet_ntop

#elif defined(_QT)
# include <QNetworkInterface>

#elif defined(_WINSOCK2)
# define WIN32_LEAN_AND_MEAN  // to avoid macro redefinitions
# include "gui/win_types.hpp"
namespace Win {
# include <ws2tcpip.h>  // IPv6 support, including inet_pton
# pragma comment(lib, "ws2_32.lib")  // link with ws2_32.lib
# include <iphlpapi.h>  // IP help API
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
using Win::InetNtop;
using Win::inet_pton;

#endif  // defined(_WINSOCK2)


// static constants

const String Address::DEFAULT("172.27.100.25");
#ifndef _QT
const String Address::LOCALHOST_IPV4("127.0.0.1");
const String Address::LOCALHOST_IPV6("::1");
#endif  // defined(_WINSOCK2)


// lifecycle

Address::Address(void)
    :
#ifdef _QT
    QHostAddress(QString(DEFAULT))
#else  // !defined(_InetNtopQT)
    mString(DEFAULT)
#endif  // !defined(_QT)
{
}

// The implicitly defined copy constructor is fine.

Address::Address(String const &rString)
#ifdef _QT
:QHostAddress(QString(rString))
#endif  // defined(_QT)
{
#ifndef _QT
    if (rString.Contains(':')) {  // Assume it's an IPv6 address.
        IN6_ADDR ipv6_address;
        INT const success = inet_pton(AF_INET6, rString, &ipv6_address);
        if (success != 1) {
            mString.MakeEmpty();
        } else {
            char buffer[64];
            TextType const text = InetNtop(AF_INET6, &ipv6_address,
                buffer, sizeof(buffer));
            mString = String(text);
        }
    } else {  // Assume it's an IPv4 address.
        IN_ADDR ipv4_address;
        INT const success = inet_pton(AF_INET, rString, &ipv4_address);
        if (success != 1) {
            mString.MakeEmpty();
        } else {
            char buffer[64];
            TextType text = InetNtop(AF_INET, &ipv4_address,
                buffer, sizeof(buffer));
            mString = String(text);
        }
    }
#endif  // !defined(_QT)
}

// Construct based on an IPv4 address in network (bigendian) byte order.
Address::Address(unsigned long address)
#ifdef _QT
:QHostAddress(quint32(address))
#endif  // defined(_QT)
{
#ifndef _QT
    IN_ADDR ipv4_address;
    ipv4_address.s_addr = address;
    char buffer[64];
    TextType text = InetNtop(AF_INET, &ipv4_address, buffer, sizeof(buffer));
    mString = String(text);
#endif  // !defined(_QT)
}

#ifndef _QT

// Construct based on a sockaddr structure.
Address::Address(sockaddr const& rAddress) {
    ADDRESS_FAMILY const family = rAddress.sa_family;

    TextType text = NULL;
    char buffer[64];
    if (family == AF_INET6) {
        PSOCKADDR_IN6 const p_sockaddr_v6 = PSOCKADDR_IN6(&rAddress);
        PIN6_ADDR const p_ipv6 = &(p_sockaddr_v6->sin6_addr);
        text = InetNtop(family, p_ipv6, buffer, sizeof(buffer));
    } else if (family == AF_INET) {
        PSOCKADDR_IN const p_sockaddr_v4 = PSOCKADDR_IN(&rAddress);
        PIN_ADDR const p_ipv4 = &(p_sockaddr_v4->sin_addr);
        text = InetNtop(family, p_ipv4, buffer, sizeof(buffer));
    } else {
        FAIL();
    }
    mString = String(text);
}

#else  // defined(_QT)

// Construct from a QHostAddress object.
Address::Address(QHostAddress const& rAddress)
    :QHostAddress(rAddress)
{
}

#endif  // defined(_QT)

// The implicitly defined destructor is fine.


// operators

// The implicitly defined assignment operator is fine.

Address::operator String(void) const {
#ifdef _QT
    return String(toString());
#else  // !defined(_QT)
    return mString;
#endif  // !defined(_QT)
}

Address::operator unsigned long(void) const {
#ifdef _QT
    unsigned long const result = (unsigned long)(toIPv4Address());
#else  // !defined(_QT)
    IN_ADDR ipv4_address;
    INT const success = inet_pton(AF_INET, mString, &ipv4_address);
    ASSERT(success == 1);
    unsigned long const result = ipv4_address.s_addr;
#endif  // !defined(_QT)

    return result;
}


// misc methods

// List all useful IPv4/IPv6 addresses (excluding localhost).
/* static */  Strings Address::ListAll(void) {
    Strings result;

#ifdef _POSIX
    /*
    POSIX doesn't provide a way to enumerate network interfaces,
    so use Linux getifaddrs(3).
    */

    ifaddrs* p_list;
    int const error = ::getifaddrs(&p_list);
    ASSERT(error == 0);
    for (ifaddrs* p_current = p_list; p_current != NULL; p_current = p_current->ifa_next) {
        unsigned flags = p_current->ifa_flags;
        ADDRESS_FAMILY const family = p_current->ifa_addr->sa_family;
        if ((flags & IFF_UP)
            && !(flags & IFF_LOOPBACK)
            && (family == AF_INET || family == AF_INET6))
        {
            Address const address(*(p_current->ifa_addr));
            if (!address.IsLocalHost()) {
                // not localhost
                String const address_text = address;
                result.Append(address_text);
            }
        }
    }

    ::freeifaddrs(p_list);

#elif defined(_QT)

    QList<QHostAddress> const list = QNetworkInterface::allAddresses();
    QList<QHostAddress>::const_iterator i_address;
    for (i_address = list.constBegin(); i_address != list.constEnd(); i_address++) {
        QHostAddress const q_address = *i_address;
        Address const address(q_address);
        if (!address.IsLocalHost()) {
            // not localhost
            String const address_text = address;
            result.Append(address_text);
        }
    }

#elif defined(_WINSOCK2)

    ULONG const flags = (GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST);
    PIP_ADAPTER_ADDRESSES p_list = NULL;
    ULONG list_bytes = 16000;  // initial size estimate
    while (p_list == NULL) {
         p_list = PIP_ADAPTER_ADDRESSES(new char[list_bytes]);
         ASSERT(p_list != NULL);

         ULONG const error_code = Win::GetAdaptersAddresses(
             AF_UNSPEC, flags, NULL, p_list, &list_bytes);
         if (error_code != NO_ERROR) {
             if (error_code == ERROR_NO_DATA) {
                 return result;
             }
             ASSERT(error_code == ERROR_BUFFER_OVERFLOW);
             delete p_list;
             p_list = NULL;
         }
    }

    // Go through the list and append useful IP addresses to the result.
    PIP_ADAPTER_ADDRESSES p_current = p_list;

    while (p_current != NULL) {
        // Only interested in unicast addresses.
        PIP_ADAPTER_UNICAST_ADDRESS p_unicast = p_current->FirstUnicastAddress;
        while (p_unicast != NULL) {
            if ((p_unicast->Flags & IP_ADAPTER_ADDRESS_TRANSIENT) == 0x0) {
                // not transient
                Win::PSOCKADDR const p_address = p_unicast->Address.lpSockaddr;
                Address const address(*p_address);
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

    delete p_list;

#endif  // defined(_WINSOCK2)

    return result;
}


// inquiry methods

bool Address::IsLocalHost(void) const {
#ifdef _QT
    bool const result = (*this == QHostAddress::LocalHost
                      || *this == QHostAddress::LocalHostIPv6);
#else  // !defined(_QT)
    bool const result = (mString == LOCALHOST_IPV4
                      || mString == LOCALHOST_IPV6);
#endif  // !defined(_QT)

    return result;
}

bool Address::IsValid(void) const {
#ifdef _QT
    bool const result = !this->isNull();
#else  // !defined(_QT)
    bool const result = !mString.IsEmpty();
#endif  // !defined(_QT)

    return result;
}
