// File:     Fraction.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Fraction class for the Gold Tile Game
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

public class Attr {
    // constants
    final public static int COUNT_DEFAULT = 6;
    final public static int COUNT_MAX = 9;
    final public static int MIN = 0;
    final public static int MAX_DEFAULT = COUNT_DEFAULT - 1;
    final public static int MAX_MAX = COUNT_MAX - 1;

    // per-instance fields (immutable)
    final private int value;
    
    // constructors
    
    public Attr() {
        this.value = 0;
    }
    
    public Attr(int value) {
        assert value >= MIN : value;
        assert value <= MAX_MAX : value;
        
        this.value = value;
    }

    // methods
    
    public boolean equals(Attr other) {
        if (other == null) {
            return false;
        }
        
        return value == other.value;
    }
    
    public int intValue() {
        return value;
    }
}
