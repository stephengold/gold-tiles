// File:     string.cpp
// Location: src
// Purpose:  implement String class
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

#include <locale>
#include <sstream>
#include "strings.hpp"

static std::string itos(int integer) {
    std::ostringstream sout;
    sout << integer;
    return sout.str(); 
}
static std::string utos(unsigned integer) {
    std::ostringstream sout;
    sout << integer;
    return sout.str(); 
}
static std::string ltos(long integer) {
    std::ostringstream sout;
    sout << integer;
    return sout.str(); 
}
static std::string ultos(unsigned long integer) {
    std::ostringstream sout;
    sout << integer;
    return sout.str(); 
}
static std::string lftos(double number) {
    std::ostringstream sout;
    sout << number;
    return sout.str();
}


// lifecycle

// construct an empty string
String::String(void):
std::string() {}

// conversions from base types
String::String(bool flag):
std::string(flag ? "true" : "false")
{
}
String::String(char character):
std::string(1, character)
{
}
String::String(double number):
std::string(::lftos(number))
{
}
String::String(int integer):
std::string(::itos(integer))
{
}
String::String(unsigned integer):
std::string(::utos(integer))
{
}
String::String(long integer):
std::string(::ltos(integer))
{
}
String::String(unsigned long integer):
std::string(::ultos(integer))
{
}

// repetition
String::String(unsigned repeatCnt, char character):
std::string(repeatCnt, character)
{
}

// copying
#ifdef _QT
String::String(QString const& rString):
    std::string(rString.toStdString())
{
}
#endif // defined(_QT)

String::String(std::string const& rString):
std::string(rString)
{
}

String::String(TextType text):
std::string(text)
{
}

// joining
String::String(Strings const& rList, String const& rSeparator) {
    Strings::ConstIterator i_string;
    for (i_string = rList.Begin(); i_string != rList.End(); i_string++) {
        if (i_string != rList.Begin()) {
            *this += rSeparator;
        }
        *this += *i_string;
    }
}


// operators

String::operator bool(void) const {
    bool result = false;

    if (*this == "true") {
        result = true;
    } else if (*this == "false") {
        result = false;
    } else {
        FAIL();
    }

    return result;
}

String::operator double(void) const {
    TextType const text = c_str();
    double const result = ::atof(text);

    return result;
}

String::operator long(void) const {
    TextType const text = c_str();
    long const result = ::atol(text);

    return result;
}

String::operator Fraction(void) const {
    TextType const text = c_str();
    Fraction const result = ::atof(text);

    return result;
}

String::operator TextType(void) const {
    TextType const result = c_str();

    return result;
}


// misc methods

// Capitalize the first letter of each word.
// Trim leading and trailing non-graphic characters.
// Convert adjacent non-graphic characters a single space.
// Equal-signs (=) are treated as non-graphic characters.
void String::Capitalize(void) {
    bool after_graphic = false;
    String result;
    char const space = ' ';

    for (unsigned i_char = 0; i_char < Length(); i_char++) {
        char ch = at(i_char);
        bool const is_graphic = (::isgraph(ch) != 0 && ch != '=');

        if (!is_graphic) {
            ch = space;
        } else if (!after_graphic) { // first letter of a word
            ch = char(::toupper(ch));
        }

        if (is_graphic || after_graphic) {
            result += ch;
        }

        after_graphic = is_graphic;
    }

    *this = result;

    if (!IsEmpty() && Last() == space) {
        // remove trailing space, which was originally some non-graphic character
        Shorten(1);
    }
}

char String::Last(void) const {
    ASSERT(!IsEmpty());

    unsigned const length = Length();
    char const result = at(length - 1);

    return result;
}

unsigned String::Length(void) const {
    unsigned const result = size();

    return result;
}

void String::MakeEmpty(void) {
    clear();
}

String String::Prefix(String const& rSuffix) const {
    String result;

    unsigned const suffix_length = rSuffix.Length();
    if (Length() >= suffix_length) {
        unsigned const prefix_length = Length() - suffix_length;
        result = substr(0, prefix_length);
    }

    return result;
}

// purge non-graphic characters
String String::Purge(void) const {
    String result;

    for (unsigned i_char = 0; i_char < Length(); i_char++) {
        char const ch = at(i_char);
        bool const is_graphic = (::isgraph(ch) != 0);

        if (is_graphic) {
            result += ch;
        }
    }

    return result;
}

// enclose the string in single quotes
String String::Quote(void) const {
    String const result("'" + *this + "'");

    return result;
}

// delete characters from the end
void String::Shorten(unsigned characterCnt) {
    unsigned const length = Length();
    unsigned new_length = length - characterCnt;
    if (characterCnt >= length) {
        new_length = 0;
    }
    resize(new_length);
}

String String::Suffix(String const& rPrefix) const {
    String result;

    unsigned const prefix_length = rPrefix.Length();
    if (Length() >= prefix_length) {
        unsigned const suffix_length = Length() - prefix_length;
        result = substr(prefix_length, suffix_length);
    }

    return result;
}


// inquiry methods

bool String::HasPrefix(String const& rPrefix) const {
    bool result = false;

    unsigned const prefix_length = rPrefix.Length();
    if (Length() >= prefix_length) {
        String const prefix = substr(0, prefix_length);
        result = (prefix == rPrefix);
    }

    return result;
}

bool String::HasSuffix(String const& rSuffix) const {
    bool result = false;

    unsigned const suffix_length = rSuffix.Length();
    if (Length() >= suffix_length) {
        unsigned const prefix_length = Length() - suffix_length;
        String const suffix = substr(prefix_length, suffix_length);
        result = (suffix == rSuffix);
    }

    return result;
}

bool String::IsEmpty(void) const {
    bool const result = (Length() == 0);

    return result;
}
