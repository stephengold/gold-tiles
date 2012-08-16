// File:     Strings.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Strings class for the Gold Tile Game
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

public class Strings extends java.util.LinkedList< String > {
    // methods
    
    public int count(String string) {
        int result = 0;
        
        for (String str : this) {
            if (str != null && str.equals(string)) {
                result ++;
            }
        }
        
        return result;
    }
    
    // create unique name using a numeric suffix
    public String inventUnique(String prefix) {
        String result = prefix;

        int i = 2;
        while (contains(result)) {
            result = String.format("%s%d", prefix, i);
            i++;
        }

        return result;
    }
    
    // create unique name using ordinals
    public String inventUnique(String prefix, String infix, String suffix) {
        String result = prefix;
        
        int i = 1;
        while (contains(result)) {
            result = String.format("%s%s%s%s", prefix, infix, 
                    StringExt.ordinal(i), suffix);
            i++;
        }

        return result;
    }
}