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
import java.io.*;
import java.util.*;

public class Tile implements Comparable {
    private int id;
    
    // static fields
    
    static final public int ID_NONE = 0;    // indicates "no such tile"
    static final public int ID_DEFAULT = 1; // generated only by def constructor
    static final public int ID_FIRST = 2;   // first ID returned by nextId()
    static final public int ID_MAX = 0x7fffffff;
    
    static final private String SEPARATOR = ":";
    static final private String SEPARATOR_REGEX = "[:]";

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
       assert !opts.containsKey(id);
       
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

    public static Tile cloneAndSetBonus(Combo combo) {
        TileOpt opt = new TileOpt(combo);
        opt.hasBonus = bonusProbability.randomBoolean();

        return new Tile(opt);
    }
    
    public Combo combo() {
        final TileOpt opt = opts.get(id);
        
        return opt.combo;
    }
    
    public int compareTo(Object object) {
        final Tile other = (Tile)object;

        return id - other.id;
    }
    
    public String description() {
        final TileOpt opt = opts.get(id);

        return opt.description();
    }
    
    public boolean equals(Tile other) {
        return id == other.id;
    }
    
    public String getUserChoice(Tiles availableTiles, String alternatives[]) 
            throws IOException
    {
        assert id == ID_DEFAULT;
        
        String result = "";
        for (;;) {
            System.out.print("Enter a tile name");
            for (String alt : alternatives) {
                System.out.print(" or ");
                System.out.print(StringExt.quote(alt));
            }
            System.out.print(": ");
            BufferedReader reader = 
                    new BufferedReader(new InputStreamReader(System.in));
            result = reader.readLine();
            if (Arrays.asList(alternatives).contains(result)) {
                break;
            }

            final TileOpt tileOpt = TileOpt.fromDescription(result);
        
            if (!tileOpt.matchesDescription(result)) {
                System.out.print(StringExt.quote(result));
                System.out.println(" is invalid input.");
            
            } else if (!availableTiles.containsOpt(tileOpt)) {
                System.out.print(result);
                System.out.println(" is unavailable.");

            } else {
                final Tile tile = availableTiles.findFirst(tileOpt);
                id = tile.id();
                break;
            }
        }
    
        return result;
    }
    
    public boolean hasOpt(TileOpt tileOpt) {
        final TileOpt opt = opts.get(id);
        
        return opt.equals(tileOpt);
    }
    
    public int id() {
        return id;
    }
    
    static public boolean isValid(int id) {
        return (id <= -ID_FIRST || (id >= ID_FIRST && id < nextId));
    }

    static private int nextId() {
        assert nextId < ID_MAX;
            
        final int result = nextId;
        nextId = result + 1;
        
        return result;
    }
    
    static public void setStatic(GameOpt options) {
        Combo.setStatic(options);
        bonusProbability = options.bonusPercent.toFraction();
        
        nextId = ID_FIRST;
        opts = new TreeMap<>();
    }
    
    public String toString() {
        final TileOpt opt = opts.get(id);
        
        return String.format("%d%s%s", id, SEPARATOR, opt.toString());
    }
}
