// File:     DisplayModes.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  DisplayModes class for the Gold Tile Game
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

public class DisplayModes {
    final private AttrMode modes[];
    
    DisplayModes(Display display) {
        final int count = Combo.ATTR_COUNT_MAX;
        modes = new AttrMode[count];
        for (int iAttr = 0; iAttr < count; iAttr++) {
            final AttrMode mode = AttrMode.getDefault(display, iAttr);
            modes[iAttr] = mode; 
        }
    }
    
    DisplayModes(DisplayModes other) {
        final int count = Combo.ATTR_COUNT_MAX;
        modes = new AttrMode[count];
        for (int iAttr = 0; iAttr < count; iAttr++) {
            final AttrMode mode = other.modes[iAttr];
            modes[iAttr] = mode; 
        }            
    }
    
    // methods
    
    // clean up  display modes for the start of a new game
    public void cleanup() {
        // can't display more than 4 markings per tile
        final int markingCnt = getMarkingCount();
        if (markingCnt > Markings.MARKING_COUNT_MAX) {
            final AttrMode mode = AttrMode.COLOR;
            setMode(0, mode);
        }

        // only one attribute can use color
        while (getColorCnt() > 1) {
            final int ind = getSecondColorIndex();
            final AttrMode mode = AttrMode.getFirst();
            setMode(ind, mode);
        }

        assert getColorCnt() <= 1 : getColorCnt();
        assert getMarkingCount() <= Markings.MARKING_COUNT_MAX 
                : getMarkingCount();
    }
    
    public boolean equals(DisplayModes other) {
        return java.util.Arrays.deepEquals(modes, other.modes);    
    }
    
    public int getColorCnt() {
        int result = 0;
        for (int iAttr = 0; iAttr < Combo.ATTR_COUNT_MAX; iAttr++) {
            final AttrMode mode = getMode(iAttr);
            if (mode.isColor()) {
                result++;
            }
        }

        return result;
    }
    
    public int getMarkingCount() {
        int result = 0;
        for (int iAttr = 0; iAttr < Combo.getAttrCount(); iAttr++) {
            final AttrMode mode = getMode(iAttr);
            if (!mode.isColor()) {
                result++;
            }
        }

        return result;
    }
    
    public AttrMode getMode(int iAttr) {
        assert iAttr < Combo.ATTR_COUNT_MAX : iAttr;
        
        return modes[iAttr];
    }

    private int getSecondColorIndex() {
        int count = 0;
        
        for (int iAttr = 0; iAttr < Combo.ATTR_COUNT_MAX; iAttr++) {
            final AttrMode mode = getMode(iAttr);
            if (mode.isColor()) {
                count++;
                if (count == 2) {
                    assert iAttr > 0 : iAttr;
                    return iAttr;
                }
            }
        }

        throw new AssertionError(this);
    }
    
    public void setMode(int iAttr, AttrMode newMode) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < Combo.ATTR_COUNT_MAX : iAttr;
                
        modes[iAttr] = newMode;      
    }
}
