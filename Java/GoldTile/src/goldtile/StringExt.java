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
    public static char getLast(String text) {
        final int length = text.length();
        assert length > 0 : text;
        
        return text.charAt(length - 1);
    }
        
    /**
     * Capitalize the first letter of each word.
     * Trim leading and trailing non-graphic characters.
     * Convert adjacent non-graphic characters a single space.
     * Equal-signs (=) are treated as non-graphic characters.
     */
    public static String normalizeName(String name) {
        assert name != null;
        
        final int length = name.length();
        final char space = ' ';

        boolean afterGraphicFlag = false;
        String result = "";
        for (int iChar = 0; iChar < length; iChar++) {
            char ch = name.charAt(iChar);
            final boolean graphicFlag = (ch > space && ch <= '~' && ch != '=');

            if (!graphicFlag) {
                ch = space;
            } else if (!afterGraphicFlag) {  // first letter of a word
                ch = Character.toUpperCase(ch);
            }

            if (graphicFlag || afterGraphicFlag) {
                result += ch;
            }

            afterGraphicFlag = graphicFlag;
        }

        if (!result.isEmpty() && StringExt.getLast(result) == space) {
            /*
             * remove trailing space, which was originally 
             * some non-graphic character
             */
            result = StringExt.shorten(result, 1);
        }
        
        return result;
    }
    
    public static String ordinal(long n) {
        assert n > 0;

        String result;

        if (n == 1) {
            result = "fir";
        } else if (n == 2) {
            result = "seco";
        } else if (n == 3) {
            result = "thi";
        } else if (n == 4) {
            result = "four";
        } else if (n == 5) {
            result = "fif";
        } else {
            // digits will suffice
            result = String.format("%d", n);
        }

        final long onesPlace = n % 10;
        final long tensPlace = (n / 10) % 10;

        if (tensPlace == 1) {
            result += "th";
        } else if (onesPlace == 1) {
            result += "st";
        } else if (onesPlace == 2) {
            result += "nd";
        } else if (onesPlace == 3) {
            result += "rd";
        } else {
            result += "th";
        }

        return result;
    }
    
    public static String pad(String text, int width) {
        assert text != null;
        assert width >= text.length();

        final int pad = width - text.length();
        return repeat(pad, ' ') + text;
    }
    
    public static String plural(long n) {
        assert n >= 0;

        if (n == 1) {
            return "";
        } else {
            return "s";
        }
    }

    public static String plural(long n, String noun) {
        assert noun != null;
        assert n >= 0;

        return String.format("%d %s%s", n, noun, plural(n));
    }
    
    /**
     * @param text the text to purge
     */
    public static String purge(String text) {
        assert text != null;
        
        final int length = text.length();
        final char space = ' ';

        String result = "";        
        for (int iChar = 0; iChar < length; iChar++) {
            final char ch = text.charAt(iChar);
            final boolean graphicFlag = (ch > space && ch <= '~' && ch != '=');
            if (graphicFlag) {
                result += ch;
            }
        }

        return result;
    }
    
    /**
     * @param text the text to enclose in quotes
     */
    public static String quote(String text) {
        return "\"" + text + "\"";
    }
    
    public static String repeat(int count, char ch) {
        assert count >= 0;
        
        String result = "";
        
        for (int iChar = 0; iChar < count; iChar++) {
            result += ch;
        }
        
        return result;
    }
    
    /**
     * @param text the text to shorten
     * @param cnt is the number of characters to remove
     */
    public static String shorten(String text, int count) {
        final int length = text.length();

        return text.substring(0, length - count);
    }
}
