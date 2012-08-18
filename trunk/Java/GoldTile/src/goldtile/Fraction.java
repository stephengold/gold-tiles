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

public class Fraction {
    // constants
    final private static double MIN = 0.0;
    final private static double MAX = 1.0;
    
    // per-instance fields (immutable)
    final private double value;
    
    // constructors
    
    public Fraction(double value) {
        assert value >= MIN : value;
        assert value <= MAX : value;

        this.value = value;
    }
    
    /**
     * @param other the Fraction to be replicated
     */
    public Fraction(Fraction other) {
        assert other != null;
        
        value = other.value;   
    }
    
    // methods
    
    public boolean randomBoolean() {
        if (value > 0.0) {
            if (value < 1.0) {
                final Fraction random = Global.nextFraction();
                return random.value < value;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    public double toDouble() {
        return value;
    }

    public double toDouble(boolean invertFlag) {
        if (invertFlag) {
            return 1.0 - value;
        } else {
            return value;
        }
    }
    
    @Override
    public String toString() {
        return Double.toString(value);
    }
}
