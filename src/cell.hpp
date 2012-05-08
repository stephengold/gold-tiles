#ifndef CELL_HPP_INCLUDED
#define CELL_HPP_INCLUDED

// File:     cell.hpp
// Location: src
// Purpose:  declare Cell class
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
A Cell object identifies a location in a 2-D grid.
Cells can be accessed by row+column or northing+easting,
with row number increasing northward and column number
increasing eastward.  Either coordinate can be negative.
Cells are used as keys by the BaseBoard and Board classes.

The Cell class encapsulates a pair of indices:  one index
for the row (or northing) and another for the column (or easting).
*/

#include "indices.hpp"  // HASA IndexType
#include "string.hpp"   // USES String

enum GridType {
    GRID_TRIANGLE = 3,
    GRID_4WAY = 4,
    GRID_HEX = 6,
    GRID_8WAY = 8,
    GRID_DEFAULT = GRID_4WAY
};

typedef IndexType ColumnType;
typedef IndexType RowType;

class Cell {
public:
    // public constants
    static const RowType    HEIGHT_MAX = 0x40000000L;
    static const RowType    HEIGHT_MIN = 4;
    static const ColumnType WIDTH_MAX  = 0x40000000L;
    static const ColumnType WIDTH_MIN = 4;

    // public lifecycle
    Cell(void);   // the start cell
    // Cell(Cell const&);  implicitly defined copy constructor
    explicit Cell(String const&);
    Cell(RowType, ColumnType);
    Cell(Cell const&, Direction const&, int count);
    // ~Cell(void);  implicitly defined destructor

    // public operators
    bool operator!=(Cell const&) const;
    bool operator<(Cell const&) const;
    // Cell& operator=(Cell const&);  implicitly defined assignment method
    bool operator==(Cell const&) const;
    operator String(void) const;

    // misc public methods
    ColumnType      Column(void) const;
    bool            GetUserChoice(String const&);
    static GridType Grid(void);
    static void     LimitPlay(unsigned&);
    void            Next(Direction const&, int count = 1);
    RowType         Row(void) const;
    static int      RowFringe(void);
    static String   ScoringAxes(void);
    static void     SetStatic(GameOpt const&);
    void            Wrap(void);

    // public inquiry methods
    static bool DoesBoardWrap(void);
    bool        HasNeighbor(Direction const&) const;
    bool        IsOdd(void) const;
    static bool IsScoringAxis(Direction const&);
    bool        IsStart(void) const;
    bool        IsValid(void) const;

private:
    // private constants
    static const String PREFIX;
    static const String SEPARATOR;
    static const String SUFFIX;

    // private data
    ColumnType         mColumn;
    static GridType   msGrid;
    static RowType    msHeight;   // must be even and <= HEIGHT_MAX
    RowType            mRow;
    static ColumnType msWidth;    // must be even and <= WIDTH_MAX
    static bool       msWrapFlag; // coordinates wrap around

    // misc private methods
    static void NextCellOffsets(Direction const&, RowType&, ColumnType&);
};

// global utility functions
String    grid_to_string(GridType);
String    index_to_string(IndexType);
GridType  string_to_grid(String const&);
IndexType string_to_index(String const&);

#endif // !defined(CELL_HPP_INCLUDED)
