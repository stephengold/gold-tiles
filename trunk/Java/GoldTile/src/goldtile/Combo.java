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
    
    // per-instance fields (mutable)
    final private Attr attrs[];
    
    // constructors
    
    public Combo() {
        attrs = new Attr[getGameOpt().getAttrCount()];
        for (int iAttr = 0; iAttr < getGameOpt().getAttrCount(); iAttr++) {
            attrs[iAttr] = new Attr();
        }
    }
    
    /**
     * @param other the Combo to be replicated
     */
    public Combo(Combo other) {
        assert other != null;
        
        attrs = new Attr[getGameOpt().getAttrCount()];
        System.arraycopy(other.attrs, 0, attrs, 0, other.attrs.length);
    }

    /**
     * @param text string-form of a Combo to construct
     */
    public Combo(String text) {
        assert text != null;
        
        attrs = new Attr[getGameOpt().getAttrCount()];
        
        int iAttr;
        for (iAttr = 0; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);

            if (iAttr < getGameOpt().getAttrCount()) {
                final int attrValue = STRING_MODE.charToAttr(ch);
                if (!getGameOpt().getAttrLast(iAttr).isValidValue(attrValue)) {
                    setAttr(iAttr, new Attr()); 
                    // so the resulting object will be valid
                } else {
                    setAttr(iAttr, new Attr(attrValue));
                }
            }
        }

        while (iAttr < getGameOpt().getAttrCount()) {
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
    
    /**
     * @param other the Combo to be compared
     */
    private int countMatchingAttrs(Combo other) {
        assert other != null;
        
        int result = 0;
        for (int iAttr = 0; iAttr < getGameOpt().getAttrCount(); iAttr++) {
            final Attr attr = getAttr(iAttr);
            if (other.hasMatchingAttr(iAttr, attr)) {
                result ++;
            }
        }

        return result;
    }    

    public String describe() {
        String result = "";
       
        for (int iAttr = 0; iAttr < getGameOpt().getAttrCount(); iAttr++) {
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
        
        return countMatchingAttrs(other) == getGameOpt().getAttrCount();        
    }
    
    /**
     * @param other the compatible Combo to be compared
     */
    public int firstMatchingAttr(Combo other) {
        assert other != null;
        assert isCompatibleWith(other) : other;
        
        for (int iAttr = 0; iAttr < getGameOpt().getAttrCount(); iAttr++) {
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
        
        final Combo result = new Combo();

        int iAttr;
        for (iAttr = 0 ; iAttr < text.length(); iAttr++) {
            final char ch = text.charAt(iAttr);
            if (iAttr < getGameOpt().getAttrCount()) {
                final AttrMode displayMode = AttrMode.getConsoleDefault(iAttr);
                final int attrValue = displayMode.charToAttr(ch);
                if (getGameOpt().getAttrLast(iAttr).isValidValue(attrValue)) {
                    result.setAttr(iAttr, new Attr(attrValue));
                } else { // invalid character
                    result.setAttr(iAttr, new Attr());
                    // so the resulting object will be valid
                }
            }
        }

        while (iAttr < getGameOpt().getAttrCount()) {
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
        assert iAttr < getGameOpt().getAttrCount() : iAttr;
        
        return attrs[iAttr];
    }
    
    public static ReadGameOpt getGameOpt() {
        return Game.getInstance().getOpt();
    }

    /**
     * @param iAttr the index of the attribute
     * @param value the test value for the attribute
     */
    public boolean hasMatchingAttr(int iAttr, Attr attr) {
        assert iAttr >= Attr.FIRST : iAttr;
        assert iAttr < getGameOpt().getAttrCount() : iAttr;
        assert attr != null;
        assert getGameOpt().getAttrLast(iAttr).isValid(attr);

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
        assert iAttr < getGameOpt().getAttrCount() : iAttr;
        assert attr != null;
        assert getGameOpt().getAttrLast(iAttr).isValid(attr);
        
        attrs[iAttr] = attr;
        
        assert hasMatchingAttr(iAttr, attr);
    }
    
    @Override
    public String toString() {
        String result = "";
        
        for (short iAttr = 0; iAttr < getGameOpt().getAttrCount(); iAttr++) {
            final Attr value = attrs[iAttr];
            result += STRING_MODE.attrToChar(value);
        }
        
        return result;
    }    
}
