// File:     Poly.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Poly class for the Gold Tile Game
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

public class Poly
    extends java.util.LinkedList< FractionPair >
{
    // methods
    
    void add(double x, double y) {
        assert x >= 0.0 : x;
        assert x <= 1.0 : x;
        assert y >= 0.0 : y;
        assert y <= 1.0 : y;

        final FractionPair pair = new FractionPair(x, y);
        add(pair);
    }
    
    void getPoints(int[] xPoints, int[] yPoints, int numPoints, 
            Rect bounds, boolean invertFlag)
    {
        assert numPoints <= size() : numPoints;

        int i = 0;
        for (FractionPair pair : this) {
             final java.awt.Point point = pair.interpolate(bounds, invertFlag);
             xPoints[i] = point.x;
             yPoints[i] = point.y;
             i++;
        }
    }
}
