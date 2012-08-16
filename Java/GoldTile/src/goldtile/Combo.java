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
    // constants 
    
    final private static AttrMode STRING_MODE = AttrMode.ABC;
    
    final public static int ATTR_COUNT_DEFAULT = 2;
    final public static int ATTR_COUNT_MAX = 5;
    final public static int ATTR_COUNT_MIN = 2;
    final public static int VALUE_COUNT_DEFAULT = 6;
    final public static int VALUE_COUNT_MAX = 9;
    
    // fields
    final private int attrs[] = new int[attrCount];
    
    // static fields
    private static int attrCount = 0;
    private static int valueMax[] = null;
    
    // constructors
    
    public Combo() {
        assertInitialized();
    }
    
    /**
     * @param other the Combo to be replicated
     */
    public Combo(Combo other) {
        assert other != null;
        assertInitialized();
        
        System.arraycopy(other.attrs, 0, attrs, 0, attrCount);
    }

    /**
     * @param text string-form of a Combo to construct
     */
    public Combo(String text) {
        assert text != null;
        assertInitialized();
        
        int iAttr;
        for (iAttr = 0; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);

            if (iAttr < attrCount) {
                int value = STRING_MODE.charToAttr(ch);
                if (value > getValueMax(iAttr)) {
                    value = 0; // so resulting object can be valid
                }
                setAttr(iAttr, value);
            }
        }

        while (iAttr < attrCount) {
            /*
             * not enough characters in the string -- 
             * pad the attribute array with zeroes
             */
            setAttr(iAttr, 0);
            iAttr++;
        }
        // caller should verify fidelity
    }

    // methods
    
    private static void assertInitialized() {
        assert attrCount >= ATTR_COUNT_MIN : attrCount;
        assert valueMax != null;
    }
    
    public static long comboCount() {
        assertInitialized();

        long result = 1;
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            result *= getValueCount(iAttr);
        }
        
        return result;
    }

    /**
     * @param other the Combo to be compared
     */
    private int countMatches(Combo other) {
        int result = 0;
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final int otherValue = other.getAttr(iAttr);
            if (hasAttr(iAttr, otherValue)) {
                result ++;
            }
        }

        return result;
    }    

    public String describe() {
        String result = "";
       
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final AttrMode displayMode = AttrMode.getConsoleDefault(iAttr);
            final int value = getAttr(iAttr);
            result += displayMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param other the Combo to compare with
     */
    public boolean equals(Combo other) {
        return countMatches(other) == attrCount;        
    }
    
    /**
     * @param other the compatible Combo to be compared
     */
    public int findFirstMatch(Combo other) {
        assert isCompatibleWith(other) : other;
        
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            if (attrs[iAttr] == other.attrs[iAttr]) {
                return iAttr;
            }
        }
        throw new AssertionError(other);
    }
    
    /**
     * @param text the description to convert
     */
    public static Combo fromDescription(String text) {
        assertInitialized();
        
        final Combo result = new Combo();

        int iAttr;
        for (iAttr = 0 ; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);
            if (iAttr < attrCount) {
                final AttrMode displayMode = AttrMode.getConsoleDefault(iAttr);
                int value = displayMode.charToAttr(ch);
                if (value > getValueMax(iAttr)) {
                    value = 0; // so resulting object can be valid
                }
                result.setAttr(iAttr, value);
            }
        }

        while (iAttr < attrCount) {
            /*
             * Not enough characters in the string -- 
             * pad the attribute array with zeroes.
             */
            result.setAttr(iAttr, 0);
            iAttr++;
        }

        // caller should verify fidelity
        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public int getAttr(int iAttr) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        
        return attrs[iAttr];
    }
    
    public static int getAttrCount() {
        return attrCount;
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static int getValueCount(int iAttr) {
        return 1 + getValueMax(iAttr);
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static int getValueMax(int iAttr) {
        assertInitialized();
    
        return valueMax[iAttr];
    }

    /**
     * @param iAttr the index of the attribute
     * @param value the test value for the attribute
     */
    public boolean hasAttr(int iAttr, int value) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        assert value >= 0 : value;
        assert value <= valueMax[iAttr] : value;

        return getAttr(iAttr) == value;
    }

    /**
     * @param other the Combo to compare with
     */
    public boolean isCompatibleWith(Combo other) {
        assert other != null;
        
        final int matchCount = countMatches(other);
        
        return matchCount == 1;
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the new value for the attribute
     */
    final public void setAttr(int iAttr, int value) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        assert value >= 0 : value;
        assert value <= valueMax[iAttr] : value;
        
        attrs[iAttr] = value;
    }
    
    public static void setStatic(GameOpt gameOpt) {
        attrCount = gameOpt.getAttrCount();
        valueMax = gameOpt.copyValueMax();
        
        assertInitialized();
    }

    @Override
    public String toString() {
        String result = "";
        
        for (short iAttr = 0; iAttr < attrCount; iAttr++) {
            final int value = attrs[iAttr];
            result += STRING_MODE.attrToChar(value);
        }
        
        return result;
    }    
}
