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

#include "board.hpp"
#include "cells.hpp"
#include "direction.hpp"
#include "tiles.hpp"


// lifecycle

// The compiler-generated default constructor is OK.
// The compiler-generated copy constructor is OK.
// The compiler-generated destructor is OK.


// operators

// The compiler-generated assignment method is OK.


// misc methods

// Convert the entire board to a string for console output.
String Board::Description(void) const {
	unsigned const width = Combo::AttributeCnt() + 4;

    String result(width, ' ');
    for (ColumnType column = -WestMax(); column <= EastMax(); column++) {
       String const column_tag(column);
       result += String(width - column_tag.Length(), ' ');
       result += column_tag;
    }
    result += "\n";
    for (RowType row = NorthMax(); row >= -SouthMax(); row--) {
        String const row_tag(row);
        result += String(width - row_tag.Length(), ' ');
        result += row_tag;
	    for (ColumnType column = -WestMax(); column <= EastMax(); column++) {
			Cell const cell(row, column);
			if (HasEmptyCell(cell)) {
                // fill with dots
                result += " " + String(width - 1, '.');
			} else {
                Tile const tile = GetTile(cell);
                String string = tile.Description();
                // pad with blanks
                if (string.Length() < width - 3) {
                    string += String(width - 3 - string.Length(), ' ');
                }
                result += " [" + string + "]";
			}
		}
		result += "\n";
	}

	return result;
}

Cell Board::FirstCell(void) const {
	int const column_fringe = 1;
	int const row_fringe = Cell::RowFringe();
    RowType const top_row = row_fringe + NorthMax();
    ColumnType const left_column = -column_fringe - WestMax();
	Cell const result(top_row, left_column);

	return result;
}

Cells Board::GetRun(
	Cell const& rCell,
	Direction const& rDirection) const
{
	ASSERT(Cell::IsScoringAxis(rDirection));
	ASSERT(rCell.IsValid());
    ASSERT(!HasEmptyCell(rCell));

	Cells result(rCell);

	// look both ways along the axis

	// look in the negative direction; stop at first invalid/empty/duplicate cell
    Cell current = rCell;
    for (;;) {
        Cell const previous(current, rDirection, -1);
        if (!previous.IsValid() 
		  || HasEmptyCell(previous) 
		  || result.Contains(previous))
		{
			break;
		}
		result.Add(previous);
		current = previous;
	}
    
	// look in the positive direction; stop at first invalid/empty/duplicate cell
    current = rCell;
	for (;;) {
        Cell const next(current, rDirection, +1);
        if (!next.IsValid()
		 || HasEmptyCell(next) 
		 || result.Contains(next)) 
		{
			break;
		}
		result.Add(next);
		current = next;
	}

	ASSERT(result.Count() > 0);
	return result;
}

Tile Board::GetTile(Cell const& rCell) const {
	ASSERT(rCell.IsValid());

    Tile::IdType result = Tile::ID_NONE;
    Tile const* const p_tile = GetCell(rCell);
    if (p_tile != NULL) {
        result = p_tile->Id();
    }

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

void Board::Next(Cell& rCell) const {
	int const column_fringe = 1;
    ColumnType const right_column = column_fringe + EastMax();

	RowType row = rCell.Row();
	ColumnType column = rCell.Column();
	if (column < right_column) {
		column++;
	} else {
        ColumnType const left_column = -column_fringe - WestMax();
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

ScoreType Board::ScoreDirection(
    Cell const& rCell,
    Direction const& rDirection) const
{
    ASSERT(!HasEmptyCell(rCell));
    
    Cells const cells = GetRun(rCell, rDirection);
	unsigned const length = cells.Count();

	ScoreType result = 0;
    if (length > 1) {
		result = ScoreType(length); // base score

		Tiles const tiles = GetTiles(cells);
		unsigned const bonus_factor = tiles.BonusFactor();
        result *= bonus_factor;

		// special bonus for two-attribute games
		if (Combo::AttributeCnt() == 2) {
		    // determine the common attribute
            AttrIndexType const common_attr = tiles.CommonAttribute();
			AttrIndexType const other_attr = 1 - common_attr;
            unsigned const max_length = Combo::ValueCnt(other_attr);

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
		    message = String("The cells you use must all lie in a single ") 
				    + dirs + " with no empty cells between them.";
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

ScoreType Board::ScoreMove(Move const& rMove) const {
    ScoreType result = 0;
    
    Cells const cells = rMove;
    
	Direction axis;
    for (axis.SetFirst(); axis.IsAxis(); axis++) {
        if (Cell::IsScoringAxis(axis)) {
            Cells done_cells;

            Cells::ConstIterator i_cell;
            for (i_cell = cells.begin(); i_cell != cells.end(); i_cell++) {
				Cell const cell = *i_cell;
                if (!done_cells.Contains(cell)) {
                    result += ScoreDirection(cell, axis);
				    Cells const run_cells = GetRun(cell, axis);
                    done_cells.AddCells(run_cells);
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

bool Board::AreAllRunsCompatible(Cells const& rCells, Direction const& rAxis) const {
	ASSERT(Cell::IsScoringAxis(rAxis));

    bool result = true;    
    Cells done_cells;
        
    Cells::ConstIterator i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        Cell const cell = *i_cell;
        if (!done_cells.Contains(cell)) {
            if (!IsRunCompatible(*i_cell, rAxis)) {
                result = false;
                break;
            }
			Cells const run_cells = GetRun(cell, rAxis);
            done_cells.AddCells(run_cells);
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

bool Board::AreSingleConnectedRun(Cells const& rCells, Direction const& rAxis) const {
	ASSERT(Cell::IsScoringAxis(rAxis));

    bool result = true;
    
    if (rCells.Count() > 1) {
		Cell const sample = rCells.First();
		Cells const run = GetRun(sample, rAxis);
		result = run.ContainsAll(rCells);
    }

    return result;
}

bool Board::Contains(Tile::IdType id) const {
	Cell cell;
    bool const result = LocateTile(id, cell);

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
            Cell const look(rCell, direction, +1);
			ASSERT(look.IsValid());
            if (!HasEmptyCell(look)) {
                result = true;
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

bool Board::IsRunCompatible(Cell const& rCell, Direction const& rAxis) const {
    ASSERT(!HasEmptyCell(rCell));
	ASSERT(rCell.IsValid());
	ASSERT(Cell::IsScoringAxis(rAxis));
    
    Cells const run_cells = GetRun(rCell, rAxis);
    bool const result = AreAllCompatible(run_cells);
    
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

    Direction axis_of_play;
    if (cells.Count() > 1) {
        // make sure all the cell used lie in a single connected run
		Direction axis;
        for (axis.SetFirst(); axis.IsAxis(); axis++) {
			if (Cell::IsScoringAxis(axis)) {
                if (after.AreSingleConnectedRun(cells, axis)) {
                    axis_of_play = axis;
                }
			}
        }
        if (!axis_of_play.IsValid()) {
		    rReason = UM_ROWCOLUMN;
            return false;
        }
    }
    
    // check compatibility of all runs involving these tiles
	Direction axis;
    for (axis.SetFirst(); axis.IsAxis(); axis++) {
        if (Cell::IsScoringAxis(axis)) {
            if (!after.AreAllRunsCompatible(cells, axis)) {
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
    RowType const top_row = row_fringe + NorthMax();
    RowType const bottom_row = -row_fringe - SouthMax();
    ColumnType const right_column = column_fringe + EastMax();
    ColumnType const left_column = -column_fringe - WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	RowType const row = rCell.Row();
	ColumnType const column = rCell.Column();

	bool const result = (row >= bottom_row 
		              && row <= top_row
					  && column >= left_column 
					  && column <= right_column);

	return result;
}
