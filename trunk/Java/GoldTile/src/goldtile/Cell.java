// File:     Cell.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Cell class for the Gold Tile Game
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
public class Cell {
    private int column;
    private int row;
    
    // static fields
    
    final public static int HEIGHT_MAX = 0x40000000; 
    final public static int HEIGHT_MIN = 4;
    final public static int WIDTH_MAX = HEIGHT_MAX; 
    final public static int WIDTH_MIN = HEIGHT_MIN;
    
    public static Grid grid;
    private static int height;
    private static int width;
    public static boolean wrapFlag; 
    
    // constructors
    
    public Cell() {
        column = 0;
        row = 0;
    }

    /**
     * @param other the Cell to be replicated
     */
    public Cell(Cell other) {
        column = other.column;
        row = other.row;
    }
    
    /**
     * @param r the index of the row containing the cell
     * @param c the index of the column containing the cell
     */
    public Cell(int r, int c) {
        column = c;
        row = r;
    }
    
    // methods
    
    public boolean isOdd() {
        return Global.isOdd(row + column);
    }
    
    public boolean isStart() {
        return row == 0 && column == 0;
    }
    
    public boolean isValid() {
        boolean result = true;
        
        if (grid == Grid.GRID_HEX && isOdd()) {
            result = false;
        } else if (row >= height/2 || row < -height/2) {
            result = false;
        } else if (column >= width/2 || column < -width/2) {
            result = false;
        }
        
        return result;
    }
    
    /**
     * @param options the game options
     */
    public static void setStatic(GameOpt options) {
        grid = options.grid;
        
        height = options.boardHeight();
        assert Global.isEven(height);
        assert height >= HEIGHT_MIN;
        assert height <= HEIGHT_MAX;
        
        width = options.boardWidth();
        assert Global.isEven(width);
        assert width >= WIDTH_MIN;
        assert width <= WIDTH_MAX;
        
        wrapFlag = options.doesBoardWrap;
    }
}
