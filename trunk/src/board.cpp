// File:    board.cpp
// Purpose: Board class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include <sstream>
#include "board.hpp"
#include "cells.hpp"
#include "indices.hpp"
#include "move.hpp"
#include "string.hpp"
#include "tilecell.hpp"
#include "tiles.hpp"

// constructors, assignment, and destructor

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated assignment method is OK.
// The compiler-generated destructor is OK.

// public methods

bool Board::AreAllColumnsCompatible(Cells const &rCells) const {
    D(std::cout << "Board::AreAllColumnsCompatible(" << String(rCells) << ")" << std::endl);
    bool result = true;
    
    Indices done_columns;
        
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        int column = i_cell->Column();
        if (!done_columns.Contains(column)) {
            if (!IsColumnCompatible(*i_cell)) {
                result = false;
                break;
            }
            done_columns.Add(column);
        }
    }
    
    return result; 
}


bool Board::AreAllCompatible(Cells const &rCells) const {
    bool result = true;

    if (rCells.Count() > 1) {
        Tiles tiles = GetTiles(rCells);
        if (!tiles.AreAllCompatible()) {
            result = false;
        }
    }
    
    return result;
}

bool Board::AreAllEmpty(Cells const &rCells) const {
    bool result = true;
    
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        if (!HasEmptyCell(*i_cell)) {
            result = false;
            break;
        }
    }
    
    return result;
}

bool Board::AreAllRowsCompatible(Cells const &rCells) const {
    bool result = true;
    
    Indices done_rows;

    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        int row = i_cell->Row();
        if (!done_rows.Contains(row)) {
            if (!IsRowCompatible(*i_cell)) {
                result = false;
                break;
            }
            done_rows.Add(row);
        }
    }
    
    return result; 
}

bool Board::ConnectsToOrigin(Cell const &rCell, Cells &rDoneCells) const { // recursive
    bool result = true;
    
    if (!rCell.IsOrigin()) {
        result = false;
        rDoneCells.insert(rCell);

        for (int direction = DIRECTION_FIRST; direction <= DIRECTION_LAST; direction++) {
            Cell look(rCell, (DirectionType)direction, 1);
            if (!HasEmptyCell(look) && 
				!rDoneCells.Contains(look) && 
				ConnectsToOrigin(look, rDoneCells))
			{
                result = true;
                break;
            }
        }
    }
    return result;
}

bool Board::DoesAnyConnectToOrigin(Cells const &rCells) const {
    bool result = false;
    
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        Cells done_cells;
        if (ConnectsToOrigin(*i_cell, done_cells)) {
            result = true;
            break;
        }
    }

    return result;
}

void Board::GetColumnLimits(
    Cell const &rCell,
    int &rFirstRow,
    int &rLastRow,
    int &rColumn) const
{
    rColumn = rCell.Column();

    rFirstRow = rCell.Row();
    while (!HasEmptyCell(rFirstRow - 1, rColumn)) {
        rFirstRow--;
    }

    rLastRow = rCell.Row();
    while (!HasEmptyCell(rLastRow + 1, rColumn)) {
        rLastRow++;
    }
}

Tile const *Board::GetPtr(int northing, int easting) const {
    Cell ref(northing, easting);
    Tile const *p_result = GetCell(ref);

    return p_result;
}

void Board::GetRowLimits(
    Cell const &rCell,
    int &rRow,
    int &rFirstColumn,
    int &rLastColumn) const
{
    rRow = rCell.Row();
    
    rFirstColumn = rCell.Column();
    while (!HasEmptyCell(rRow, rFirstColumn - 1)) {
        rFirstColumn--;
    }
    
    rLastColumn = rCell.Column();
    while (!HasEmptyCell(rRow, rLastColumn + 1)) {
        rLastColumn++;
    }
}

Tile Board::GetTile(int northing, int easting) const {
    Tile const *p_tile = GetPtr(northing, easting);
    ASSERT(p_tile != NULL);
    Tile result = *p_tile;

    return result;
}

Tiles Board::GetTiles(Cells const &rCells) const {
    Tiles result;
    
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {    
        Tile const *p_tile = GetCell(*i_cell);
        if (p_tile != NULL) {
            result.insert(*p_tile);
        }
    }

    return result;
}

bool Board::HasEmptyCell(int northing, int easting) const {
    Tile const *p_tile = GetPtr(northing, easting);
    bool result = (p_tile == NULL);

    return result;
}

bool Board::HasEmptyCell(Cell const &rCell) const {
    Tile const *p_tile = GetCell(rCell);
    bool result = (p_tile == NULL);

    return result;
}

bool Board::IsColumnCompatible(Cell const &rCell) const {
    D(std::cout << "Board::IsColumnCompatible(" << (String)rCell << ")" << std::endl);
    int first_row, last_row, column;
    GetColumnLimits(rCell, first_row, last_row, column);
    ASSERT(first_row <= last_row);
    bool result = true;
    
    for (int r1 = first_row; r1 <= last_row; r1++) {
        Tile t1 = GetTile(r1, column);
        for (int r2 = r1 + 1; r2 <= last_row; r2++) {
            Tile t2 = GetTile(r2, column);
            if (!t1.IsCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::IsConnectedColumn(Cells const &rCells) const {
     bool result = true;
    
    if (rCells.Count() > 1) {
        Cells::ConstIteratorType i_cell = rCells.begin();
        int column = i_cell->Column();
        int first_row = i_cell->Row();
        int last_row = i_cell->Row();
        for ( ; i_cell != rCells.end(); i_cell++) {
            if (column != i_cell->Column()) {
                return false;
            }
            
            int row = i_cell->Row();
            if (row > last_row) {
                last_row = row;
            }
            if (row < first_row) {
                first_row = row;
            }
        }
        
        for (int i_row = first_row; i_row <= last_row; i_row++) {
            if (HasEmptyCell(i_row, column)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::IsConnectedRow(Cells const &rCells) const {
    bool result = true;
    
    if (rCells.Count() > 1) {
        Cells::ConstIteratorType i_cell = rCells.begin();
        int row = i_cell->Row();
        int first_column = i_cell->Column();
        int last_column = i_cell->Column();
        for ( ; i_cell != rCells.end(); i_cell++) {
            if (row != i_cell->Row()) {
                return false;
            }
            
            int column = i_cell->Column();
            if (column > last_column) {
                last_column = column;
            }
            if (column < first_column) {
                first_column = column;
            }
        }
        
        for (int i_column = first_column; i_column <= last_column; i_column++) {
            if (HasEmptyCell(row, i_column)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::IsLegalMove(Move const &rMove) const {
    // a pass (no tiles played or swapped) is always legal
    if (rMove.IsPass()) {
        D(std::cout << "Legal pass." << std::endl);
        return true;
    }

    // check for repeated tiles
    if (rMove.RepeatsTile()) {
        D(std::cout << "Not legal: repeated tile(s)." << std::endl);
        return false;
    }

	if (rMove.IncludesSwap()) {
		if (!rMove.IsPureSwap()) {
            D(std::cout << "Not legal: mixture of place and swap." << std::endl);
            return false;
		}
        
		D(std::cout << "Legal swap." << std::endl);
        return true;
	}

	// check for repeated squares
    if (rMove.RepeatsCell()) {
        D(std::cout << "Not legal: repeated cell(s)." << std::endl);
        return false;
    }

    // get the set of board cells to be played
    Cells cells = rMove.GetCells();

    // make sure all those cells are empty
    if (!AreAllEmpty(cells)) {
        D(std::cout << "Not legal: cell(s) already played." << std::endl);
        return false;
    }

    // make sure the cells lie in a single row or column
    bool is_single_row = cells.AreAllInSameRow();
    bool is_single_column = cells.AreAllInSameColumn();
    if (!is_single_row && !is_single_column) {
        D(std::cout << "Not legal: multiple rows and columns." << std::endl);
        return false;
    }

    // make sure one of the cells will connect to the origin
    if (!DoesAnyConnectToOrigin(cells)) {
        D(std::cout << "Not legal: no connection to origin." << std::endl);
        return false;
    }

    // make a copy of the board and place the tiles on it
    Board after(*this);
    after.PlayMove(rMove);

    // make sure there are no empty squares between played tiles
    if (is_single_row) {
        if (!after.IsConnectedRow(cells)) {
            D(std::cout << "Not legal: gaps in row." << std::endl);
            return false;
        }
    } else {
        if (!after.IsConnectedColumn(cells)) {
            D(std::cout << "Not legal: gaps in column." << std::endl);
            return false;
        }
    }

    // check compatibility of connected tiles in each row and column played
    if (!after.AreAllRowsCompatible(cells)) {
        D(std::cout << "Not legal: incompatible tiles in same row." << std::endl);
        return false;
    }
    if (!after.AreAllColumnsCompatible(cells)) {
        D(std::cout << "Not legal: incompatible tiles in same column." << std::endl);
        return false;
    }

    D(std::cout << "Legal play." << std::endl);
    return true;
}

bool Board::IsRowCompatible(Cell const &rCell) const {
    int row, first_column, last_column;
    GetRowLimits(rCell, row, first_column, last_column);
    ASSERT(first_column <= last_column);
    bool result = true;   
    
    for (int c1 = first_column; c1 <= last_column; c1++) {
        Tile t1 = GetTile(row, c1);
        for (int c2 = c1 + 1; c2 <= last_column; c2++) {
            Tile t2 = GetTile(row, c2);
            if (!t1.IsCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

void Board::PlayMove(Move const &rMove) {
    Move::const_iterator i_place;
    for (i_place = rMove.begin(); i_place != rMove.end(); i_place++) {
        PlayTile(*i_place);
    }
}

void Board::PlayTile(TileCell const &rTileCell) {
    Cell cell = Cell(rTileCell);
    Tile tile = Tile(rTileCell);
    ASSERT(HasEmptyCell(cell));
    PlayOnCell(cell, tile);
}

unsigned Board::ScoreColumn(Cell const &rCell) const {
    int first_row, last_row, column;
    GetColumnLimits(rCell, first_row, last_row, column);
    ASSERT(first_row <= last_row);
    unsigned result = 0;

    if (first_row != last_row) {
        unsigned length = last_row + 1 - first_row;
        Tile first_tile = GetTile(first_row, column);
        Tile last_tile = GetTile(last_row, column);
        AIndexType attr = first_tile.CommonAttribute(last_tile);
        unsigned max_length = 1 + Tile::ValueMax(attr);
        if (length == max_length) {
            result = 2*length;
        } else {
            result = length;
        }
    }

    return result;
}

unsigned Board::ScoreMove(Move const &rMove) const {
    unsigned result = 0;

    Move::const_iterator i_place;
    Indices done_columns, done_rows;
    for (i_place = rMove.begin(); i_place != rMove.end(); i_place++) {
        Cell cell = Cell(*i_place);
        
        int row = cell.Row();
        if (!done_rows.Contains(row)) {
            result += ScoreRow(cell);
            done_rows.insert(row);
        }
        int column = cell.Column();
        if (!done_columns.Contains(column)) {
            result += ScoreColumn(cell);
            done_columns.insert(column);
        }
    }

    return result;
}

unsigned Board::ScoreRow(Cell const &rCell) const {
    int row, first_column, last_column;
    GetRowLimits(rCell, row, first_column, last_column);
    ASSERT(first_column <= last_column);
    unsigned result = 0;

    if (first_column != last_column) {
        unsigned run_length = last_column + 1 - first_column;
        Tile first_tile = GetTile(row, first_column);
        Tile last_tile = GetTile(row, last_column);
        AIndexType attr = first_tile.CommonAttribute(last_tile);
        unsigned max_length = 1 + Tile::ValueMax(attr);
        if (run_length == max_length) {
            result = 2*run_length;
        } else {
            result = run_length;
        }
    }

    return result;
}