#ifndef STRINGS_HPP_INCLUDED
#define STRINGS_HPP_INCLUDED

// File:    strings.hpp
// Purpose: Strings class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
  A Strings object represents an ordered collection of unique text strings.
*/

#include <list>
#include "string.hpp"

class Strings {
public:
    typedef std::list<String>::const_iterator ConstIteratorType;

	// misc
	void Append(String const &);
	ConstIteratorType Begin(void) const;
	ConstIteratorType End(void) const;
	ConstIteratorType Find(String const &) const;

private:
	std::list<String> mList;
};

#endif