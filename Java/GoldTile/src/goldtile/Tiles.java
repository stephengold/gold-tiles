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
import java.util.Iterator;

public class Tiles extends java.util.TreeSet< Tile > {
    // constants
    public static final class PullEmptyException extends Exception {}
    private static final String PREFIX = "tiles{";
    private static final String SEPARATOR = " ";
    private static final String SUFFIX = "}";
    
    // constructors
    
    public Tiles() {
        super();
    }

    public Tiles(Tiles other) {
        super(other);
    }

    // methods
    
    public void addAllCombos() {
        final int iAttr = 0;
        addCombos(iAttr, new Combo());
    }

    // generate tiles for the stock bag - RECURSIVE
    private void addCombos(int iAttr, Combo comboSoFar) {
        final int attrCount = Combo.getAttrCount();
        if (iAttr < attrCount) {
            final int max = Combo.getValueMax(iAttr);
            for (int attrValue = 0; attrValue <= max; attrValue++) {
                final Attr attr = new Attr(attrValue);
                comboSoFar.setAttr(iAttr, attr);
                addCombos(iAttr + 1, comboSoFar);
            }
        } else {
            assert iAttr == attrCount : iAttr;
            final Tile clone = Tile.cloneAndSetBonus(comboSoFar);
            add(clone);
        }
    }
    
    public boolean areAllCompatible() {
        for (Tile t1 : this) {
            for (Tile t2 : this) {
                if (!t1.equals(t2) && !t1.isCompatibleWith(t2)) {
                    return false;                    
                }    
            }
        }
        
        return true;
    }
    
    public boolean areAllCompatibleWith(Tile tile) {
        for (Tile t : this) {
            if (!tile.isCompatibleWith(t)) {
                 return false;                
            }
        }
        
        return true;
    }
    
    // construct "runs":  subsets of mutually-compatible tiles - RECURSIVE
    private Tiles buildRuns(Tiles runSoFar, Tiles longestRun) {
        assert runSoFar != null;
        assert longestRun != null;
        
        if (isEmpty()) {
            if (runSoFar.size() > longestRun.size()) {
                longestRun = runSoFar;
            }
            
        } else {
            final Tiles run = new Tiles(runSoFar);
            final Tiles remainder = new Tiles(this);
            final Tile first;
            try {
                first = remainder.pullFirst();
            } catch (PullEmptyException e) {
                throw new AssertionError();
            }

            // build runs excluding the first tile
            longestRun = remainder.buildRuns(run, longestRun);

            if (run.areAllCompatibleWith(first)) {
                // build runs including the first tile
                run.add(first);
                longestRun = remainder.buildRuns(run, longestRun);
            }
        }
        
        return longestRun;
    }
    
    public boolean contains(Combo combo) {
        return first(combo) != null;
    }
    
    public boolean contains(TileOpt tileOpt) {
        return first(tileOpt) != null;
    }
    
    public String describe() {
        String result = "";
        
        boolean firstFlag = true;
        for (Tile tile : this) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }

            result += tile.describe();
        }       

        return result;
    }
    
    public Tile first(Combo combo) {
        for (Tile tile : this) {
            if (tile.hasCombo(combo)) {
                return tile;
            }
        }
        
        return null;
    }
    
    public Tile first(TileOpt tileOpt) {
        for (Tile tile : this) {
            if (tile.hasOpt(tileOpt)) {
                return tile;
            }
        }

        return null;
    }
    
    public int firstMatchingAttr() {
        assert areAllCompatible();
        assert size() > 1 : size();
        assert Combo.getAttrCount() == 2 : Combo.getAttrCount();
        
        final Tile first = first();
        assert first != null;
        
        final Tile second = higher(first);
        assert second != null;

        return first.firstMatchingAttr(second);
    }
    
    public int getBonusFactor() {
        int result = 1;
        
        for (Tile tile : this) {
            if (tile.hasBonus()) {
                result *= 2;
            }
        }
        
        assert result > 0;
        return result;
    }
    
    // return the largest subset of mutually compatible tiles
    public Tiles getLongestRun() {
        final Tiles uniqueCombos = skipClones();
        final Tiles result = uniqueCombos.buildRuns(new Tiles(), new Tiles());

        assert result.areAllCompatible();
        return result;
    }
    
    private Tile pullFirst() throws PullEmptyException {
        final int n = size();
        if (n == 0) {
            throw new PullEmptyException();
        }
        
        Tile result = first();       
        boolean successFlag = remove(result);
        assert successFlag;
        
        assert size() == n-1 : size();
        return result;
    }
    
    private Tile pullRandom() throws PullEmptyException {
        final int n = size();
        if (n == 0) {
            throw new PullEmptyException();
        }
        
        final int r = Global.nextInt(n);

        // find the "r"th tile in the bag
        final Iterator iTile = iterator();
        assert iTile.hasNext();
        
        for (int i = 0; i < r; i++) {
            iTile.next();
            assert iTile.hasNext();
        }

        final Object object = iTile.next();
        iTile.remove();
        final Tile result = (Tile)object;
        
        assert size() == n-1 : size();
        return result;
    }
    
    public Tiles pullRandom(int count) throws PullEmptyException {
        assert count >= 0 : count;
        
        final Tiles result = new Tiles();
        
        for (int iTile = 0; iTile < count; iTile++) {
            final Tile tile = pullRandom();
            result.add(tile);
        }
        
        return result;
    }
    
    // return a new set containing only one instance of each clone/combo
    public Tiles skipClones() {
        final Tiles result = new Tiles();
        
        for (Tile tile : this) {
            final Combo combo = tile.getCombo();
            if (!result.contains(combo)) {
                result.add(tile);
            }
        }
        
        return result;
    }
    
    @Override
    public String toString() {
        String result = PREFIX;

        boolean firstFlag = true;
        for (Tile tile : this) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }

            result += tile.toString();
        }       
        result += SUFFIX;

        return result;
    }
}
