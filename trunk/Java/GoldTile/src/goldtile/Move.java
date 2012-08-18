// File:     Move.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Move class for the Gold Tile Game
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
import java.util.TreeSet;

public class Move {
    // constants
    final private static String PREFIX = "move{";
    final private static String RESIGN = "resign";
    final private static String SEPARATOR = " ";
    final private static String SUFFIX = "}";
    
    // per-instance fields
    final private boolean resignFlag;  // immutable
    final java.util.Set<TileCell> set; // mutable
    
    // constructors
    
    // construct a "pass"
    public Move() {
        resignFlag = false;
        set = new TreeSet<>();
        
        assert isPass();
    }
    
    // construct a resignation
    private Move(Tiles tiles) {
        assert tiles != null;

        resignFlag = true;        
        set = new TreeSet<>();
        
        for (Tile tile : tiles) {
            final TileCell tileCell = new TileCell(tile);
            set.add(tileCell);
        }
        
        assert isResignation();
    }
    
    public Move(Move other) {
        assert other != null;
        
        resignFlag = other.resignFlag;
        set = new TreeSet(other.set);
    }
    
    // methods
    
    public static Move chooseConsole(Tiles available, int mustPlay) {
        assert available != null;
        assert mustPlay >= 0;
        
        final Move result = new Move();

        for (;;) {
            final Strings alternatives = new Strings();
            if (result.isPass()) {
                if (mustPlay == 0) {
                   alternatives.addLast("pass");
                }
                alternatives.addLast("resign");
            } else if (result.size() >= mustPlay) {
                alternatives.addLast("move");
            }

            TileCell.Choice choice = 
                    TileCell.chooseConsole(available, alternatives);
            
            switch (choice.string) {
                case "move":
                case "pass":
                    return result;
                    
                case "resign":
                    return new Move(available);
    
                default:
                    result.set.add(choice.tileCell);
            }
        }
    }
    
    public Cells copyCells() {
        final Cells result = new Cells();
        
        for (TileCell tileCell : set) {
            final Cell cell = tileCell.getDestination();
            if (cell != null) {
                result.add(cell);
            }
        }
        
        return result;
    }
    
    public Tiles copyTiles() {
        final Tiles result = new Tiles();
        
        for (TileCell tileCell : set) {
            result.add(tileCell.getTile());
        }
        
        return result;
    }
    
    public int countTilesPlaced() {
        int result = 0;
        
        for (TileCell tileCell : set) {
            if (tileCell.getDestination() != null) {
                ++ result;
            }
        }    
        
        return result;
    }

    public boolean doesPlace() {
        return !resignFlag && !set.isEmpty();
    }
    
    public boolean equals(Move other) {
        if (other == null) {
            return false;
        }
        
        return resignFlag == other.resignFlag && set.equals(other.set);
    }
    
    public boolean involvesSwap() {
        for (TileCell tileCell : set) {
            if (tileCell.isSwap()) {
                return true;
            }
        }    
        
        return false;
    }
    
    final public boolean isPass() {
        return !resignFlag && set.isEmpty();    
    }
    
    public boolean isPureSwap() {
        if (resignFlag) {
            return false;
        }
        
        for (TileCell tileCell : set) {
            if (!tileCell.isSwap()) {
                return false;
            }
        }
        
        return true;
    }
    
    final public boolean isResignation() {
        return resignFlag;
    }

    public void place(Board board) {
        assert board != null;
        assert doesPlace();
        
        for (TileCell tileCell : set) {
            board.place(tileCell);
        }
    }
    
    public boolean repeatsCell() {
        if (set.size() < 2) {
            return false;
        }
        
        final Cells cellsSeen = new Cells();

        for (TileCell tileCell : set) {
            final Cell cell = tileCell.getDestination();
            if (cell != null) {
                if (cellsSeen.contains(cell)) {
                    return true;
                } else {
                    cellsSeen.add(cell);
                }
            }
        }
        
        return false;
    }

    public boolean repeatsTile() {
        if (set.size() < 2) {
            return false;
        }
        
        final Tiles tilesSeen = new Tiles();

        for (TileCell tileCell : set) {
            final Tile tile = tileCell.getTile();
            assert tile != null;
            
            if (tilesSeen.contains(tile)) {
                return true;
            } else {
                tilesSeen.add(tile);
            }
        }
        
        return false;
    }


    public int size() {
        return set.size();
    }
    
    @Override
    public String toString() {
        String result = PREFIX;
        
        if (resignFlag) {
            result += RESIGN + SEPARATOR;
        }
        
        boolean firstFlag = true;
        for (TileCell tileCell : set) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }
            result += tileCell.toString();
        }
        result += SUFFIX;
        
        return result;
    }
}
