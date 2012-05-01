// File:     indices.cpp
// Location: src
// Purpose:  implement Indices class
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

#include "indices.hpp"
#include "strings.hpp"


// static data
const String Indices::PREFIX("indices{");
const String Indices::SEPARATOR(" ");
const String Indices::SUFFIX("}");


// lifecycle

Indices::Indices(void) {
}

Indices::Indices(String const& rString) {
    bool const has_prefix = rString.HasPrefix(PREFIX);
    bool const has_suffix = rString.HasSuffix(SUFFIX);
    if (!has_prefix || !has_suffix) {
        FAIL(); // TODO recovery
    }
    String const body = rString.Suffix(PREFIX).Prefix(SUFFIX);

    Strings const words(body, SEPARATOR);

    Strings::ConstIterator i_word;
    for (i_word = words.Begin(); i_word != words.End(); i_word++) {
        String const word = *i_word;
        IndexType const tile(word);
        Add(tile);
    }
}

// The compiler-generated copy constructor is fine.

Indices::~Indices(void) {
}


// operators

// The compiler-generated assignment method is fine.

Indices::operator String(void) const {
    String result(PREFIX);

    ConstIterator i_index;
    for (i_index = begin(); i_index != end(); i_index++) {
        if (i_index != begin()) {
            result += SEPARATOR;
        }

        IndexType const index = *i_index;
        result += String(index);
    }       
    result += SUFFIX;

    return result;
}


// misc methods

void Indices::Add(IndexType index) {
    ASSERT(!Contains(index));

    insert(index);

    ASSERT(Contains(index));
}

void Indices::AddRemove(IndexType index, bool addFlag) {
    bool const have_index = Contains(index);

    if (have_index && !addFlag) {
        Remove(index);
    } else if (!have_index && addFlag) {
        Add(index);
    }

    ASSERT(addFlag == Contains(index));
}

unsigned Indices::Count(void) const {
    unsigned const result = size();

    return result;
}

IndexType Indices::First(void) const {
    ASSERT(!IsEmpty());

    ConstIterator const iterator = begin();
    IndexType const result = *iterator;

    ASSERT(Contains(result));
    return result;
}

IndexType Indices::Last(void) const {
    ASSERT(!IsEmpty());

    ConstReverseIterator const iterator = rbegin();
    IndexType const result = *iterator;

    ASSERT(Contains(result));
    return result;
}

void Indices::MakeEmpty(void) {
    clear();

    ASSERT(IsEmpty());
}

void Indices::Merge(Indices const& rIndices) {
    ConstIterator i_index;
    for (i_index = rIndices.begin(); i_index != rIndices.end(); i_index++) {
        IndexType const index = *i_index;
        Add(index);
    }
}

void Indices::Purge(Indices const& rIndices) {
    ASSERT(Count() >= rIndices.Count());

    ConstIterator i_index;
    for (i_index = rIndices.begin(); i_index != rIndices.end(); i_index++) {
        IndexType const index = *i_index;
        Remove(index);
    }
}

void Indices::Remove(IndexType index) {
    ASSERT(Contains(index));

    Iterator const i_index = find(index);
    ASSERT(i_index != end());
    ASSERT(*i_index == index);

    erase(i_index);

    ASSERT(!Contains(index));
}


// inquiry methods
bool Indices::Contains(IndexType index) const {
    bool const result = (find(index) != end());

    return result;
}

bool Indices::IsEmpty(void) const {
    bool const result = (Count() == 0);

    return result;
}
