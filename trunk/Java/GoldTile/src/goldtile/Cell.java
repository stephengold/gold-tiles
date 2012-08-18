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
import java.util.Arrays;

public class Cell implements Comparable {
    // constants
    
    final public static int HEIGHT_MAX = 0x40000000; 
    final public static int HEIGHT_MIN = 4;
    final public static int WIDTH_MAX = HEIGHT_MAX; 
    final public static int WIDTH_MIN = HEIGHT_MIN;
    
    final public static String PREFIX = "(";
    final public static String SEPARATOR = ",";
    final public static String SUFFIX = ")";

    // per-instance fields (immutable)
    final private int column;
    final private int row;
    
    // static fields
    private static GameOpt gameOpt;
    
    // constructors
    
    public Cell() {
        column = 0;
        row = 0;
        assert isStart() : this;
    }

    /**
     * @param row the index of the row containing the cell
     * @param column the index of the column containing the cell
     */
    public Cell(int row, int column) {
        this.column = column;
        this.row = row;
        // caller should check validity
    }
        
    /*
     * Construct the next valid cell (not necessarily a neighbor) 
     * in a given direction from a base cell
     */
    public Cell(Cell base, Direction direction) {
        assert base != null;
        assert base.isValid() : base;
        assert direction != null;

        if (getGrid() == Grid.GRID_TRIANGLE) {
            final boolean oddFlag = base.isOdd();
            // TODO direction = direction.triangleNeighbor(oddFlag);
        }
        
        row = base.row + direction.rowOffset;
        column = base.column + direction.columnOffset;
    }
    
    // methods

    public static Cell chooseConsole(String alternative) {
        assert alternative != null;
 
        int row;
        for (;;) {
            Global.print("Enter a row number");
            if (!alternative.isEmpty()) {
                Global.print(" or ");
                Global.print(StringExt.quote(alternative));
            }
            Global.print(": ");
            String input = Global.readLine();
            if (input.equals(alternative)) {
                return null;
            }
            try {
                row = Integer.parseInt(input);
                break;
            } catch (NumberFormatException exception) {
                Global.print(StringExt.quote(input));
                Global.print(" is invalid input.\n");
            }
        }
        
        int column;
        for (;;) {
            Global.print("Enter a column number: ");
            String input = Global.readLine();
            try {
                column = Integer.parseInt(input);
                return new Cell(row, column);
            } catch (NumberFormatException exception) {
                Global.print(StringExt.quote(input));
                Global.print(" is invalid input.\n");
            }
        }
    }
    
    @Override
    public int compareTo(Object object) {
        final Cell other = (Cell)object;
        
        if (row == other.row) {
            return column - other.column;
        } else {
            return row - other.row;
        }
    }
    
    public static boolean doesBoardWrap() {
        assert gameOpt != null;
        
        return gameOpt.doesBoardWrap();
    }

    public boolean equals(Cell other) {
        if (other == null) {
            return false;
        }
        
        return row == other.row && 
               column == other.column;
    }
    
    public static int getBoardHeight() {
        assert gameOpt != null;
        
        return gameOpt.getBoardHeight();
    }

    public static int getBoardWidth() {
        assert gameOpt != null;
        
        return gameOpt.getBoardWidth();
    }

    public int getColumn() {
        return column;
    }
    
    public static Grid getGrid() {
        assert gameOpt != null;
        
        return gameOpt.getGrid();
    }
    
    public int getRow() {
        return row;
    }
    
    public static Direction[] getScoringAxes() {
        return getGrid().getScoringAxes();
    }
    
    public static Shape getShape() {
        return getGrid().getCellShape();
    }
    
    public boolean hasNeighbor(Direction direction) {
        assert isValid();
        assert direction != null;

        switch (getGrid()) {
            case GRID_TRIANGLE:
                if (direction == Direction.NORTH && isEven()) {
                    return false;
                } else if (direction == Direction.SOUTH && isOdd()) {
                    return false;
                } else if (direction.diagonalFlag) {
                    return false;
                }
                break;

            case GRID_4WAY:
                if (direction.diagonalFlag) {
                    return false;
                }
                break;
                
            case GRID_HEX:
                if (direction.horizontalFlag) {
                    return false;
                }
                break;

            case GRID_8WAY:
                break; // generally has neighbors in all eight directions
            
            default:
                throw new AssertionError(getGrid());
        }

        // Check for edges.
        int row = this.row + direction.rowOffset;
        int column = this.column + direction.columnOffset;

        if (doesBoardWrap()) {
            final int height = getBoardHeight();
            if (row >= height/2 || row < -height/2) {
                return false;
            }
            
            final int width = getBoardWidth();
            if (column >= width/2 || column < -width/2) {
                return false;
            }
        }

        return true;
    }
    
    final public boolean isEqual(Cell other) {
        return row == other.row && column == other.column;
    }
    
    public boolean isEven() {
        return Global.isEven(row + column);
    }
    
    public boolean isOdd() {
        return Global.isOdd(row + column);
    }

    static public boolean isScoringAxis(Direction direction) {
        final Direction[] scoringAxes = getScoringAxes();
        
        return Arrays.asList(scoringAxes).contains(direction);
    } 
    
    final public boolean isStart() {
        return row == 0 && column == 0;
    }
    
    public boolean isValid() {
        if (getGrid() == Grid.GRID_HEX && isOdd()) {
            return false;
        } else if (row >= getBoardHeight()/2 || row < -getBoardHeight()/2) {
            return false;
        } else if (column >= getBoardWidth()/2 || column < -getBoardWidth()/2) {
            return false;
        } else {
            return true;
        }
    }
    
    public static int limitPlay(int cellCnt) {
        assert cellCnt > 0;

        final int cellsNeeded = cellCnt;
        int mostFound = 1;

        for (Direction axis : getScoringAxes()) {
            int cellsFound = 1;
            
            Cell current = new Cell(); // start cell
            assert current.isValid();
            
            // look in the negative direction; stop at first invalid cell
            Direction direction = axis.getOpposite();
            while (cellsFound < cellsNeeded) {
                final Cell previous = new Cell(current, direction);
                if (!previous.isValid()) {
                    break;
                }
                ++cellsFound;
                current = previous;
            }

            current = new Cell(); // start cell again
            assert current.isValid();

            // look in the positive direction; stop at first invalid cell
            direction = axis;
            while (cellsFound < cellsNeeded) {
                final Cell next = new Cell(current, direction);
                if (!next.isValid()) {
                    break;
                }
                ++cellsFound;
                current = next;
            }

            if (cellsFound >= cellsNeeded) {
                return cellsNeeded;
            }
            if (cellsFound > mostFound) {
                mostFound = cellsFound;
            }
        }

        assert mostFound < cellsNeeded;
        return mostFound;
    }
       
    /**
     * @param gameOpt the current game options
     */
    public static void setStatic(GameOpt gameOpt) {
        assert gameOpt != null;
        
        Cell.gameOpt = new GameOpt(gameOpt);

        int height = getBoardHeight();
        assert Global.isEven(height) : height;
        assert height >= HEIGHT_MIN : height;
        assert height <= HEIGHT_MAX : height;
        
        int width = getBoardWidth();
        assert Global.isEven(width) : width;
        assert width >= WIDTH_MIN : width;
        assert width <= WIDTH_MAX : width;
    }
    
    @Override
    public String toString() {
        assert isValid();

        return String.format("%s%d%s%d%s",
                PREFIX, row, SEPARATOR, column, SUFFIX);
    }
    
    public Cell wrap() {
        int column = this.column;
        int row = this.row;
        
        if (doesBoardWrap()) {
            final int width = getBoardWidth();
            if (column >= 0) {
                final int numWraps = (column + width/2) / width;
                assert numWraps >= 0 : numWraps;
                column -= numWraps * width;
            } else {
                final int numWraps = (width/2 - column - 1) / width;
                assert numWraps >= 0 : numWraps;
                column += numWraps * width;
            }

            final int height = getBoardHeight();
            if (row >= 0) {
                final int numWraps = (row + height/2) / height;
                assert numWraps >= 0 : numWraps;
                row -= numWraps * height;
            } else {
                final int numWraps = (height/2 - row - 1) / height;
                assert numWraps >= 0 : numWraps;
                row += numWraps * height;
            }
        }

        final Cell result = new Cell(row, column);
        assert result.isValid() : result;
        
        return result;
    }
}
