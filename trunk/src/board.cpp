// File:     board.cpp
// Location: src
// Purpose:  implement Board class
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

#include <iostream>
#include "board.hpp"
#include "cells.hpp"
#include "direction.hpp"
#include "move.hpp"
#include "tiles.hpp"


// lifecycle

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.

// convert the entire board to a string
Board::operator String(void) const {
	unsigned const width = Tile::AttributeCnt() + 4;

    String result(width, ' ');
    for (IndexType column = -WestMax(); column <= EastMax(); column++) {
       String const column_tag(column);
       result += String(width - column_tag.Length(), ' ');
       result += column_tag;
    }
    result += "\n";
    for (IndexType row = NorthMax(); row >= -SouthMax(); row--) {
        String const row_tag(row);
        result += String(width - row_tag.Length(), ' ');
        result += row_tag;
	    for (IndexType column = -WestMax(); column <= EastMax(); column++) {
			Cell const cell(row, column);
			if (HasEmptyCell(cell)) {
				result += " .";
                result += Tile::StringEmpty();
                result += ".";
			} else {
                Tile const tile = GetTile(cell);
				result += " [";
                result += String(tile);
                result += "]";
			}
		}
		result += "\n";
	}

	return result;
}


// misc methods

Cell Board::FirstCell(void) const {
	int const column_fringe = 1;
	int const row_fringe = Cell::RowFringe();
    IndexType const top_row = row_fringe + NorthMax();
    IndexType const left_column = -column_fringe - WestMax();
	Cell const result(top_row, left_column);

	return result;
}

TileIdType Board::GetId(Cell const& rCell) const {
    TileIdType result = Tile::ID_NONE;
    Tile const* const p_tile = GetCell(rCell);
	if (p_tile != NULL) {
		result = p_tile->Id();
	}

	return result;
}

long Board::GetLimits(
	Cell const& rCell,
	Direction const& rDirection,
	Cell& rFirst,
	Cell& rLast) const
{
	ASSERT(Cell::IsScoringAxis(rDirection));
	ASSERT(rCell.IsValid());
    ASSERT(!HasEmptyCell(rCell));
	long result = 1;

	// look in the negative direction; stop at first invalid or empty cell
    rFirst = rCell;
    for (;;) {
        Cell const previous(rFirst, rDirection, -1);
        if (!previous.IsValid() || HasEmptyCell(previous)) {
			break;
		}
		rFirst = previous;
		++result;
	}
    
	// look in the positive direction; stop at first invalid or empty cell
    rLast = rCell;
	for (;;) {
        Cell const next(rLast, rDirection);
        if (!next.IsValid() || HasEmptyCell(next)) {
			break;
		}
		rLast = next;
		++result;
	}

	return result;
}

Tile Board::GetTile(Cell const& rCell) const {
	ASSERT(rCell.IsValid());

    Tile const* p_tile = GetCell(rCell);
    ASSERT(p_tile != NULL);

    Tile const result = *p_tile;

    return result;
}

Tiles Board::GetTiles(Cells const& rCells) const {
    Tiles result;
    
    Cells::const_iterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {    
        Tile const* p_tile = GetCell(*i_cell);
        if (p_tile != NULL) {
            result.Add(*p_tile);
        }
    }

    return result;
}

bool Board::LocateTile(Tile const& rTile, Cell& rCell) const {
	TileIdType const id = rTile.Id();
	bool const result = BaseBoard::LocateTileId(id, rCell);

	return result;
}

void Board::Next(Cell& rCell) const {
	int const column_fringe = 1;
    IndexType const right_column = column_fringe + EastMax();

	IndexType row = rCell.Row();
	IndexType column = rCell.Column();
	if (column < right_column) {
		column++;
	} else {
        IndexType const left_column = -column_fringe - WestMax();
		column = left_column;
		row--;
	}
	rCell = Cell(row, column);
}

void Board::PlayMove(Move const& rMove) {
    Move::ConstIterator i_place;
    for (i_place = rMove.Begin(); i_place != rMove.End(); i_place++) {
        PlayTile(*i_place);
    }
}

void Board::PlayTile(TileCell const& rTileCell) {
    Cell const cell = Cell(rTileCell);
    Tile const tile = rTileCell.operator Tile();
    ASSERT(HasEmptyCell(cell));
    PlayOnCell(cell, tile);
}

unsigned Board::ScoreDirection(
    Cell const& rCell,
    Direction const& rDirection) const
{
    ASSERT(!HasEmptyCell(rCell));
    
    Cell first_cell;
	Cell last_cell;
    unsigned const length = GetLimits(rCell, rDirection, first_cell, last_cell);

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
            i_cell.Next(rDirection);
		}

		// special bonus for two-attribute games
		if (Tile::AttributeCnt() == 2) {
		    // determine the common attribute
            Tile const first_tile = GetTile(first_cell);
            Tile const last_tile = GetTile(last_cell);
            AttrIndexType const common_attr = first_tile.CommonAttribute(last_tile);
			AttrIndexType const other_attr = 1 - common_attr;
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

/* static */ String Board::ReasonMessage(UmType reason, String& rTitle) {
	String message;
	rTitle = "Information";

	switch(reason) {
	    case UM_COLUMNCOMPAT:
		    message = "Tiles in a column (with no intervening empty cells) must all be mutually compatible.";
		    rTitle = "Column Compatibility Rule";
			break;
		case UM_DIAGCOMPAT:
		    message = "Tiles on a diagonal (with no intervening empty cells) must all be mutually compatible.";
		    rTitle = "Diagonal Compatibility Rule";
			break;
		case UM_EMPTY:
		    message = "That cell has already been used.";
		    rTitle = "Empty Cell Rule";
			break;
		case UM_FIRST:
		    message = "On the first turn, you must play as many tiles as possible.  Keep looking!";
		    rTitle = "First Turn Rule";
			break;
		case UM_GAP:
		    message = "You can't leave any empty cells between the tiles you play.";
		    rTitle = "Gap Rule";
			break;
		case UM_NEIGHBOR:
		    message = "Each cell you use must be a neighbor of a used cell.";
		    rTitle = "Neighbor Rule";
			break;
		case UM_REPEATCELL:
		    message = "You can't use the same cell twice.";
		    rTitle = "Repeated Cell Rule";
			break;
		case UM_REPEATTILE:
		    message = "You can't use the same tile twice.";
		    rTitle = "Repeated Tile Rule";
			break;
		case UM_ROWCOLUMN: {
		    String const dirs = Cell::ScoringAxes();
		    message = String("The cells you use must all lie in a single ") + dirs + ".";
		    rTitle = "Row/Column Rule";
			break;
		}
		case UM_ROWCOMPAT:
		    message = "Tiles in a row (with no intervening empty cells) must all be mutually compatible.";
		    rTitle = "Row Compatibility Rule";
			break;
		case UM_RULES:
		    message = "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing";
		    rTitle = "Rules";
			break;
		case UM_START:
		    message = "Your first tile must use the start cell. To change this tile, you must take back ALL your tiles.";
		    rTitle = "Start Rule";
	        break;
		case UM_STARTSIMPLE:
		    message = "Your first tile must use the start cell.";
		    rTitle = "Start Rule";
			break;
		case UM_STOCK:
		    message = "You can't swap more tiles than the number remaining in the stock bag.";
		    rTitle = "Stock Rule";
		    break;
		case UM_SWAP:
		    message = "You can play tiles or swap them, but you can't do both in the same turn.";
		    rTitle = "Swap Rule";
		    break;
		default:
			FAIL();
	}

    return message;
}

unsigned Board::ScoreMove(Move const& rMove) const {
    unsigned result = 0;
    
    Cells const cells = Cells(rMove);
    
	Direction axis;
    for (axis.SetFirst(); axis.IsAxis(); axis++) {
        if (Cell::IsScoringAxis(axis)) {
            Indices done_ortho;

            Cells::ConstIterator i_cell;
            for (i_cell = cells.begin(); i_cell != cells.end(); i_cell++) {
                IndexType const ortho = i_cell->Ortho(axis);
                if (!done_ortho.Contains(ortho)) {
                    result += ScoreDirection(*i_cell, axis);        
                    done_ortho.Add(ortho);
                }
            }
        }
    }

    return result;
}

void Board::UnplayMove(Move const& rMove) {
    Move::ConstIterator i_place;
    for (i_place = rMove.Begin(); i_place != rMove.End(); i_place++) {
		ASSERT(!i_place->IsSwap());
		Cell const cell = Cell(*i_place);
        MakeEmpty(cell);
    }
}


// inquiry methods

bool Board::AreAllCompatible(Cells const& rCells, Direction const& rAxis) const {
	ASSERT(Cell::IsScoringAxis(rAxis));

    bool result = true;
    
    Indices done_orthos;
        
    Cells::ConstIterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        IndexType const ortho = i_cell->Ortho(rAxis);
        if (!done_orthos.Contains(ortho)) {
            if (!IsAxisCompatible(*i_cell, rAxis)) {
                result = false;
                break;
            }
            done_orthos.Add(ortho);
        }
    }
    
    return result; 
}


bool Board::AreAllCompatible(Cells const& rCells) const {
    bool result = true;

    if (rCells.Count() > 1) {
        Tiles const tiles = GetTiles(rCells);
        if (!tiles.AreAllCompatible()) {
            result = false;
        }
    }
    
    return result;
}

bool Board::AreAllEmpty(Cells const& rCells) const {
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

bool Board::Contains(Tile const& rTile) const {
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

bool Board::DoesAnyHaveNeighbor(Cells const& rCells) const {
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

bool Board::HasEmptyCell(Cell const& rCell) const {
    Tile const* const p_tile = GetCell(rCell);
    bool const result = (p_tile == NULL);

    return result;
}

bool Board::HasNeighbor(Cell const& rCell) const {
    bool result = false;
    
	Direction direction;
    for (direction.SetFirst(); direction.IsValid(); direction++) {
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

bool Board::IsAxisCompatible(Cell const& rCell, Direction const& rAxis) const {
    ASSERT(!HasEmptyCell(rCell));
	ASSERT(rCell.IsValid());
	ASSERT(Cell::IsScoringAxis(rAxis));
    
    Cell first_cell;
	Cell last_cell;
    GetLimits(rCell, rAxis, first_cell, last_cell);

    bool result = true;
    
    for (Cell cell1 = first_cell; result && cell1 != last_cell; cell1.Next(rAxis)) {
        Tile const tile1 = GetTile(cell1);
        for (Cell cell2(cell1, rAxis); cell2 != last_cell; cell2.Next(rAxis)) {
            Tile const tile2 = GetTile(cell2);
            if (!tile1.IsCompatibleWith(&tile2)) {
                result = false;
                break;
            }
        }
		if (result) {
            Tile const last_tile = GetTile(last_cell);
            if (!tile1.IsCompatibleWith(&last_tile)) {
                result = false;
            }
		}
    }
    
    return result;
}

bool Board::IsConnectedAxis(Cells const& rCells, Direction const& rAxis) const {
	 ASSERT(Cell::IsScoringAxis(rAxis));
     bool result = true;
    
    if (rCells.Count() > 1) {
        Cells::ConstIterator i_cell = rCells.begin();
        Cell first_cell = *i_cell;
        Cell last_cell = *i_cell;
        for (i_cell++ ; i_cell != rCells.end(); i_cell++) {
			Cell const cell = *i_cell;
			ASSERT(cell != first_cell);
            if (first_cell.Ortho(rAxis) != cell.Ortho(rAxis)) {
                return false;
			}
			ASSERT(cell.Group(rAxis) != first_cell.Group(rAxis));
            if (cell.Group(rAxis) < first_cell.Group(rAxis)) {
                first_cell = *i_cell;
            }

			ASSERT(cell != last_cell);
            ASSERT(last_cell.Ortho(rAxis) == cell.Ortho(rAxis));
			ASSERT(cell.Group(rAxis) != last_cell.Group(rAxis));
            if (cell.Group(rAxis) > last_cell.Group(rAxis)) {
                last_cell = *i_cell;
            }
        }
        
        for (Cell cell = first_cell; cell != last_cell; cell.Next(rAxis)) {
            if (HasEmptyCell(cell)) {
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

bool Board::IsValidMove(Move const& rMove) const {
	UmType reason;
	bool const result = IsValidMove(rMove, reason);
	// forget the reason

	return result;
}

bool Board::IsValidMove(Move const& rMove, UmType& rReason) const {
    // a pass (no tiles played or swapped) is always valid
    if (rMove.IsPass()) {
        return true;
    }

    // check for repeated tiles
    if (rMove.RepeatsTile()) {
		rReason = UM_REPEATTILE;
        return false;
    }

	if (rMove.InvolvesSwap()) {
		if (rMove.IsResign()) {
            // a valid resignation
            return true;
		} else if (!rMove.IsPureSwap()) {
   		    rReason = UM_SWAP;
            return false;
		}
        // a valid swap
        return true;
	}

	// check for repeated cells
    if (rMove.RepeatsCell()) {
		rReason = UM_REPEATCELL;
        return false;
    }

    // get the set of board cells to be played
    Cells const cells = Cells(rMove);

    // make sure all those cells are empty
    if (!AreAllEmpty(cells)) {
		rReason = UM_EMPTY;
        return false;
    }

    Direction axis_of_play;
    if (cells.Count() > 1) {
        // make sure the cells lie in a single ortho
		Direction axis;
        for (axis.SetFirst(); axis.IsAxis(); axis++) {
			if (Cell::IsScoringAxis(axis)) {
                if (cells.AreAllInSameOrtho(axis)) {
                    axis_of_play = axis;
                }
			}
        }
        if (!axis_of_play.IsValid()) {
		    rReason = UM_ROWCOLUMN;
            return false;
        }
    }

	if (IsEmpty()) {
        if (!cells.IsAnyStart()) {
	        rReason = UM_START;
	        return false;
        }
    } else if (!DoesAnyHaveNeighbor(cells)) {
		rReason = UM_NEIGHBOR;
        return false;   
    }

    // make a copy of the board and place the tiles on it
    Board after(*this);
    after.PlayMove(rMove);

    if (cells.Count() > 1) {
        // make sure there are no empty squares between played tiles
        if (!after.IsConnectedAxis(cells, axis_of_play)) {
     		rReason = UM_GAP;
            return false;
        }
    }
    
    // check compatibility of connected tiles in each group
	Direction axis;
    for (axis.SetFirst(); axis.IsAxis(); axis++) {
        if (Cell::IsScoringAxis(axis)) {
            if (!after.AreAllCompatible(cells, axis)) {
                if (axis.IsVertical()) {
	                rReason = UM_COLUMNCOMPAT;
                    return false;
				} else if (axis.IsHorizontal()) {
	                rReason = UM_ROWCOMPAT;
                    return false;
				} else if (axis.IsDiagonal()) {
	                rReason = UM_DIAGCOMPAT;
                    return false;
				} else {
                    FAIL();
                }
            }
        }
    }

    return true;
}

bool Board::MightUse(Cell const& rCell) const {
	int const column_fringe = 1;
	int const row_fringe = Cell::RowFringe();
    IndexType const top_row = row_fringe + NorthMax();
    IndexType const bottom_row = -row_fringe - SouthMax();
    IndexType const right_column = column_fringe + EastMax();
    IndexType const left_column = -column_fringe - WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	IndexType const row = rCell.Row();
	IndexType const column = rCell.Column();

	bool const result = (row >= bottom_row 
		              && row <= top_row
					  && column >= left_column 
					  && column <= right_column);

	return result;
}
