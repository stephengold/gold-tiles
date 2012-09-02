// File:     WideButton.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  WideButton class for the Gold Tile Game
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

public class WideButton
    extends javax.swing.JButton
{
    // constants
    final private static Dimension SIZE = new Dimension(200, 0);

    // constructors
    
    public WideButton() {
        super();
        setPreferredSize(SIZE);
    }

    public WideButton(String text) {
        super(text);
        setPreferredSize(SIZE);
    }
}
