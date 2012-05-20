#ifndef INDICES_HPP_INCLUDED
#define INDICES_HPP_INCLUDED

// File:     indices.hpp
// Location: src
// Purpose:  declare Indexes class
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
An Indices object represents a set of indices, which may be rows or 
columns in a grid or tile IDs.

The Indices class is implemented by applying the "set" container 
template (from the Standard Template Library) to IndexType.
*/

#include <climits>    // USES LONG_MAX
#include <set>        // ISA std::set
#include "string.hpp" // USES String

typedef int32_t IndexType;

class Indices: public std::set<IndexType> {
public:
    // public types
    typedef std::set<IndexType>::const_iterator         ConstIterator;
    typedef std::set<IndexType>::const_reverse_iterator ConstReverseIterator;
    typedef std::set<IndexType>::iterator               Iterator;

    // public constants
    static const IndexType INDEX_MAX = LONG_MAX;
    static const IndexType INDEX_MIN = LONG_MIN;

    // public lifecycle
    Indices(void);
    // Indices(Indices const&);  implicitly defined copy constructor
    explicit Indices(String const&);
    virtual ~Indices(void);

    // public operators
    //Indices& operator=(Indices const&);  implicitly defined assignment method
    operator String(void) const;

    // misc public methods
    void      Add(IndexType);
    void      AddRemove(IndexType, bool);
    SizeType  Count(void) const;
    IndexType First(void) const;
    IndexType Last(void) const;
    void      MakeEmpty(void);
    void      Merge(Indices const&);
    void      Purge(Indices const&);
    void      Remove(IndexType);

    // public inquiry methods
    bool Contains(IndexType) const;
    bool IsEmpty(void) const;

private:
    // private constants
    static const String PREFIX;
    static const String SEPARATOR;
    static const String SUFFIX;
};
#endif // !defined(INDICES_HPP_INCLUDED)
