#ifndef SAMPLE_HPP_INCLUDED
#define SAMPLE_HPP_INCLUDED

// File:     sample.hpp
// Location: src/templates
// Purpose:  declare sample class
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
A Sample object represents a ...

The Sample class extends/encapsulates the SampleBase class.
*/

#include "basesample.hpp"


class Sample: public BaseSample {
public:
	// public types

	// public constants

	// public lifecycle
	// Sample(void);  compiler-generated default constructor is OK
    // Sample(Sample const&);  compiler-generated copy constructor is OK
    // ~Sample(void);  compiler-generated destructor is OK

	// public operators
    // Sample &operator=(Sample const&);  compiler-generated assignment operator is OK

	// misc public methods

	// public inquiry methods

private:
	// private types

	// private constants

	// private data

	// private lifecycle

	// private operators

	// misc private methods

	// private inquiry methods

};
#endif // !defined(SAMPLE_HPP_INCLUDED)
