// File:     TileOpt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  TileOpt class for the Gold Tile Game
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

public class TileOpt {
    // constants
    private final static char BONUS_CHARACTER = '+';
    
    // per-instance fields
    final public boolean bonusFlag;
    final public Combo combo;
    
    // constructors
    
    public TileOpt() {
        bonusFlag = false;
        combo = new Combo();
    }

    public TileOpt(Combo combo) {
        assert combo != null;
        
        bonusFlag = false;
        this.combo = new Combo(combo);
    }
    
    public TileOpt(Combo combo, boolean bonusFlag) {
        assert combo != null;

        this.bonusFlag = bonusFlag;
        this.combo = new Combo(combo);
    }
    
    /**
     * @param text string-form of a TileOpt to construct
     */
    public TileOpt(String string) {
        assert string != null;
        
        if (StringExt.getLast(string) == BONUS_CHARACTER) {
            bonusFlag = true;
            string = StringExt.shorten(string, 1);
        } else {
            bonusFlag = false;
        }
        combo = new Combo(string);
    }
    
    /**
     * @param other the TileOpt to be replicated
     */
    public TileOpt(TileOpt other) {
        assert other != null;
        assert other.combo != null;
        
        bonusFlag = other.bonusFlag;
        combo = new Combo(other.combo);
        
        assert equals(other);
    }

    // methods
    
    public String describe() {
        String result = combo.describe();
        
        if (bonusFlag) {
            result += BONUS_CHARACTER;
        }
        
        return result;
    }
    
    /**
     * @param other the TileOpt to be compared
     */
    final public boolean equals(TileOpt other) {
        assert other != null;
        assert other.combo != null;
        
        return bonusFlag == other.bonusFlag
            && combo.equals(other.combo);
    }
     
    /**
     * @param text description of a TileOpt to construct
     */
    static public TileOpt fromDescription(String text) {
        assert text != null;
        
        boolean bonusFlag = false;
        if (StringExt.getLast(text) == BONUS_CHARACTER) {
            bonusFlag = true;
            text = StringExt.shorten(text, 1);
        }
        final Combo combo = Combo.fromDescription(text);      
        final TileOpt result = new TileOpt(combo, bonusFlag);

        // caller should verify fidelity
        return result;
    }
    
    public boolean isCompatibleWith(TileOpt other) {
        return combo.isCompatibleWith(other.combo);
    }
    
    /**
     * @param text the description to be compared
     */
    public boolean matchesDescription(String text) {
        String description = describe();

        // Purge both strings, so that only graphic characters are compared.
        description = StringExt.purge(description);
        final String match = StringExt.purge(text);

        return description.equals(match);
    }
    
    @Override
    public String toString() {
        String result = combo.toString();
        
        if (bonusFlag) {
            result += BONUS_CHARACTER;
        }
        
        return result;
    }
}
