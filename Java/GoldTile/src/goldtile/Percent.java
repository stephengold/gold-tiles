// File:     Percent.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Percent class for the Gold Tile Game
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

public class Percent {
    final private int percentage;
    
    public final static int MAX = 100;
    public final static int MIN = 0;
    
    // constructors
    
    public Percent() {
        percentage = 0;
    }
    
    public Percent(int percentage) {
        assert percentage >= MIN : percentage;
        assert percentage <= MAX : percentage;

        this.percentage = percentage;
    }
    
    /**
     * @param other the Percent to be replicated
     */
    public Percent(Percent other) {
        assert other != null;
        
        percentage = other.percentage;   
    }
    
    // methods
    
    public Fraction toFraction() {
        return new Fraction(percentage/(double)MAX);
    }
    
    public int toInt() {
        return percentage;
    }
    
    @Override
    public String toString() {
        return Integer.toString(percentage) + '%';
    }
}
