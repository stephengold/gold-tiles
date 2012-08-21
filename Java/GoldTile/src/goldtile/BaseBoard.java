// File:     BaseBoard.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  BaseBoard class for the Gold Tile Game
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

import java.util.Map;
import java.util.TreeMap;

public class BaseBoard {
    // fields
    
    private int eastMax = 0;
    private int northMax = 0;
    private int southMax = 0;
    private int westMax = 0;
    
    final private Map<Cell,Tile> cellMap;
    final private Map<Tile,Cell> tileMap;
            
    // constructors
    
    public BaseBoard() {
        cellMap = new TreeMap<>();
        tileMap = new TreeMap<>();
    }
    
    public BaseBoard(BaseBoard other) {
        assert other != null;
        
        eastMax = other.eastMax;   
        northMax = other.northMax;   
        southMax = other.southMax;   
        westMax = other.westMax;
        
        cellMap = new TreeMap(other.cellMap);
        tileMap = new TreeMap(other.tileMap);
        
        assert size() == other.size();
    }
    
    // methods, sorted by name
    
    public void clear() {
        eastMax = 0;
        northMax = 0;
        southMax = 0;
        westMax = 0;
        
        cellMap.clear();
        tileMap.clear();
        
        assert size() == 0 : size();
    }
    
    public void clear(Cell cell) {
        assert cell != null;
        final Tile tile = cellMap.get(cell);
        assert tile != null;
        assert tileMap.get(tile).equals(cell) : cell;
        
        cellMap.remove(cell);
        tileMap.remove(tile);
        
        // Shrink the limits as needed.
        final int column = cell.getColumn();
        while (column != 0 && isEmptyColumn(column)) {
            if (column == eastMax) {
                eastMax --;
            } else if (column == -westMax) {
                westMax --;
            } else {
                break;
            }
        }

        final int row = cell.getRow();
        while (row != 0 && isEmptyRow(row)) {
            if (row == northMax) {
                northMax --;
            } else if (column == -southMax) {
                southMax --;
            } else {
                break;
            }
        }
        
        assert cellMap.get(cell) == null;
        assert tileMap.get(tile) == null;
    }
    
    public Cell find(Tile tile) {
        if (tile == null) {
            return null;
        } else {
            return tileMap.get(tile);
        }
    }
    
    public Tile getContent(Cell cell) {
        if (cell == null) {
            return null;
        } else {
            return cellMap.get(cell);
        }
    }
    
    public int getEastMax() {
        return eastMax; 
    }
    
    public int getNorthMax() {
        return northMax; 
    }
    
    public int getSouthMax() {
        return southMax; 
    }
    
    public Tiles getTiles() {
        Tiles result = new Tiles();
        
        for (Tile tile : tileMap.keySet()) {
            result.add(tile);    
        }
        
        return result;
    }
    
    public int getWestMax() {
        return westMax; 
    }
    
    private boolean isEmptyColumn(int column) {
        for (int row = -southMax; row <= northMax; row++) {
            final Cell cell = new Cell(row, column);
            final Tile tile = cellMap.get(cell);
            if (tile != null) {
                return false;
            }
        }
        
        return true;    
    }

    private boolean isEmptyRow(int row) {
        for (int column = -westMax; column <= eastMax; column++) {
            final Cell cell = new Cell(row, column);
            final Tile tile = cellMap.get(cell);
            if (tile != null) {
                return false;
            }
        }
        
        return true;    
    }
    
    public void place(Cell cell, Tile tile) {
        assert cellMap.get(cell) == null : cellMap.get(cell);
        assert tileMap.get(tile) == null : tileMap.get(tile);
        
        // Grow the limits as needed.
        final int column = cell.getColumn();
        if (column > eastMax) {
            eastMax = column;
        }
        if (column < -westMax) {
            westMax = -column;
        }

        final int row = cell.getRow();
        if (row > northMax) {
            northMax = row;
        }
        if (row < -southMax) {
            southMax = -row;
        }
        
        cellMap.put(cell, tile);
        tileMap.put(tile, cell);
        
        assert cellMap.get(cell) == tile : tile;
        assert tileMap.get(tile) == cell : cell;
        assert size() > 0 : size();
    }
    
    final protected int size() {
        final int result = cellMap.size();
        
        assert tileMap.size() == result : result;
        return result;
    }
}
