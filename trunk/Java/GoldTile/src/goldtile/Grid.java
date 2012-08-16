// File:     Grid.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Grid enum for the Gold Tile Game
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

public enum Grid {
    GRID_TRIANGLE (1, 1, 3),
    GRID_4WAY (1, 1, 4),
    GRID_HEX (1, 2, 6),
    GRID_8WAY (1, 1, 8);
    
    final public int columnFringe;
    final public int rowFringe;
    final public int wayCnt;

    // constructors
    
    private Grid(int cf, int rf, int ways) {
        columnFringe = cf;
        rowFringe = rf;
        wayCnt = ways;   
    }
    
    // methods
    
    public Shape getCellShape() {
        switch (this) {
            case GRID_TRIANGLE:
                return Shape.TRIANGLE;
            case GRID_4WAY:
                return Shape.SQUARE;
            case GRID_HEX:
                return Shape.HEXAGON;
            case GRID_8WAY:
                return Shape.SQUARE;
            default:
                throw new AssertionError(Cell.getGrid());
        }
    }

    public static Grid getDefault() {
        return GRID_4WAY;
    }

    public Direction[] getScoringAxes() {
        switch (this) {
            case GRID_TRIANGLE:
                return Direction.nonVerticalAxes;
            case GRID_4WAY:
                return Direction.nonDiagonalAxes;
            case GRID_HEX:
                return Direction.nonHorizontalAxes;
            case GRID_8WAY:
                return Direction.allAxes;
            default:
                throw new AssertionError(Cell.getGrid());
        }
    }
}