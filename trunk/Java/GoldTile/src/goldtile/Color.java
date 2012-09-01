// File:     Color.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Color class for the Gold Tile Game
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

public class Color extends java.awt.Color {
    // constants
    final public static Color
            BLACK = new Color(0, 0, 0);
    final public static Color
            DARK_GRAY = new Color(80, 80, 80);
    final public static Color
            MEDIUM_GRAY = new Color(128, 128, 128);
    final public static Color
            LIGHT_GRAY = new Color(160, 160, 160);
    final public static Color
            WHITE = new Color(255, 255, 255);
    final public static Color
            RED = new Color(255, 0, 0);
    final public static Color
            PINK = new Color(255, 128, 128);
    final public static Color
            BROWN = new Color(120, 60, 0);
    final public static Color
            YELLOW = new Color(255, 180, 0);
    final public static Color
            DULL_GOLD = new Color(160, 140, 0);
    final public static Color
            GOLD = new Color(255, 204, 0);
    final public static Color
            DARK_GREEN = new Color(0, 100, 0);
    final public static Color
            GREEN = new Color(0, 255, 0);
    final public static Color
            LIGHT_GREEN = new Color(64, 255, 64);
    final public static Color
            DARK_BLUE = new Color(0, 0, 160);
    final public static Color
            LIGHT_BLUE = new Color(128, 128, 255);
    final public static Color
            PURPLE = new Color(128, 0, 128);

    // constructors

    private Color(int red, int green, int blue) {
       super(red, green, blue);
    }
}
