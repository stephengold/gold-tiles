#ifndef STRING_HPP_INCLUDED
#define STRING_HPP_INCLUDED

// File:     string.hpp
// Location: src
// Purpose:  declare String class
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
A String object represents a text string (sequence of characters).

The String class is extends std::string in the Standard Template Library.
*/

#ifdef _QT
# include <QString>
#endif  // defined _QT
#include <string>      // ISA std::string
#include "fraction.hpp"
#include "project.hpp"

#ifndef __STDC_WANT_SECURE_LIB__
// for envs that don't provide strcpy_s() and strtok_s()
#define strcpy_s(a, sz, b) strcpy(a, b)
#define strtok_s(a, b, context) strtok(a, b)
#endif  // !defined(__STDC_WANT_SECURE_LIB__)

class String: public std::string {
public:
    // public types
    typedef const_iterator ConstIterator;
    typedef iterator       Iterator;

    // public lifecycle
    String(void);
    explicit String(bool);
    explicit String(char);
    explicit String(double);
    explicit String(int);            // itoa
    explicit String(long);           // ltoa
    explicit String(long long);      // lltoa
    explicit String(unsigned);       // utoa
    explicit String(unsigned long);  // ultoa
    explicit String(unsigned long long);  // ulltoa

    String(SizeType cnt, char);      // repeat characters
    String(TextType);
    String(std::string const&);
#ifdef _QT
    explicit String(QString const&);
#endif  // defined(_QT)
    String(Strings const&, String const& separator);  // join strings
    String(Strings const&, String const& separator, String const& final);  // join strings
    // String(String const&);  implicitly defined copy constructor
    // ~String(void);  implicitly defined destructor

    // public operators
    // String& operator=(String const&);  implicitly defined assignment method
    operator bool(void) const;
    operator double(void) const;
    operator int(void) const; // atoi
    operator long(void) const; // atol
    operator Fraction(void) const;
    operator TextType(void) const;

    // misc public methods
    void     Capitalize(void);
    char     Last(void) const;
    SizeType Length(void) const;
    void     MakeEmpty(void);
    String   Prefix(String const& suffix) const;
    String   Purge(void) const;
    String   Quote(void) const;
    void     Shorten(SizeType);
    String   Suffix(String const& prefix) const;

    // public inquiry methods
    bool Contains(char) const;
    bool HasPrefix(String const&) const;
    bool HasSuffix(String const&) const;
    bool IsBlank(void) const;
    bool IsEmpty(void) const;
};
#endif  // !defined(STRING_HPP_INCLUDED)
