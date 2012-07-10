// File:     Tiles.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Tiles class for the Gold Tile Game
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
import java.util.*;

public class Tiles extends TreeSet<Tile> {
    
    private void addAllTiles(int iAttr, Combo combo) {
        final int nAttr = Combo.attrCnt();
        if (iAttr < nAttr) {
            final short max = Combo.valueMax(iAttr);
            for (short attr = 0; attr <= max; attr++) {
                combo.setAttr(iAttr, attr);
                addAllTiles(iAttr + 1, combo);
            }
        } else {
            assert iAttr == nAttr;
            final Tile clone = Tile.cloneAndSetBonus(combo);
            add(clone);
        }
    }
    
    public boolean containsOpt(TileOpt tileOpt) {
        return findFirst(tileOpt) != null;
    }
    
    public String description() {
        String result = "";
        
        for (Tile tile : this) {
            result += tile.description() + " ";
        }

        return result;
    }
    
    public Tile findFirst(TileOpt tileOpt) {
        for (Tile tile : this) {
            if (tile.hasOpt(tileOpt)) {
                return tile;
            }
        }

        return null;
    }
    
    public void restock() {
        final short attrIndex = 0;
        Combo combo = new Combo();

        addAllTiles(attrIndex, combo);
    }
}
