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

public class Tile implements Comparable {
    // classes
    
    public static class Choice {
        final public String string;
        public Tile tile = null;
        
        public Choice(String string) {
            this.string = string;
        }
    }
    
    // constants
    
    final public static int ID_NONE = 0;    // indicates "null tile"
    final public static int ID_DEFAULT = 1; // generated only by def constructor
    final public static int ID_FIRST = 2;   // first ID returned by nextId()
    final public static int ID_MAX = 0x7fffffff;
    
    final private static String SEPARATOR = ":";
    final private static String SEPARATOR_REGEX = "[:]";

    // per-instance fields (immutable)
    final private int id;
    
    // static fields
    private static Fraction bonusProbability;  // configured by setStatic()
    private static int nextId = ID_FIRST;
    private static java.util.Map<Integer,TileOpt> opts;

    // constructors
    
    public Tile() {
        id = ID_DEFAULT;
        if (!opts.containsKey(id)) {
            final TileOpt opt = new TileOpt();
            opts.put(id, opt);
        }
    }

    public Tile(int id) {
        assert id != ID_NONE;
        
        this.id = id;
        assert opts.containsKey(id);
    }
    
    public Tile(int idMagnitude, boolean remote) {
        assert idMagnitude > ID_NONE;
        
        if (remote) {
            id = -idMagnitude; // change sign of remote ID            
        } else {
            this.id = idMagnitude;
        }
        
        assert opts.containsKey(id);
    }
    
    /**
     * @param opt the options for the new Tile
     */
    public Tile(TileOpt opt) {
        assert opt != null;
        
        id = nextId();
        assert !opts.containsKey(id);
       
        opts.put(id, opt);
    }
    
    /**
     * @param text string-form of a Tile to construct
     * @param remoteFlag true if string-form is from a remote host
     */
    public Tile(String text, boolean remote) 
        throws ParseException 
    {
        assert text != null;
        
        final String[] parts = text.split(SEPARATOR_REGEX);
        if (parts.length != 2) {
            throw new ParseException();
        }
        final String first = parts[0];
        final String second = parts[1];

        final int idMagnitude = Integer.parseInt(first);
        if (remote) {
            id = -idMagnitude; // change sign of remote ID
        } else {
            id = idMagnitude;
        }

        final TileOpt opt = new TileOpt(second);
        opts.put(id, opt);
    }
    
    // methods

    public static Choice chooseConsole(Tiles available, Strings alternatives) {
        assert available != null;
        assert !available.isEmpty();
        assert alternatives != null;
        
        for (;;) {
            Console.print("Enter a tile description");
            for (String alt : alternatives) {
                Console.printf(" or %s", StringExt.quote(alt));
            }
            final String input = Console.readLine(": ");
            
            final Choice result = new Choice(input);
            if (alternatives.contains(input)) {
                return result;
            }

            // Convert the description to a TileOpt.
            final TileOpt tileOpt = TileOpt.fromDescription(input);
            
            if (!tileOpt.matchesDescription(input)) {
                Console.printf("%s does not describe a tile.\n", 
                        StringExt.quote(result.string));
            
            } else if (!available.contains(tileOpt)) {
                Console.printf("No %s tile is available.\n", input);

            } else {
                result.tile = available.first(tileOpt);
                return result;
            }
        }
    }
    
    public static Tile cloneAndSetBonus(Combo combo) {
        final boolean bonusFlag = bonusProbability.randomBoolean();
        final TileOpt opt = new TileOpt(combo, bonusFlag);

        return new Tile(opt);
    }
    
    @Override
    public int compareTo(Object object) {
        final Tile other = (Tile)object;

        return id - other.id;
    }
    
    public String describe() {
        return getOpt().describe();
    }
    
    public boolean equals(Tile other) {
        if (other == null) {
            return false;
        }
        
        return id == other.id;
    }
    
    public int firstMatchingAttr(Tile other) {
        final Combo combo = getCombo();
        final Combo otherCombo = other.getCombo();
        
        return combo.firstMatchingAttr(otherCombo);
    }
    
    public Combo getCombo() {
        return getOpt().combo;
    }
    
    public int getId() {
        return id;
    }
    
    public TileOpt getOpt() {
        return opts.get(id);
    }
    
    public boolean hasBonus() {
        return getOpt().bonusFlag;    
    }
    
    public boolean hasCombo(Combo combo) {
        final Combo c = getCombo();
        
        return c.equals(combo);
    }
    
    public boolean hasOpt(TileOpt tileOpt) {
        final TileOpt opt = getOpt();
        
        return opt.equals(tileOpt);
    }
    
    public boolean isCompatibleWith(Tile other) {
        if (other == null) {
            return true;
        } else {
            final TileOpt opt = getOpt();
            final TileOpt otherOpt = other.getOpt();
            
            return opt.isCompatibleWith(otherOpt);
        }
    }
    
    public static boolean isValid(int id) {
        return (id <= -ID_FIRST || (id >= ID_FIRST && id < nextId));
    }

    private static int nextId() {
        assert nextId < ID_MAX;
            
        final int result = nextId;
        nextId = result + 1;
        
        return result;
    }
    
    public static void setStatic(ReadGameOpt opt) {
        Combo.setStatic(opt);
        bonusProbability = opt.getBonusFraction();
        
        nextId = ID_FIRST;
        opts = new java.util.TreeMap<>();
    }
    
    @Override
    public String toString() {
        final TileOpt opt = getOpt();
        
        return String.format("%d%s%s", id, SEPARATOR, opt.toString());
    }
}
