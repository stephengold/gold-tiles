// File:     User.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  User class for the Gold Tile Game
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

import java.awt.Point;

public class User {
    // per-instance fields
    private boolean autopause = false;
    private boolean peek = false;
    private boolean showClocks = false;
    private boolean showGrid = false;
    private boolean showScores = false;
    private DisplayModes displayModes = new DisplayModes(Display.GUI);
    private int boardTileSize = MenuBar.BOARD_SIZE_DEFAULT;
    private int handTileSize = MenuBar.TILE_SIZE_DEFAULT;

    // logical coordinates of the Start cell    
    private Point startCellPosition = new Point(0, 0);  
    
    final private String name;

    // static fields
    private static java.util.Map<String,User> users = 
            new java.util.TreeMap<>();

    // constructors
    
    private User(String name) {
        this.name = name;
    }
    
    // methods
    
    public boolean getAutopause() {
        return autopause;    
    }
    
    public int getBoardTileSize() {
        return boardTileSize;
    }
    
    public DisplayModes getDisplayModes() {
        return displayModes;    
    }
    
    public int getHandTileSize() {
        return handTileSize;
    }
    
    public boolean getPeek() {
        return peek;
    }
    
    public boolean getShowClocks() {
        return showClocks;
    }
    
    public boolean getShowGrid() {
        return showGrid;
    }
    
    public boolean getShowScores() {
        return showScores;
    }
    
    public Point getStartCellPosition() {
        return startCellPosition;
    }
    
    public static User lookup(String name) {
        if (users.containsKey(name)) {
            return users.get(name);
        } else {
            final User result = new User(name);
            users.put(name, result);
            return result;
        }
    }
    
    public void setAutopause(boolean flag) {
        autopause = flag;
    }
    
    public void setBoardTileSize(int size) {
        boardTileSize = size;
    }
    
    public void setDisplayModes(DisplayModes displayModes) {
        this.displayModes = new DisplayModes(displayModes);
    }
    
    public void setHandTileSize(int size) {
        handTileSize = size;
    }
    
    public void setPeek(boolean flag) {
        peek = flag;
    }
    
    public void setShowClocks(boolean flag) {
        showClocks = flag;
    } 
    
    public void setShowGrid(boolean flag) {
        showGrid = flag;
    }
    
    public void setShowScores(boolean flag) {
        showScores = flag;
    }
    
    public void setStartCellPosition(Point position) {
        startCellPosition = position;
    }
}
