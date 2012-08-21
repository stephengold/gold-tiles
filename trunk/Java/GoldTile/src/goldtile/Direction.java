// File:     Direction.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Direction enum for the Gold Tile Game
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

public enum Direction {
    // values
    NORTH     (true,  +1,  0),
    NORTHEAST (true,  +1, +1),
    EAST      (true,   0, +1),
    SOUTHEAST (true,  -1, +1),
    SOUTH     (false, -1,  0),
    SOUTHWEST (false, -1, -1),
    WEST      (false,  0, -1),
    NORTHWEST (false, +1, -1) ;
    
    // per-instance fields (immutable), sorted by type
    
    final public boolean axisFlag;
    final public boolean diagonalFlag;
    final public boolean horizontalFlag;    
    final public boolean verticalFlag;
    
    final public int columnOffset;
    final public int rowOffset;
    
    // static fields
    final public static Direction[] allAxes 
            = { NORTH, NORTHEAST, EAST, SOUTHEAST };
    final public static Direction[] nonDiagonalAxes 
            = { NORTH, EAST };
    final public static Direction[] nonHorizontalAxes 
            = { NORTH, NORTHEAST, SOUTHEAST };
    final public static Direction[] nonVerticalAxes 
            = { NORTHEAST, EAST, SOUTHEAST };

    // constructors
    
    private Direction(boolean a, int rowOffset, int columnOffset) {
        axisFlag = a;
        diagonalFlag = (rowOffset != 0 && columnOffset != 0);
        horizontalFlag = (rowOffset == 0);
        verticalFlag = (columnOffset == 0);
        
        this.rowOffset = rowOffset;
        this.columnOffset = columnOffset;
    }
    
    // methods, sorted by name
    
    public Direction getOpposite() {
        switch(this) {
            case NORTH:
                return SOUTH;
            case NORTHEAST:
                return SOUTHWEST;
            case EAST:
                return WEST;
            case SOUTHEAST:
                return NORTHWEST;
            case SOUTH:
                return NORTH;
            case SOUTHWEST:
                return NORTHEAST;
            case WEST:
                return EAST;
            case NORTHWEST:
                return SOUTHEAST;
            default:
                throw new AssertionError(this);
        }
    }
    
    public Direction getOrthogonalAxis() {
        switch(this) {
            case NORTH:
            case SOUTH:
                return EAST;
            case NORTHEAST:
            case SOUTHWEST:
                return SOUTHEAST;
            case EAST:
            case WEST:
                return NORTH;
            case SOUTHEAST:
            case NORTHWEST:
                return NORTHEAST;
            default:
                throw new AssertionError(this);
        }        
    }    
}
