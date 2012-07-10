// File:     Combo.java
// Location: Java/GoldTile/src/goldtile
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
    
    // static fields
    
    static private short attrCnt = 0;
    static private short valueMax[];
    static public final short ATTR_CNT_MIN = 2;
    static public final short ATTR_CNT_DEFAULT = 2;
    static public final short VALUE_CNT_DEFAULT = 6;
    static private final AttrMode stringMode = AttrMode.ATTR_MODE_ABC;
    
    // constructors
    
    public Combo() {
        assertInitialized();
        
        attrs = new short[attrCnt];          
    }
    
    /**
     * @param other the Combo to be replicated
     */
    public Combo(Combo other) {
        assertInitialized();
        
        attrs = new short[attrCnt];
        System.arraycopy(other.attrs, 0, attrs, 0, attrCnt);
    }

    /**
     * @param text string-form of a Combo to construct
     */
    public Combo(String text) {
        assertInitialized();
        
        attrs = new short[attrCnt];
        
        short iAttr;
        for (iAttr = 0; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);

            if (iAttr < attrCnt) {
                int value = stringMode.charToAttr(ch);
                if (value > valueMax(iAttr)) {
                    value = 0; // so resulting object can be valid
                }
                setAttr(iAttr, value);
            }
        }

        while (iAttr < attrCnt) {
            /*
             * not enough characters in the string -- 
             * pad the attribute array with zeroes
             */
            setAttr(iAttr, 0);
            iAttr++;
        }
    }

    // methods
    
    private static void assertInitialized() {
        assert attrCnt >= ATTR_CNT_MIN;
        assert valueMax != null;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public short attr(int iAttr) {
        return attrs[iAttr];
    }
    
    public static short attrCnt() {
        return attrCnt;
    }

    public static short attrCntMax() {
        if (Global.consoleFlag) { 
            return 0x7fff;
        } else {
            return 5;
        }
    }
    
    public static long combinationCnt() {
        assertInitialized();

        long result = 1;
        for (int iAttr = 0; iAttr < attrCnt; iAttr++) {
            result *= valueCnt(iAttr);
        }
        
        return result;
    }

    /**
     * @param other the Combo to be compared
     */
    public short commonAttr(Combo other) {
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
    public short countMatchingAttrs(Combo other) {
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
            final short value = attr(iAttr);
            final AttrMode displayMode = AttrMode.defaultDisplayMode(iAttr);
            result += displayMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param other the Combo to compare with
     */
    public boolean equals(Combo other) {
        final short matchCnt = countMatchingAttrs(other);
        
        return matchCnt == attrCnt;        
    }
    
    /**
     * @param text the description to convert
     */
    public static Combo fromDescription(String text) {
        assertInitialized();
        
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
            iAttr++;
        }

        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the test value for the attribute
     */
    public boolean hasAttr(int iAttr, int value) {
        return attr(iAttr) == value;
    }

    /**
     * @param other the Combo to compare with
     */
    public boolean isCompatibleWith(Combo other) {
        final short matchCnt = countMatchingAttrs(other);
        
        return matchCnt == 1;
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the new value for the attribute
     */
    public void setAttr(int iAttr, int value) {
        assert value >= 0;
        assert value <= valueMax[iAttr];
        
        attrs[iAttr] = (short)value;
    }
    
    public static void setStatic(GameOpt opt) {
        attrCnt = opt.attrCnt();
        valueMax = opt.valueMax();
        
        assertInitialized();
    }

    public String toString() {
        String result = "";
        
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            final int value = attrs[iAttr];
            result += stringMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public static short valueCnt(int iAttr) {
        short result = valueMax(iAttr);
        result++;
        return result;    
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static short valueMax(int iAttr) {
        assertInitialized();
    
        return valueMax[iAttr];
    }
}
