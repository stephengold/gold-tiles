// File:     Combo.java
// Location: Java
// Purpose:  Combo class for the Gold Tile Game
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
public class Combo {
    private short attrs[];
    static private short attrCnt = 0;
    static private short valueMax[];

    // constructors
    
    public Combo() {
        if (attrCnt <= 0)
            throw new RuntimeException();
        
        attrs = new short[attrCnt];          
    }
    
    /**
     * @param other the Combo to be replicated
     */
    public Combo(Combo other) {
        if (attrCnt <= 0)
            throw new RuntimeException();
        
        attrs = new short[attrCnt];
        System.arraycopy(other.attrs, 0, attrs, 0, attrCnt);
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public int attr(int iAttr) {
        return attrs[iAttr];
    }
    
    public static short attrCnt() {
        if (attrCnt <= 0)
            throw new RuntimeException();

        return attrCnt;
    }
    
    public static long combinationCnt() {
        if (attrCnt <= 0)
            throw new RuntimeException();

        long result = 1;
        for (int iAttr = 0; iAttr < attrCnt; iAttr++) {
            result *= valueCnt(iAttr);
        }
        
        return result;
    }

    /**
     * @param other the Combo to be compared
     */
    public int commonAttr(Combo other) {
        short result = attrCnt;
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            if (attrs[iAttr] == other.attrs[iAttr]) {
                result = iAttr;
                break;
            }
        }

        if (result >= attrCnt)
            throw new RuntimeException();
    
        return result;
    }
    
    /**
     * @param other the Combo to be compared
     */
    public int countMatchingAttrs(Combo other) {
        short result = 0;
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            final int otherValue = other.attrs[iAttr];
            if (hasAttr(iAttr, otherValue)) {
                result ++;
            }
        }

        return result;
    }    

    public String description() {
        String result = "";
       
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            final int value = attrs[iAttr];
            final AttrMode displayMode = AttrMode.defaultDisplayMode(iAttr);
            result += displayMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param text the description to convert
     */
    public Combo fromDescription(String text) {   
        Combo result = new Combo();

        short iAttr;
        for (iAttr = 0 ; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);
            if (iAttr < attrCnt) {
                final AttrMode displayMode = AttrMode.defaultDisplayMode(iAttr);
                int value = displayMode.charToAttr(ch);
                if (value > valueMax(iAttr)) {
                    value = 0; // so resulting object can be valid
                }
                result.setAttr(iAttr, value);
            }
        }

        while (iAttr < attrCnt) {
            /*
             * Not enough characters in the string -- 
             * pad the attribute array with zeroes.
             */
            result.setAttr(iAttr, 0);
        }

        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the test value for the attribute
     */
    public boolean hasAttr(int iAttr, int value) {
        return (attr(iAttr) == value);
    }

    /**
     * @param other the Combo to compare with
     */
    public boolean isCompatibleWith(Combo other) {
        final int matchCnt = countMatchingAttrs(other);
        
        return (matchCnt == 1);
    }
    
    /**
     * @param other the Combo to compare with
     */
    public boolean isEqualTo(Combo other) {
        final int matchCnt = countMatchingAttrs(other);
        
        return (matchCnt == attrCnt);        
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the new value for the attribute
     */
    public void setAttr(int iAttr, int value) {
        if (value < 0 || value > valueMax[iAttr])
            throw new RuntimeException();
        
        attrs[iAttr] = (short)value;
    }
    
    public static void setStatic() {
        attrCnt = 2;
        valueMax = new short[attrCnt];
        valueMax[0] = 5;
        valueMax[1] = 5;
    }

    public String toString() {
        String result = "";
        
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            final int value = attrs[iAttr];
            final AttrMode displayMode = AttrMode.ATTR_MODE_ABC;
            result += displayMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public static int valueCnt(int iAttr) {
        return 1 + valueMax(iAttr);    
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static int valueMax(int iAttr) {
        return valueMax[iAttr];
    }
}
