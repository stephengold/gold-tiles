// File:     address.cpp
// Location: src
// Purpose:  Address class for the Gold Tile Game
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

#ifdef _WINSOCK2
# include "gui/win_types.hpp"
namespace Win {
# include <iphlpapi.h> // IP help API
# pragma comment(lib, "iphlpapi.lib")  // link with iphlpapi.lib
};
using Win::IN_ADDR;
using Win::MIB_IPADDRTABLE;
using Win::PMIB_IPADDRROW;
using Win::PMIB_IPADDRTABLE;
#endif // defined(_WINSOCK2)


// lifecycle

Address::Address(void):
    mString("0.0.0.0")
{}

// The compiler-generated copy constructor is fine.

Address::Address(String const &rString) {
	unsigned long const address = Win::inet_addr(rString);

	IN_ADDR ipv4_address;
    ipv4_address.S_un.S_addr = address;
	TextType const text = Win::inet_ntoa(ipv4_address);
    mString = String(text);
}

Address::Address(unsigned long address) {
	IN_ADDR ipv4_address;
    ipv4_address.S_un.S_addr = address;
	TextType const text = Win::inet_ntoa(ipv4_address);
    mString = String(text);
}

// The compiler-generated destructor is fine.


// operators

// The compiler-generated assignment operator is fine.

Address::operator String(void) const {
	return mString;
}

Address::operator unsigned long(void) const {
	unsigned long const result = Win::inet_addr(mString);

	return result;
}


// misc methods

// List all usable, local IP addresses, excluding localhost.
/* static */ Strings Address::ListAll(void) {
	// allocate a too-small buffer for the address table
    PMIB_IPADDRTABLE p_table = new MIB_IPADDRTABLE;
	ASSERT(p_table != NULL);

	// query for table size
    DWORD table_bytes = 0;
    DWORD error_code = Win::GetIpAddrTable(p_table, &table_bytes, 0);
	ASSERT(error_code == ERROR_INSUFFICIENT_BUFFER);

	// resize the address table
	delete p_table;
    p_table = PMIB_IPADDRTABLE(new char[table_bytes]);
	ASSERT(p_table != NULL);
    error_code = Win::GetIpAddrTable(p_table, &table_bytes, 0);
    ASSERT(error_code == NO_ERROR);

	Strings result;

	// Select rows from the table and append usable IP addresses to the result.
	unsigned const row_cnt = p_table->dwNumEntries;
    for (unsigned i_row = 0; i_row < row_cnt; i_row++) {
		PMIB_IPADDRROW const p_row = &(p_table->table[i_row]);
		unsigned short w_type = p_row->wType;
	    unsigned short mask = (MIB_IPADDR_TRANSIENT | MIB_IPADDR_DISCONNECTED | MIB_IPADDR_DELETED);
		if ((w_type & mask) == 0x0) {
		    // The row is neither transient, nor disconnected, nor undergoing deletion.
			Address const address(p_row->dwAddr);
			if (address.Net() != LOCALHOST_NET) { // skip localhost
			    String const address_text = address;
                result.Append(address_text);
			}
		}
    }
    delete p_table;

	return result;
}

Address::NetType Address::Net(void) const {
	unsigned long const address = Win::inet_addr(mString);

	IN_ADDR ipv4_address;
    ipv4_address.S_un.S_addr = address;
    NetType const result = ipv4_address.s_net;

	return result;
}
