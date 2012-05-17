// File:     strings.cpp
// Location: src
// Purpose:  implement Strings class
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

#include <string.h>
#include "strings.hpp"


// lifecycle

Strings::Strings(void) {
}

Strings::Strings(TextType list, TextType separators) {
    // Create a writable copy of the list.
    unsigned const length = ::strlen(list);
    char* copy_list = new char[length + 1];
    ::strcpy_s(copy_list, length + 1, list);

    // Parse the list into tokens.
    char* next_token = NULL;
    TextType token = ::strtok_s(copy_list, separators, &next_token);
    while (token != NULL) {
        Append(token);
        token = ::strtok_s(NULL, separators, &next_token);
    }

    delete[] copy_list;
}

// The implicitly defined copy constructor is OK.
// The implicitly defined destructor is OK.


// operators

// The implicitly defined assignment method is OK.


// misc methods

void Strings::Append(String const& rString) {
    mList.push_back(rString);
}

Strings::ConstIterator Strings::Begin(void) const {
    ConstIterator const result = mList.begin();

    return result;
}

Strings::Iterator Strings::Begin(void) {
    Iterator const result = mList.begin();

    return result;
}

unsigned Strings::Count(void) const {
    unsigned const result = mList.size();

    return result;
}

unsigned Strings::Count(String const& rString) const {
    unsigned result = 0;
    ConstIterator i_string;
    for (i_string = Begin(); i_string != End(); i_string++) {
        if (*i_string == rString) {
            result++;
        }
    }

    return result;
}

Strings::ConstIterator Strings::End(void) const {
    ConstIterator const result = mList.end();

    return result;
}

Strings::Iterator Strings::End(void) {
    Iterator const result = mList.end();

    return result;
}

Strings::ConstIterator Strings::Find(String const& rString) const {
    ConstIterator i_string;
    for (i_string = Begin(); i_string != End(); i_string++) {
        if (*i_string == rString) {
            break;
        }
    }

    return i_string;
}

String Strings::First(void) const {
    ASSERT(!IsEmpty());

    ConstIterator const i = Begin();
    String const result = *i;

    return result;
}

// create unique name using numeric suffix
String Strings::InventUnique(String const& rPrefix) const {
    String result = rPrefix;

    unsigned i = 2;
    while (Contains(result)) {
        result = rPrefix + String(i);
        i++;
    }

    return result;
}

// create unique name using ordinals
String Strings::InventUnique(
    String const& rPrefix,
    String const& rInfix,
    String const& rSuffix) const
{
    String result = rPrefix;
    unsigned i = 1;
    while (Contains(result)) {
        result = rPrefix + rInfix + ::ordinal(i) + rSuffix;
        i++;
    }

    return result;
}

void Strings::MakeEmpty(void) {
    mList.clear();

    ASSERT(IsEmpty());
}

String Strings::Second(void) const {
    ASSERT(Count() > 1);

    ConstIterator i = Begin();
    i++;
    String const result = *i;

    return result;
}

void Strings::Unappend(void) {
    ASSERT(!IsEmpty());

    mList.pop_back();
}

// list the unique members in the object
Strings Strings::Unique(void) const {
    Strings result;

    ConstIterator i_string;
    for (i_string = Begin(); i_string != End(); i_string++) {
        if (!result.Contains(*i_string)) {
            result.Append(*i_string);
        }
    }

    return result;
}


// inquiry methods

bool Strings::Contains(String const& rString) const {
    bool const result = (Find(rString) != End());

    return result;
}

bool Strings::IsEmpty(void) const {
    bool const result = (mList.size() == 0);

    return result;
}
