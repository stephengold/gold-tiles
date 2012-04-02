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

TileIdType Board::GetId(Cell const &rCell) const {
    TileIdType result = 0;
    Tile const *const p_tile = GetCell(rCell);
	if (p_tile != NULL) {
		result = p_tile->Id();
	}

	return result;
}

long Board::GetLimits(
	Cell const &rCell,
	DirectionType direction,
	Cell &rFirst,
	Cell &rLast) const
{
    ASSERT(!HasEmptyCell(rCell));
	long result = -1;
    
    rFirst = rCell;
    while (!HasEmptyCell(rFirst)) {
        // TODO edge effects
        rFirst.Next(direction, -1);
		++result;
    }
    rFirst.Next(direction);
    
    rLast = rCell;
    while (!HasEmptyCell(rLast)) {
        rLast.Next(direction);
		++result;
    }
    rLast.Next(direction, -1);

	return result;
}

Tile Board::GetTile(Cell const &rCell) const {
	ASSERT(rCell.IsValid());

    Tile const *p_tile = GetCell(rCell);
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
	TileIdType const id = rTile.Id();
	bool const result = BaseBoard::LocateTileId(id, rCell);

	return result;
}

void Board::PlayMove(Move const &rMove) {
    Move::ConstIterator i_place;
    for (i_place = rMove.Begin(); i_place != rMove.End(); i_place++) {
        PlayTile(*i_place);
    }
}

void Board::PlayTile(TileCell const &rTileCell) {
    Cell const cell = Cell(rTileCell);
    Tile const tile = rTileCell.operator Tile();
    ASSERT(HasEmptyCell(cell));
    PlayOnCell(cell, tile);
}

unsigned Board::ScoreDirection(
	Cell const &rCell,
	DirectionType direction) const
{
    ASSERT(!HasEmptyCell(rCell));
    
    Cell first_cell, last_cell;
    unsigned const length = GetLimits(rCell, direction, first_cell, last_cell);

	unsigned result = 0;
    if (length > 1) {
		result = length; // base score

		// examine each tiles in the group
        Cell i_cell(first_cell);
		for (unsigned i = 0; i < length; i++) {
            Tile const tile = GetTile(i_cell);
	        // double the score for each bonus tile
			if (tile.HasBonus()) {
				result *= 2;
			}
            i_cell.Next(direction);
		}

		// special bonus for two-attribute games
		if (Tile::AttributeCnt() == 2) {
		    // determine the common attribute
            Tile const first_tile = GetTile(first_cell);
            Tile const last_tile = GetTile(last_cell);
            AIndexType const common_attr = first_tile.CommonAttribute(last_tile);
			AIndexType const other_attr = 1 - common_attr;
            unsigned const max_length = Tile::ValueCnt(other_attr);

		    // double the score yet again if at max length
		    ASSERT(length <= max_length);
            if (length == max_length) {
				result *= 2;
		    }
		}
    }

    return result;
}

/* static */ String Board::ReasonMessage(char const *reason, String &rTitle) {
	String message = reason;
	rTitle = "Information";

	// expand shortcuts
	if (::str_eq(message, "COLUMNCOMPAT")) {
		message = "Tiles in a column (with no intervening empty cells) must all be mutually compatible.";
		rTitle = "Column Compatibility Rule";
	} else if (::str_eq(message, "DIAGCOMPAT")) {
		message = "Tiles on a diagonal (with no intervening empty cells) must all be mutually compatible.";
		rTitle = "Diagonal Compatibility Rule";
	} else if (::str_eq(message, "EMPTY")) {
		message = "That cell has already been used.";
		rTitle = "Empty Cell Rule";
	} else if (::str_eq(message, "FIRST")) {
		message = "On the first turn, you must play as many tiles as possible.  Keep looking!";
		rTitle = "First Turn Rule";
	} else if (::str_eq(message, "GAP")) {
		message = "You can't leave any empty cells between the tiles you play.";
		rTitle = "Gap Rule";
	} else if (::str_eq(message, "NEIGHBOR")) {
		message = "Each cell you use must be a neighbor of a used cell.";
		rTitle = "Neighbor Rule";
	} else if (::str_eq(message, "REPEATCELL")) {
		message = "You can't use the same cell twice.";
		rTitle = "Repeated Cell Rule";
	} else if (::str_eq(message, "REPEATTILE")) {
		message = "You can't use the same tile twice.";
		rTitle = "Repeated Tile Rule";
	} else if (::str_eq(message, "ROWCOLUMN")) {
		String dirs;
		switch (Cell::Grid()) {
		case GRID_TRIANGLE:
			dirs = "row or diagonal";
			break;
		case GRID_4WAY:
			dirs = "row or column";
			break;
		case GRID_HEX:
			dirs = "column or diagonal";
			break;
		case GRID_8WAY:
		    dirs = "row, column, or diagonal";
			break;
		default:
			FAIL();
		}
		message = String("The cells you use must all lie in a single ") + dirs + String(".");
		rTitle = "Row/Column Rule";
	} else if (::str_eq(message, "ROWCOMPAT")) {
		message = "Tiles in a row (with no intervening empty cells) must all be mutually compatible.";
		rTitle = "Row Compatibility Rule";
	} else if (::str_eq(message, "RULES")) {
		message = "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing";
		rTitle = "Rules";
	} else if (::str_eq(message, "START")) {
		message = "Your first tile must use the start cell. To change this tile, you must take back ALL your tiles.";
		rTitle = "Start Rule";
	} else if (::str_eq(message, "STARTSIMPLE")) {
		message = "Your first tile must use the start cell.";
		rTitle = "Start Rule";
	} else if (::str_eq(message, "STOCK")) {
		message = "You can't swap more tiles than the number remaining in the stock bag.";
		rTitle = "Stock Rule";
	} else if (::str_eq(message, "SWAP")) {
		message = "You can play tiles or swap them, but you can't do both in the same turn.";
		rTitle = "Swap Rule";
	}

    return message;
}

unsigned Board::ScoreMove(Move const &rMove) const {
    unsigned result = 0;
    
    Cells const cells = Cells(rMove);
    
    for (int dir = DIRECTION_FIRST; 
             dir <= DIRECTION_LAST_POSITIVE;
             dir++)
    {
        DirectionType direction = DirectionType(dir);
        if (IsScoringDirection(direction)) {
            Indices done_ortho;

            Cells::ConstIterator i_cell;
            for (i_cell = cells.begin(); i_cell != cells.end(); i_cell++) {
                IndexType const ortho = i_cell->Ortho(direction);
                if (!done_ortho.Contains(ortho)) {
                    result += ScoreDirection(*i_cell, direction);        
                    done_ortho.Add(ortho);
                }
            }
        }
    }

    return result;
}

void Board::UnplayMove(Move const &rMove) {
    Move::ConstIterator i_place;
    for (i_place = rMove.Begin(); i_place != rMove.End(); i_place++) {
		ASSERT(!i_place->IsSwap());
		Cell const cell = Cell(*i_place);
        MakeEmpty(cell);
    }
}


// inquiry methods

bool Board::AreAllCompatible(Cells const &rCells, DirectionType direction) const {
    bool result = true;
    
    Indices done_orthos;
        
    Cells::ConstIterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        IndexType const ortho = i_cell->Ortho(direction);
        if (!done_orthos.Contains(ortho)) {
            if (!IsDirectionCompatible(*i_cell, direction)) {
                result = false;
                break;
            }
            done_orthos.Add(ortho);
        }
    }
    
    return result; 
}


bool Board::AreAllCompatible(Cells const &rCells) const {
    bool result = true;

    if (rCells.Count() > 1) {
        Tiles const tiles = GetTiles(rCells);
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

bool Board::Contains(Tile const &rTile) const {
	Cell cell;
	TileIdType const id = rTile.Id();
	bool const result = LocateTileId(id, cell);

	return result;
}

bool Board::ContainsId(TileIdType id) const {
	Cell cell;
	bool const result = LocateTileId(id, cell);

	return result;
}

bool Board::DoesAnyHaveNeighbor(Cells const &rCells) const {
    bool result = false;
    
    Cells::ConstIterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
		Cell const cell = *i_cell;
        if (HasNeighbor(cell)) {
            result = true;
            break;
        }
    }

    return result;
}

bool Board::HasEmptyCell(Cell const &rCell) const {
    Tile const *const p_tile = GetCell(rCell);
    bool const result = (p_tile == NULL);

    return result;
}

bool Board::HasNeighbor(Cell const &rCell) const {
    bool result = false;
    
    for (int i_dir = DIRECTION_FIRST; i_dir <= DIRECTION_LAST; i_dir++) {
	    DirectionType direction = DirectionType(i_dir);
		if (rCell.HasNeighbor(direction)) {
            Cell const look(rCell, direction);
			ASSERT(look.IsValid());
            if (!HasEmptyCell(look)) {
                result = true;
                break;
            }
		}
    }
    
    return result;
}

bool Board::IsConnectedDirection(Cells const &rCells, DirectionType direction) const {
	 ASSERT(IsScoringDirection(direction));
     bool result = true;
    
    if (rCells.Count() > 1) {
        Cells::ConstIterator i_cell = rCells.begin();
        Cell first_cell = *i_cell;
        Cell last_cell = *i_cell;
        for (i_cell++ ; i_cell != rCells.end(); i_cell++) {
			Cell const cell = *i_cell;
			ASSERT(cell != first_cell);
            if (first_cell.Ortho(direction) != cell.Ortho(direction)) {
                return false;
			}
			ASSERT(cell.Group(direction) != first_cell.Group(direction));
            if (cell.Group(direction) < first_cell.Group(direction)) {
                first_cell = *i_cell;
            }

			ASSERT(cell != last_cell);
            ASSERT(last_cell.Ortho(direction) == cell.Ortho(direction));
			ASSERT(cell.Group(direction) != last_cell.Group(direction));
            if (cell.Group(direction) > last_cell.Group(direction)) {
                last_cell = *i_cell;
            }
        }
        
        for (Cell cell = first_cell; cell != last_cell; cell.Next(direction)) {
            if (HasEmptyCell(cell)) {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool Board::IsDirectionCompatible(Cell const &rCell, DirectionType direction) const {
    ASSERT(!HasEmptyCell(rCell));
	ASSERT(IsScoringDirection(direction));
    
    Cell first_cell, last_cell;
    GetLimits(rCell, direction, first_cell, last_cell);
    Cell end_cell(last_cell, direction);

    bool result = true;
    
    for (Cell cell1 = first_cell; cell1 != last_cell; cell1.Next(direction)) {
        Tile const t1 = GetTile(cell1);
        for (Cell cell2(cell1, direction); cell2 != end_cell; cell2.Next(direction)) {
            Tile const t2 = GetTile(cell2);
            if (!t1.IsCompatibleWith(&t2)) {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

bool Board::IsEmpty(void) const {
	bool const result = (Count() == 0);

	return result;
}

/* static */ bool Board::IsScoringDirection(DirectionType direction) {
     bool result = false;
     switch (Cell::Grid()) {
         case GRID_TRIANGLE:
             result = (direction != DIRECTION_NORTH 
                    && direction != DIRECTION_SOUTH);
             break;
         case GRID_4WAY:
             result = (direction == DIRECTION_NORTH 
                    || direction == DIRECTION_EAST
                    || direction == DIRECTION_SOUTH
                    || direction == DIRECTION_WEST);
             break;
         case GRID_HEX:
             result = (direction != DIRECTION_EAST
                    && direction != DIRECTION_WEST);
             break;
         case GRID_8WAY:
             result = true;
             break;
         default:
             FAIL();
     }
     
     return result;
}

bool Board::IsValidMove(Move const &rMove) const {
	char const *reason;
	bool const result = IsValidMove(rMove, reason);
	// forget the reason

	return result;
}

bool Board::IsValidMove(Move const &rMove, char const *&rReason) const {
    // a pass (no tiles played or swapped) is always valid
    if (rMove.IsPass()) {
        return true;
    }

    // check for repeated tiles
    if (rMove.RepeatsTile()) {
		rReason = "REPEATTILE";
        return false;
    }

	if (rMove.InvolvesSwap()) {
		if (rMove.IsResign()) {
            // a valid resignation
            return true;
		} else if (!rMove.IsPureSwap()) {
   		    rReason = "SWAP";
            return false;
		}
        // a valid swap
        return true;
	}

	// check for repeated cells
    if (rMove.RepeatsCell()) {
		rReason = "REPEATCELL";
        return false;
    }

    // get the set of board cells to be played
    Cells cells = Cells(rMove);

    // make sure all those cells are empty
    if (!AreAllEmpty(cells)) {
		rReason = "EMPTY";
        return false;
    }

    DirectionType direction_of_play = DIRECTION_UNKNOWN;
    if (cells.Count() > 1) {
        // make sure the cells lie in a single ortho
        for (int dir = DIRECTION_FIRST; 
             dir <= DIRECTION_LAST_POSITIVE;
             dir++)
        {
            DirectionType const direction = DirectionType(dir);
			if (IsScoringDirection(direction)) {
                if (cells.AreAllInSameOrtho(direction)) {
                    direction_of_play = direction;
                }
			}
        }
        if (direction_of_play == DIRECTION_UNKNOWN) {
		    rReason = "ROWCOLUMN";
            return false;
        }
    }

	if (IsEmpty()) {
        if (!cells.IsAnyStart()) {
	        rReason = "START";
	        return false;
        }
    } else if (!DoesAnyHaveNeighbor(cells)) {
		rReason = "NEIGHBOR";
        return false;   
    }

    // make a copy of the board and place the tiles on it
    Board after(*this);
    after.PlayMove(rMove);

    if (cells.Count() > 1) {
        // make sure there are no empty squares between played tiles
        if (!after.IsConnectedDirection(cells, direction_of_play)) {
     		rReason = "GAP";
            return false;
        }
    }
    
    // check compatibility of connected tiles in each group played
    for (int dir = DIRECTION_FIRST;
             dir <= DIRECTION_LAST_POSITIVE;
             dir++)
    {
        DirectionType direction = DirectionType(dir);
        if (IsScoringDirection(direction)) {
            if (!after.AreAllCompatible(cells, direction)) {
                switch (direction) {
                    case DIRECTION_NORTH:
		                rReason = "COLUMNCOMPAT";
                        return false;

                    case DIRECTION_EAST:
		                rReason = "ROWCOMPAT";
                        return false;

                    case DIRECTION_NORTHEAST:
                    case DIRECTION_SOUTHEAST:
		                rReason = "DIAGCOMPAT";
                        return false;

                    default:
                        FAIL();
                }
            }
        }
    }

    return true;
}
