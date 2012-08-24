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
    final public static String PREFIX = "(";
    final public static String SEPARATOR = ",";
    final public static String SEPARATOR_REGEX = "[,]";
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
        
    public Cell(String text)
        throws ParseException
    {
        assert text != null;
        
        final boolean hasPrefix = text.startsWith(PREFIX);
        final boolean hasSuffix = text.endsWith(SUFFIX);
        if (!hasPrefix || !hasSuffix) {
            throw new ParseException();
        }
        
        final int endIndex = text.length() - PREFIX.length();
        final String body = text.substring(PREFIX.length(), endIndex);
        final String[] parts = body.split(SEPARATOR_REGEX);
        if (parts.length != 2) {
            throw new ParseException();
        }
        
        final String first = parts[0];
        final String second = parts[1];
        
        row = Integer.parseInt(first);
        column = Integer.parseInt(second);
    }
    
    // methods, sorted by name

    public static Cell chooseConsole(String alternative) {
        assert alternative != null;
 
        int row;
        for (;;) {
            Console.print("Enter a row number");
            if (!alternative.isEmpty()) {
                Console.printf(" or %s",
                        StringExt.quote(alternative));
            }
            String input = Console.readLine(": ");
            if (input.equals(alternative)) {
                return null;
            }
            try {
                row = Integer.parseInt(input);
                break;
            } catch (NumberFormatException exception) {
                Console.printf("%s is not a valid number.\n", 
                        StringExt.quote(input));
            }
        }
        
        int column;
        for (;;) {
            String input = Console.readLine("Enter a column number: ");
            try {
                column = Integer.parseInt(input);
                return new Cell(row, column);
            } catch (NumberFormatException exception) {
                Console.printf("%s is not a valid number.\n", 
                        StringExt.quote(input));
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
    
    public static Dim getBoardHeight() {
        assert gameOpt != null;
        
        return gameOpt.getBoardHeight();
    }

    public static Dim getBoardWidth() {
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
            if (!getBoardHeight().isValidIndex(row)) {
                return false;
            }
            if (!getBoardWidth().isValidIndex(column)) {
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
        if (getGrid().isHex() && isOdd()) {
            return false;
        } else {
            return getBoardHeight().isValidIndex(row) && 
                getBoardWidth().isValidIndex(column);
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
        
        Cell.gameOpt = new GameOpt(gameOpt); // save a copy
    }
    
    @Override
    public String toString() {
        assert isValid();

        return String.format("%s%d%s%d%s",
                PREFIX, row, SEPARATOR, column, SUFFIX);
    }
    
    public Cell wrap() {
        assert isValid();
        
        if (doesBoardWrap()) {
            final int column = getBoardWidth().wrapIndex(this.column);
            final int row = getBoardWidth().wrapIndex(this.row);
            final Cell result = new Cell(row, column);
            
            assert result.isValid();
            return result;
        } else {
            return this;
        }
    }
}
