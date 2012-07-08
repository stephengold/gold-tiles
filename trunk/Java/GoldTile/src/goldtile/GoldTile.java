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
public class GoldTile {
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Combo.setStatic();
        Combo c = new Combo();
        c.setAttr(0, 4);
        c.setAttr(1, 5);
        Combo d = new Combo();
        d.setAttr(0, 4);
        d.setAttr(1, 4);
        Combo copy = new Combo(c);
        
        long a = c.combinationCnt();
        System.out.println(a);

        int ai = c.commonAttr(d);
        System.out.println(ai);

        int cnt = c.countMatchingAttrs(d);
        System.out.println(cnt);
        
        boolean isCompat = d.isCompatibleWith(c);
        System.out.println(isCompat);

        boolean isCompat2 = copy.isCompatibleWith(c);
        System.out.println(isCompat2);
    }
}
