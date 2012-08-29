// File:     Markings.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Markings class for the Gold Tile Game
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

public class Markings {
    // constants
    final public static int COLOR_COUNT_MAX = 1;
    final public static int MARKING_COUNT_MAX = 4;
    private static Color colors[] = null;

    // per-instance fields (immutable)
    final private Attr markings[];
    final private AttrMode modes[];
    final private Color color;
    final private int count;
    
    // constructors
    
    Markings(Tile tile, DisplayModes displayModes) {
        if (colors == null) {
            initializeColors();
        }
        
        Color color = colors[0];
        Attr markings[] = new Attr[MARKING_COUNT_MAX];
        AttrMode modes[] = new AttrMode[MARKING_COUNT_MAX];
        int count = 0;

        final int attrCount = Game.getInstance().getOpt().getAttrCount();
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final AttrMode displayMode = displayModes.getMode(iAttr);
            final Attr attr = tile.getCombo().getAttr(iAttr);

            if (displayMode == AttrMode.COLOR) {
                color = colors[attr.intValue()];
            } else {
                assert count < MARKING_COUNT_MAX;
                modes[count] = displayMode;
                markings[count] = attr;
                count++;
            }
        }

        assert count > 0 : count;
        assert count <= MARKING_COUNT_MAX : count;

        this.color = color;
        this.count = count;
        this.markings = markings;
        this.modes = modes;        
    }  
    
    // methods
    
    public Color getColor() {
        return color;
    }
    
    public Attr getMarking(int iAttr) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < count : iAttr;
        
        return markings[iAttr];
    }
    
    public AttrMode getMode(int iAttr) {
        assert iAttr >= 0 : iAttr;
        assert iAttr < count : iAttr;
        
        return modes[iAttr];
    }
    
    private static void initializeColors() {
        assert colors == null : colors;
        colors = new Color[Attr.COUNT_MAX];
        
        colors[0] = Color.BLACK;
        colors[1] = Color.RED;
        colors[2] = Color.DARK_BLUE;
        colors[3] = Color.DARK_GREEN;
        colors[4] = Color.PURPLE;
        colors[5] = Color.BROWN;
        colors[6] = Color.DARK_GRAY;
        colors[7] = Color.PINK;
        colors[8] = Color.LIGHT_BLUE;
    }

    public int size() {
        return count;
    }
}
