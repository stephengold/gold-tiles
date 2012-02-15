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
    Tile const *p_tile = GetCell(rCell);
	if (p_tile != NULL) {
		result = p_tile->Id();
	}

	return result;
}

void Board::GetLimits(
	Cell const &rCell,
	DirectionType direction,
	Cell &rFirst,
	Cell &rLast) const
{
    ASSERT(!HasEmptyCell(rCell));
    
    rFirst = rCell;
    while (!HasEmptyCell(rFirst)) {
        // TODO edge effects
        rFirst.Next(direction, -1);
    }
    rFirst.Next(direction, +1);
    
    rLast = rCell;
    while (!HasEmptyCell(rLast)) {
        rLast.Next(direction, +1);
    }
    rLast.Next(direction, -1);
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
    ASSERT(!HasEmptyCell(rCell));
    
    Cell first_cell, last_cell;
    GetLimits(rCell, direction, first_cell, last_cell);

	unsigned result = 0;
    if (first_cell != last_cell) {
        unsigned length = first_cell.Distance(last_cell, direction) + 1;
        ASSERT(length > 1);
        Tile first_tile = GetTile(first_cell);
        Tile last_tile = GetTile(last_cell);
        AIndexType attr = first_tile.CommonAttribute(last_tile);
        unsigned max_length = 1 + Tile::ValueMax(attr);
        if (length == max_length) {
            result = 2*length;
        } else {
            ASSERT(length < max_length);
            result = length;
        }
    }

    return result;
}

unsigned Board::ScoreMove(Move const &rMove) const {
    unsigned result = 0;
    
    Cells cells = Cells(rMove);
    
    for (int dir = DIRECTION_FIRST; 
             dir <= DIRECTION_LAST_POSITIVE;
             dir++)
    {
        DirectionType direction = DirectionType(dir);
        if (::is_scoring_direction(direction)) {
            DirectionType ortho = ::ortho_direction(direction);
            Indices done_group;

            Cells::ConstIteratorType i_cell;
            for (i_cell = cells.begin(); i_cell != cells.end(); i_cell++) {
                IndexType group = i_cell->Group(ortho);
                if (!done_group.Contains(group)) {
                    result += ScoreDirection(*i_cell, direction);        
                    done_group.Add(group);
                }
            }
        }
    }

    return result;
}


// inquiry methods

bool Board::AreAllCompatible(Cells const &rCells, DirectionType direction) const {
    bool result = true;
    
    Indices done_orthos;
        
    Cells::ConstIteratorType i_cell;
    for (i_cell = rCells.begin(); i_cell != rCells.end(); i_cell++) {
        IndexType ortho_group = i_cell->Group(::ortho_direction(direction));
        if (!done_orthos.Contains(ortho_group)) {
            if (!IsDirectionCompatible(*i_cell, direction)) {
                result = false;
                break;
            }
            done_orthos.Add(ortho_group);
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

bool Board::HasEmptyCell(Cell const &rCell) const {
    Tile const *p_tile = GetCell(rCell);
    bool result = (p_tile == NULL);

    return result;
}

bool Board::IsConnectedDirection(Cells const &rCells, DirectionType direction) const {
     bool result = true;
    
    if (rCells.Count() > 1) {
        Cells::ConstIteratorType i_cell = rCells.begin();
       
        DirectionType ortho = ::ortho_direction(direction);
        
        Cell first_cell = *i_cell;
        Cell last_cell = *i_cell;
        for (i_cell++ ; i_cell != rCells.end(); i_cell++) {
			Cell cell = *i_cell;
            if (first_cell.Group(ortho) != cell.Group(ortho)) {
                return false;
            }            
            if (cell.Group(direction) > last_cell.Group(direction)) {
                last_cell = *i_cell;
            }
            if (cell.Group(direction) < first_cell.Group(direction)) {
                first_cell = *i_cell;
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
	ASSERT(::is_scoring_direction(direction));
    
    Cell first_cell, last_cell;
    GetLimits(rCell, direction, first_cell, last_cell);
    Cell end_cell(last_cell, direction, 1);

    bool result = true;
    
    for (Cell cell1 = first_cell; cell1 != last_cell; cell1.Next(direction)) {
        Tile t1 = GetTile(cell1);
        for (Cell cell2(cell1, direction); cell2 != end_cell; cell2.Next(direction)) {
			ASSERT(cell1.Group(::ortho_direction(direction)) 
				== cell2.Group(::ortho_direction(direction)));
            Tile t2 = GetTile(cell2);
            if (!t1.IsCompatibleWith(&t2)) {
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

    DirectionType direction_of_play = DIRECTION_UNKNOWN;
    if (cells.Count() > 1) {
        // make sure the cells lie in a single group
        for (int dir = DIRECTION_FIRST; 
             dir <= DIRECTION_LAST_POSITIVE;
             dir++)
        {
            DirectionType direction = DirectionType(dir);
			if (::is_scoring_direction(direction)) {
			    DirectionType ortho = ::ortho_direction(direction);
                if (cells.AreAllInSameGroup(ortho)) {
                    direction_of_play = direction;
                }
			}
        }
        if (direction_of_play == DIRECTION_UNKNOWN) {
            D(std::cout << "The cells you use must all lie in a single row, " 
                        << "column, or diagonal." << std::endl);
		    rReason = "ROWCOLUMN";
            return false;
        }
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

    if (cells.Count() > 1) {
        // make sure there are no empty squares between played tiles
        if (!after.IsConnectedDirection(cells, direction_of_play)) {
            D(std::cout << "You must not leave any empty cells between the tiles you play." 
				<< std::endl);
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
        if (::is_scoring_direction(direction)) {
            DirectionType ortho = ::ortho_direction(direction);
            if (!after.AreAllCompatible(cells, direction)) {
                switch (direction) {
                    case DIRECTION_NORTH:
                        D(std::cout << "Tiles in a column (with no intervening "
                                    << "empty cells) must all be mutually "
                                    << "compatible." << std::endl);
		                rReason = "COLUMNCOMPAT";
                        return false;

                    case DIRECTION_EAST:
                        D(std::cout << "Tiles in a row (with no intervening "
                                    << "empty cells) must all be mutually "
                                    << "compatible." << std::endl);
		                rReason = "ROWCOMPAT";
                        return false;

                    case DIRECTION_NORTHEAST:
                    case DIRECTION_SOUTHEAST:
                        D(std::cout << "Tiles on a diagonal (with no intervening "
                                    << "empty cells) must all be mutually "
                                    << "compatible." << std::endl);
		                rReason = "DIAGCOMPAT";
                        return false;

                    default:
                        ASSERT(false);
                }
            }
        }
    }

    D(std::cout << "The move is a valid play." << std::endl);
    return true;
}
