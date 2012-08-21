// File:     AttrMode.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  AttrMode enum for the Gold Tile Game
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

public enum AttrMode {
    // values
    ABC ('A'),
    COLOR,
    NUMERAL ('1'), 
    RST ('R'), 
    SHAPE;
    
    // per-instance fields (immutable)
    final private char base;
    
    // constructors
    
    private AttrMode() {
        base = '!';
    }
    
    private AttrMode(char base) {
       this.base = base;        
    }
    
    // methods, sorted by name
    
    /**
     * @param attr the numeric value of the attribute
     */
    public char attrToChar(Attr attr) {
        switch (this) {
            case ABC:
            case NUMERAL:
            case RST:
                return (char)(base + attr.intValue());
                
            default:
                throw new AssertionError(this);
        }
   }

    /**
     * @param attr the numeric value of the attribute
     */
    public String attrToString(Attr attr) {
        final char ch = attrToChar(attr);
        
        return Character.toString(ch);
    }

    /**
     * @param ch the char representation of the attribute
     */
    public int charToAttr(char ch) {
        switch (this) {
            case ABC:
            case NUMERAL:
            case RST:
                return ch - base;
            default:
                throw new AssertionError(this);
        }
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static AttrMode getConsoleDefault(int iAttr) {
        assert iAttr < Combo.ATTR_COUNT_MAX : iAttr;
        
        switch (iAttr) {
            case 0:
                return ABC;
            case 1:
                return RST;
            default:
                return NUMERAL;
        }
    }
    
    public static AttrMode getDefault(Display display, int iAttr) {
        switch (display) {
            case CONSOLE:
                return getConsoleDefault(iAttr);
            case GUI:
                return getGuiDefault(iAttr);
            default:
                throw new AssertionError(display);
        }    
    }
    
    public static AttrMode getFirst() {
        return ABC;
    }
    
    /**
     * @param iAttr the index of the attribute
     */
    public static AttrMode getGuiDefault(int iAttr) {
        assert iAttr < Combo.ATTR_COUNT_MAX : iAttr;
        
        switch (iAttr) {
            case 0:
                return COLOR;
            default:
                return SHAPE;
        }
    }
    
    public boolean isColor() {
        return this == COLOR;
    }    
}
