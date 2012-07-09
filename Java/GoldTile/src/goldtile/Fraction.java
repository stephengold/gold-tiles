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
import java.util.*;

public class Fraction {
    private double value;
    
    // static fields
    
    static private Random generator;
    
    // constructors
    
    public Fraction() {
        value = 0.0;
    }
    
    public Fraction(double x) {
        if (x < 0.0 || x > 1.0)
            throw new RuntimeException();
        value = x;
    }
    
    /**
     * @param other the Fraction to be replicated
     */
    public Fraction(Fraction other) {
        value = other.value;   
    }
    
    // methods
    
    public double inverse() {
        return 1.0 - value;
    }
    
    public boolean randomBoolean() {
        boolean result = false;
        
        if (value >= 1.0) {
            result = true;
        } else if (value > 0.0) {
            final double r = generator.nextDouble();
            if (r < 0.0 || r > 1.0)
                throw new RuntimeException();
            result = (r < value);
        }
        
        return result;
    }
    
    public static void reseedGenerator(long seed) {
        generator.setSeed(seed);
    }
    
    public double value() {
        return value;
    }
}
