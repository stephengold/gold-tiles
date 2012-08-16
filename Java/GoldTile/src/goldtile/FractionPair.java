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
import java.awt.Point;

public class FractionPair {
    public Fraction x, y;
    
    // constructor
    
    FractionPair(double x, double y) {
        this.x = new Fraction(x);
        this.y = new Fraction(y);
    }
    
    // methods
    
    Point interpolate(Rect bounds, boolean invertYFlag) {
        final double width = bounds.width - 1;
        final int dx = (int)(0.5 + x.toDouble()*width);
        assert dx < bounds.width : dx;
        final int x = bounds.x + dx;
        
        final double height = bounds.height - 1;
        final double yValue = y.toDouble(!invertYFlag);
        final int dy = (int)(0.5 + yValue*height);
        assert dy < bounds.height : dy;
        final int y = bounds.y + dy;  

        final Point result = new Point(x, y);
        assert bounds.contains(result) : result;
        
        return result;
    }
}
