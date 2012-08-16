// File:     Board.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Board class for the Gold Tile Game
/**
 * @author Stephen Gold
 */
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

package goldtile;

public class Board extends BaseBoard {
    // constructors
    
    public Board() {
        super();
    }
    
    public Board(BaseBoard other) {
        super(other);
    }
    
    // methods
    
    public int getBottomUseRow() {
        final Grid grid = Cell.getGrid();
        return -(grid.rowFringe + getSouthMax());     
    }

    public int getLeftUseColumn() {
        final Grid grid = Cell.getGrid();
        return -(grid.columnFringe + getWestMax());     
    }

    public int getRightUseColumn() {
        final Grid grid = Cell.getGrid();
        return grid.columnFringe + getEastMax();     
    }

    public int getTopUseRow() {
        final Grid grid = Cell.getGrid();
        return grid.rowFringe + getSouthMax();     
    }
    
    public boolean mightUse(Cell cell) {
        final int column = cell.getColumn();        
        if (column < getLeftUseColumn()) {
            return false;
        } else if (column > getRightUseColumn()) {
            return false;
        }
        
        final int row = cell.getRow();
        if (row < getBottomUseRow()) {
            return false;
        } else if (row > getTopUseRow()) {
            return false;
        }
        
        return true;
    }
}
