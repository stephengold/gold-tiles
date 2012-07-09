// File:     StringExt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  StringExt class for the Gold Tile Game
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
public class StringExt {
    /**
     * @param text the text to examine
     */
    public static char last(String text) {
        final int length = text.length();
        
        return text.charAt(length - 1);
    }
    
    /**
     * @param text the text to purge
     */
    public static String purge(String text) {
        final int length = text.length();
        String result = "";
        
        for (int iChar = 0; iChar < length; iChar++) {
            final char ch = text.charAt(iChar);
            final boolean isGraphic = (ch > 0x0020 && ch < 0x007f);

            if (isGraphic) {
                result += ch;
            }
        }

        return result;
    }
    
    /**
     * @param text the text to shorten
     * @param cnt is the number of characters to remove
     */
    public static String shorten(String text, int cnt) {
        final int length = text.length();

        String result = text.substring(0, length - cnt);

        return result;
    }
}
