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
    ATTR_MODE_ABC, 
    ATTR_MODE_RST, 
    ATTR_MODE_123, 
    ATTR_MODE_SHAPE,
    ATTR_MODE_COLOR;
    
    // methods
    
    /**
     * @param value the value of the attribute
     */
    public char attrToChar(int value) {
        char result = '?';
        
        switch (this) {
            case ATTR_MODE_ABC:
                result = (char)('A' + value);
                break;
            case ATTR_MODE_RST:
                result = (char)('R' + value);
                break;
            case ATTR_MODE_123:
                result = (char)('1' + value);
                break;
            default:
                throw new RuntimeException();
        }

        return result;
    }

    /**
     * @param ch the char representation of the attribute
     */
    public short charToAttr(char ch) {
        short result = 0;
        
        switch (this) {
            case ATTR_MODE_ABC:
                result = (short)(ch - 'A');
                break;
            case ATTR_MODE_RST:
                result = (short)(ch - 'R');
                break;
            case ATTR_MODE_123:
                result = (short)(ch - '1');
                break;
            default:
                throw new RuntimeException();
        }

        return result;
    }

    /**
     * @param iAttr the index of the attribute
     */
    public static AttrMode defaultDisplayMode(int iAttr) {
        AttrMode result = ATTR_MODE_SHAPE;
        
        switch (iAttr) {
            case 0:
                result = ATTR_MODE_ABC;
                break;
            case 1:
                result = ATTR_MODE_RST;
                break;
            default:
                result = ATTR_MODE_123;
        }
        
        return result;
    }
}
