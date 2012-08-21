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
    // constants, sorted by type
    
    final private static AttrMode STRING_MODE = AttrMode.ABC;
    
    final public static int ATTR_COUNT_DEFAULT = 2;
    final public static int ATTR_COUNT_MAX = 5;
    final public static int ATTR_COUNT_MIN = 2;
    
    // per-instance fields (mutable)
    final private Attr attrs[] = new Attr[attrCount];
    
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
                int attrValue = STRING_MODE.charToAttr(ch);
                if (attrValue <= 0 || attrValue > getValueMax(iAttr)) {
                    setAttr(iAttr, new Attr()); 
                    // so the resulting object will be valid
                } else {
                    setAttr(iAttr, new Attr(attrValue));
                }
            }
        }

        while (iAttr < attrCount) {
            /*
             * not enough characters in the string -- 
             * pad the attribute array with zeroes
             */
            setAttr(iAttr, new Attr());
            iAttr++;
        }
        // caller should verify fidelity
    }

    // methods, sorted by name
    
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
    private int countMatchingAttrs(Combo other) {
        assert other != null;
        
        int result = 0;
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final Attr attr = getAttr(iAttr);
            if (other.hasMatchingAttr(iAttr, attr)) {
                result ++;
            }
        }

        return result;
    }    

    public String describe() {
        String result = "";
       
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final AttrMode displayMode = AttrMode.getConsoleDefault(iAttr);
            final Attr value = getAttr(iAttr);
            result += displayMode.attrToChar(value);
        }
        
        return result;
    }
    
    /**
     * @param other the Combo to compare with
     */
    public boolean equals(Combo other) {
        if (other == null) {
            return false;
        }
        
        return countMatchingAttrs(other) == attrCount;        
    }
    
    /**
     * @param other the compatible Combo to be compared
     */
    public int firstMatchingAttr(Combo other) {
        assert other != null;
        assert isCompatibleWith(other) : other;
        
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final Attr attr = getAttr(iAttr);
            if (other.hasMatchingAttr(iAttr, attr)) {
                return iAttr;
            }
        }
        throw new AssertionError(other);
    }
    
    /**
     * @param text the description to convert
     */
    public static Combo fromDescription(String text) {
        assert text != null;
        assertInitialized();
        
        final Combo result = new Combo();

        int iAttr;
        for (iAttr = 0 ; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);
            if (iAttr < attrCount) {
                final AttrMode displayMode = AttrMode.getConsoleDefault(iAttr);
                int attrValue = displayMode.charToAttr(ch);
                if (attrValue <= 0 || attrValue > getValueMax(iAttr)) {
                    result.setAttr(iAttr, new Attr());
                    // so the resulting object will be valid
                } else {
                    result.setAttr(iAttr, new Attr(attrValue));
                }
            }
        }

        while (iAttr < attrCount) {
            /*
             * Not enough characters in the string -- 
             * pad the attribute array with zeroes.
             */
            result.setAttr(iAttr, new Attr());
            iAttr++;
        }

        // caller should verify fidelity
        return result;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public Attr getAttr(int iAttr) {
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
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        
        return 1 + getValueMax(iAttr);
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static int getValueMax(int iAttr) {
        assertInitialized();
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;        
    
        return valueMax[iAttr];
    }

    /**
     * @param iAttr the index of the attribute
     * @param value the test value for the attribute
     */
    public boolean hasMatchingAttr(int iAttr, Attr attr) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        assert attr != null;
        assert attr.intValue() <= getValueMax(iAttr) : attr;

        return getAttr(iAttr).equals(attr);
    }

    /**
     * @param other the Combo to compare with
     */
    public boolean isCompatibleWith(Combo other) {
        assert other != null;
        
        return countMatchingAttrs(other) == 1;
    }
    
    /**
     * @param iAttr the index of the attribute
     * @param value the new value for the attribute
     */
    final public void setAttr(int iAttr, Attr attr) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < attrCount : iAttr;
        assert attr != null;
        assert attr.intValue() <= valueMax[iAttr] : attr;
        
        attrs[iAttr] = attr;
        
        assert hasMatchingAttr(iAttr, attr);
    }
    
    public static void setStatic(ReadGameOpt gameOpt) {
        attrCount = gameOpt.getAttrCount();
        valueMax = gameOpt.copyValueMax();
        
        assertInitialized();
    }

    @Override
    public String toString() {
        String result = "";
        
        for (short iAttr = 0; iAttr < attrCount; iAttr++) {
            final Attr value = attrs[iAttr];
            result += STRING_MODE.attrToChar(value);
        }
        
        return result;
    }    
}
