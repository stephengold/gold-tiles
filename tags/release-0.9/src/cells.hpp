#ifndef CELLS_HPP_INCLUDED
#define CELLS_HPP_INCLUDED

// File:     cells.hpp
// Location: src
// Purpose:  declare Cells class
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
A Cells object represents zero or more cells in a 2-D grid.

The Cells class is implemented using the std::set container template.
*/

#include <set>       // ISA std::set
#include "cell.hpp"  // HASA Cell


class Cells: public std::set<Cell> {
public:
    // public types
    typedef std::set<Cell>::const_iterator ConstIterator;
    typedef std::set<Cell>::iterator       Iterator;

    // public lifecycle
    Cells(void);
    explicit Cells(Cell const&);
    // Cells(Cells const&);  compiler-generated copy constructor is OK
    // ~Cells(void);  compiler-generated destructor is OK

    // public operators
    // Cell& operator=(Cell const&);  compiler-generated assignment operator is OK 
    operator String(void) const;

    // misc public methods
    void     Add(Cell const&);
    void     AddCells(Cells const&);
    unsigned Count(void) const;
    Cell     First(void) const;
    void     MakeEmpty(void);
    void     Remove(Cell const&);

    // public inquiry methods
    bool Contains(Cell const&) const;
    bool ContainsAll(Cells const&) const;
    bool IsAnyStart(void) const;
    bool IsEmpty(void) const;
};
#endif // !defined(CELLS_HPP_INCLUDED)
