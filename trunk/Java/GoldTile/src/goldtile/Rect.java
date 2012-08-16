// File:     Rect.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Rect class for the Gold Tile Game
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
import java.awt.Dimension;
import java.awt.Point;

public class Rect extends java.awt.Rectangle {
    // constructors
    
    public Rect(int left, int top, int w, int h) {
        x = left;
        y = top;
        width = w;
        height = h;
    }
    
    public Rect(Point center, Dimension dim) {
        width = dim.width;
        height = dim.height;
        x = center.x - width/2;
        y = center.y - height/2;
    }

    public Rect(Point ulc, Point brc) {
        x = ulc.x;
        y = ulc.y;
        width = brc.x - x;
        height = brc.y - y;
    }
    
    // methods
    
    /*
     Construct the largest square that is centered and contained
     within this Rect
     */
    public Rect centerSquare() {
        int w = width;
        int h = height;
        int left = x;
        int top = y;

        if (w > h) {
            left += (w - h)/2;
            w = h;
        } else if (h > w) {
            top += (h - w)/2;
            h = w;
        } 
        final Rect result = new Rect(left, top, w, h);

        assert contains(result) : result;   
        return result;
    }
}
