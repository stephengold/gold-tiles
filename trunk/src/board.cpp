// File:    board.cpp
// Purpose: Board class
// Author:  Stephen Gold sgold@sonic.net
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

#include <iostream>
#include <sstream>
#include "board.hpp"
#include "cells.hpp"
#include "indices.hpp"
#include "move.hpp"
#include "string.hpp"
#include "tilecell.hpp"
#include "tiles.hpp"

// lifecycle

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.


// misc methods

void Board::GetLimits(
	Cell const &rCell,
	DirectionType direction,
	Cell &rFirst,
	Cell &rLast) const
{
    Cell first_cell = rCell;
    while (!HasEmptyCell(first_cell)) {
        first_cell = Cell(first_cell, direction, -1);
    }
    
    Cell last_cell = rCell;
    while (!HasEmptyCell(last_cell)) {
        last_cell = Cell(last_cell, direction, +1);
    }
}

TileIdType Board::GetId(Cell const &rCell) const {
    TileIdType result = 0;
    Tile const *p_tile = GetCell(rCell);
	if (p_tile != NULL) {
		result = p_tile->Id();
	}

	return result;
}

Tile const *Board::GetPtr(IndexType northing, IndexType easting) const {
    Cell ref(northing, easting);
    Tile const *p_result = GetCell(ref);

    return p_result;
}

Tile Board::GetTile(IndexType northing, IndexType easting) const {
	ASSERT(Cell::IsValid(northing, easting));

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
            result.Add(*p_tile);
        }
    }

    return result;
}

bool Board::LocateTile(Tile const &rTile, Cell &rCell) const {
	TileIdType id = rTile.Id();
	bool result = BaseBoard::LocateTileId(id, rCell);

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
    Tile tile = rTileCell.operator Tile();
    ASSERT(HasEmptyCell(cell));
    PlayOnCell(cell, tile);
}

unsigned Board::ScoreDirection(
	Cell const &rCell,
	DirectionType direction) const
{
    Cell first_cell, last_cell;
    GetLimits(rCell, direction, first_cell, last_cell);

	unsigned result = 0;

    if (first_cell != last_cell) {
        unsigned length = first_cell.Distance(last_cell) + 1;
        Tile first_tile = GetTile(first_cell);
        Tile last_tile = GetTile(last_cell);
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
        
        IndexType row = cell.Row();
        if (!done_rows.Contains(row)) {
            result += ScoreDirection(cell, DIRECTION_NORTH);
            done_rows.insert(row);
        }
        IndexType column = cell.Column();
        if (!done_columns.Contains(column)) {
            result += ScoreDirection(cell, DIRECTION_EAST);
            done_columns.insert(column);
        }
    }

    return result;
}


// inquiry methods

bool Board::AreAllCompatible(Cells const &rCells, DirectionType direction) const {
    bool result = true;
    
    Indices done_groups;
        
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        IndexType group = i_cell->Group(direction);
        if (!done_groups.Contains(group)) {
            if (!IsDirectionCompatible(*i_cell, direction)) {
                result = false;
                break;
            }
            done_groups.Add(group);
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

bool Board::ConnectsToStart(Cell const &rCell) const {
	Cells done;
    bool result = ConnectsToStart(rCell, done);

	return result;
}

bool Board::ConnectsToStart(Cell const &rCell, Cells &rDoneCells) const { // recursive
    bool result = true;
    
	ASSERT(rCell.IsValid());
    if (!rCell.IsStart()) {
        result = false;
        rDoneCells.insert(rCell);

        for (int i_dir = DIRECTION_FIRST; i_dir <= DIRECTION_LAST; i_dir++) {
			DirectionType direction = (DirectionType)i_dir;
			if (rCell.HasNeighbor(direction)) {
                Cell look(rCell, direction);
				ASSERT(look.IsValid());
                if (!HasEmptyCell(look) && 
				    !rDoneCells.Contains(look) && 
				    ConnectsToStart(look, rDoneCells))
			    {
                    result = true;
                    break;
                }
			}
        }
    }
    return result;
}

bool Board::Contains(Tile const &rTile) const {
	Cell cell;
	TileIdType id = rTile.Id();
	bool result = LocateTileId(id, cell);

	return result;
}

bool Board::ContainsId(TileIdType id) const {
	Cell cell;
	bool result = LocateTileId(id, cell);

	return result;
}

bool Board::DoesAnyConnectToStart(Cells const &rCells) const {
    bool result = false;
    
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
		Cell cell = *i_cell;
        if (ConnectsToStart(cell)) {
            result = true;
            break;
        }
    }

    return result;
}

bool Board::HasEmptyCell(IndexType northing, IndexType easting) const {
    Tile const *p_tile = GetPtr(northing, easting);
    bool result = (p_tile == NULL);

    return result;
}

bool Board::HasEmptyCell(Cell const &rCell) const {
    Tile const *p_tile = GetCell(rCell);
    bool result = (p_tile == NULL);

    return result;
}

bool Board::IsDirectionCompatible(Cell const &rCell, DirectionType direction) const {
    Cell first_cell, last_cell;
    GetLimits(rCell, direction, first_cell, last_cell);
    bool result = true;
    
    for (Cell cell1 = first_cell; cell1 != last_cell; cell1.Next(direction)) {
        Tile t1 = GetTile(cell1);
        for (Cell cell2(cell1, direction); cell1 != last_cell; cell2.Next(direction)) {
            Tile t2 = GetTile(cell2);
            if (!t1.IsCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::IsConnectedDirection(Cells const &rCells, DirectionType direction) const {
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

bool Board::IsEmpty(void) const {
	bool result = (Count() == 0);

	return result;
}

bool Board::IsValidMove(Move const &rMove) const {
	char const *reason;
	bool result = IsValidMove(rMove, reason);

	return result;
}

bool Board::IsValidMove(Move const &rMove, char const *&rReason) const {
    // a pass (no tiles played or swapped) is always legal
    if (rMove.IsPass()) {
        D(std::cout << "The move is a pass." << std::endl);
        return true;
    }

    // check for repeated tiles
    if (rMove.RepeatsTile()) {
        D(std::cout << "You can't use the same tile twice." << std::endl);
		rReason = "REPEATTILE";
        return false;
    }

	if (rMove.InvolvesSwap()) {
		if (!rMove.IsPureSwap()) {
            D(std::cout << "You can play tiles or swap them, "
				<< "but you can't do both in the same turn." << std::endl);
   		    rReason = "SWAP";
            return false;
		}
        
		D(std::cout << "The move is a valid swap." << std::endl);
        return true;
	}

	// check for repeated cells
    if (rMove.RepeatsCell()) {
        D(std::cout << "You can't use the same cell twice." << std::endl);
		rReason = "REPEATCELL";
        return false;
    }

    // get the set of board cells to be played
    Cells cells = Cells(rMove);

    // make sure all those cells are empty
    if (!AreAllEmpty(cells)) {
        D(std::cout << "You can only use empty cells." << std::endl);
		rReason = "EMPTY";
        return false;
    }

    // make sure the cells lie in a single row or column
    bool is_single_row = cells.AreAllInSameRow();
    bool is_single_column = cells.AreAllInSameColumn();
    if (!is_single_row && !is_single_column) {
        D(std::cout << "The cells you use must all lie in a single row or column." << std::endl);
		rReason = "ROWCOLUMN";
        return false;
    }

    // make sure one of the cells will connect to the start
    if (!DoesAnyConnectToStart(cells)) {
		if (IsEmpty()) {
            D(std::cout << "Your first tile must be played on the start cell. "
				>> "To change this tile, you must take back ALL your tiles." << std::endl);
		    rReason = "START";
		} else {
            D(std::cout << "Each cell you use must be a neighbor of a used cell." << std::endl);
		    rReason = "NEIGHBOR";
		}
        return false;
    }

    // make a copy of the board and place the tiles on it
    Board after(*this);
    after.PlayMove(rMove);

    // make sure there are no empty squares between played tiles
    if (is_single_row) {
        if (!after.IsConnectedDirection(cells, DIRECTION_EAST)) {
            D(std::cout << "You must not leave any empty cells between the tiles you play." 
				<< std::endl);
     		rReason = "GAP";
            return false;
        }
    } else {
        if (!after.IsConnectedDirection(cells, DIRECTION_NORTH)) {
            D(std::cout << "You must not leave any empty cells between the tiles you play." 
				<< std::endl);
     		rReason = "GAP";
			return false;
        }
    }

    // check compatibility of connected tiles in each row and column played
    if (!after.AreAllCompatible(cells, DIRECTION_EAST)) {
        D(std::cout << "Tiles in a row (with no intervening empty cells) "
			>> "must all be mutually compatible." << std::endl);
		rReason = "ROWCOMPAT";
        return false;
    }
    if (!after.AreAllCompatible(cells, DIRECTION_NORTH)) {
        D(std::cout << "Tiles in a column (with no intervening empty cells) "
			>> "must all be mutually compatible." << std::endl);
		rReason = "COLUMNCOMPAT";
        return false;
    }

    D(std::cout << "The move is a valid play." << std::endl);
    return true;
}
