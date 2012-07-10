// File:     GoldTile.java
// Location: Java
// Purpose:  main class for the Gold Tile Game
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

import java.io.IOException;

public class GoldTile {
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        GameOpt options = new GameOpt();
        Cell.setStatic(options);
        Tile.setStatic(options);
        
        final long a = Combo.combinationCnt();
        System.out.print(a);
        System.out.println(" possible combos.");

        String[] alternatives = new String[1];
        alternatives[0] = "quit";

        Tiles stockBag = new Tiles();
        for (short i = 0; i < options.tilesPerCombo(); i++) {
            // Generate all possible tiles.
            stockBag.restock();
        }
        String listString = stockBag.description();
        System.out.println(listString);
        
        final Tile c = new Tile();
        System.out.print("c:");
        String quit = c.getUserChoice(stockBag, alternatives);
        if (quit.equals("quit"))
            System.exit(0);
        
        final Tile d = new Tile();
        System.out.print("d:");
        quit = d.getUserChoice(stockBag, alternatives);
        if (quit.equals("quit"))
            System.exit(0);
        
        System.out.print(c.description());
        System.out.print(" and ");
        System.out.print(d.description());
        final boolean isCompat = c.combo().isCompatibleWith(d.combo());
        if (isCompat) {
            final int ai = c.combo().commonAttr(d.combo());
            System.out.print(" have a common attribute in position #");
            System.out.print(ai);
            System.out.println(".");
        } else {
            System.out.println(" are incompatible.");                
        }

        System.out.print(c);
        System.out.print(" and ");
        System.out.print(d);
        System.out.print(" are ");
        final boolean isEqual = c.equals(d);
        if (!isEqual) {
            System.out.print("not ");
        }
        System.out.println("equal.");        
    }
}
