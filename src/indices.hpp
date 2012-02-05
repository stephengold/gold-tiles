#ifndef INDICES_HPP_INCLUDED
#define INDICES_HPP_INCLUDED

// File:    indices.hpp
// Purpose: Indexes class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <set>

class Indices: public std::set<int> {
public:
	typedef std::set<int>::const_iterator ConstIteratorType;

	// misc
	void     Add(int);
	void     Remove(int);

	// access
    unsigned Count(void) const;

	// inquiry
    bool     Contains(int) const;
	bool     IsEmpty(void) const;
};

#endif
