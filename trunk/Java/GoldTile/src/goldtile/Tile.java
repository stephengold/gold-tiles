// File:     Tile.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Tile class for the Gold Tile Game
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

public class Tile {
    private int id;
    
    // static fields
    static final int ID_NONE = 0;    // indicates "no such tile"
    static final int ID_DEFAULT = 1; // generated only by default constructor
    static final int ID_FIRST = 2;   // first ID returned by nextId()
    static final int ID_MAX = 0x7fffffff;
    
    static final String SEPARATOR = ":";
    static final String SEPARATOR_REGEX = "[:]";

    static private Fraction bonusProbability;  // configured by setStatic()
    static private int nextId = ID_FIRST;
    static private Map<Integer,TileOpt> opts;
    
    // constructors
    
    public Tile() {
        id = ID_DEFAULT;
        if (!opts.containsKey(id)) {
            final TileOpt opt = new TileOpt();
            opts.put(id, opt);
        }
    }
    
    /**
     * @param opt the options for the new Tile
     */
    public Tile(TileOpt opt) {
       id = nextId();
       if (opts.containsKey(id))
           throw new RuntimeException();
       
       opts.put(id, opt);
    }
    
    /**
     * @param text string-form of a Tile to construct
     * @param remoteFlag true if string-form is from a remote host
     */
    public Tile(String text, boolean remoteFlag) {
        final String[] parts = text.split(SEPARATOR_REGEX);
        
        if (parts.length != 2)
            throw new RuntimeException(); // TODO recovery
        final String first = parts[0];
        final String second = parts[1];

        final int idMagnitude = Integer.parseInt(first);
        if (remoteFlag) {
            id = -idMagnitude; // change sign of remote ID
        } else {
            id = idMagnitude;
        }

        final TileOpt opt = new TileOpt(second);
        opts.put(id, opt);
    }
    
    // methods

    public int id() {
        return id;
    }
    
    static private int nextId() {
        if (nextId >= ID_MAX)
            throw new RuntimeException();
            
        final int result = nextId;
        nextId = result + 1;
        
        return result;
    }
    
    static public void setStatic() {
        Combo.setStatic();
        bonusProbability = new Fraction(0.0);
        nextId = ID_FIRST;
        opts.clear();
    }
}
