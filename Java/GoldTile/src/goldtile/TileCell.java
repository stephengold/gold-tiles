// File:     TileCell.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  TileCell class for the Gold Tile Game
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

public class TileCell implements Comparable {
    
    public static class Choice {
        final public String string;
        public TileCell tileCell = null;
        
        public Choice(String string) {
            this.string = string;
        }
    }

    // constants
    final private static String SEPARATOR = "@";
    final private static String SWAP = "swap";
    
    // per-instance fields
    final private Cell destination; // null means "swap area"
    final private Tile tile;
    
    // constructors
    
    public TileCell(Tile tile) {
        assert tile != null;
        
        this.destination = null;
        this.tile = tile;
    }
    
    public TileCell(Tile tile, Cell cell) {
        assert tile != null;
        
        this.destination = cell;
        this.tile = tile;
    }
    
    // methods

    public static Choice chooseConsole(Tiles available, Strings alternatives) {
        assert available != null;
        assert alternatives != null;
        
        final Tile.Choice choice = Tile.chooseConsole(available, alternatives);
        final Choice result = new Choice(choice.string);
        if (choice.tile != null) {
            final Cell cell = Cell.chooseConsole("swap");
            result.tileCell = new TileCell(choice.tile, cell);
        }

        return result;
    }
    
    @Override
    public int compareTo(Object object) {
        assert object != null;
        TileCell other = (TileCell)object;

        if (tile != other.tile) {
            return tile.compareTo(other.tile);
            
        } else if (destination == null) {
            if (other.destination == null) {
                return 0;
            } else {
                return 1;
            }
            
        } else if (other.destination == null) {
            return -1;
            
        } else {
            return destination.compareTo(other.destination);
        }
    }
    
    public Cell getDestination() {
        return destination;
    }
    
    public Tile getTile() {
        return tile;
    }
    
    public boolean isSwap() {
        return destination == null;    
    }
    
    @Override
    public String toString() {
        String result = String.format("%d%s", tile.getId(), SEPARATOR);

        if (isSwap()) {
            result += SWAP;
        } else {
            result += destination.toString();
        }

        return result;
    }
}
