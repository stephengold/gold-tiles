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
    public Combo combo;
    public boolean hasBonus;
    
    // static fields
    
    private final static char BONUS_CHARACTER = '+';
    
    // constructors
    
    public TileOpt() {
        combo = new Combo();
        hasBonus = false;
    }

    public TileOpt(Combo c) {
        combo = new Combo(c);
    }
    
    /**
     * @param text string-form of a TileOpt to construct
     */
    public TileOpt(String text) {
        String copy;
        if (StringExt.last(text) == BONUS_CHARACTER) {
            hasBonus = true;
            copy = StringExt.shorten(text, 1);
        } else {
            hasBonus = false;
            copy = text;
        }
        combo = new Combo(copy);
    }
    
    /**
     * @param other the TileOpt to be replicated
     */
    public TileOpt(TileOpt other) {
        hasBonus = other.hasBonus;
        combo = new Combo(other.combo);
    }

    // methods
    
    public String description() {
        String result = combo.description();
        
        if (hasBonus)
            result += BONUS_CHARACTER;
        
        return result;
    }
    
    /**
     * @param other the TileOpt to be compared
     */
    public boolean equals(TileOpt other) {
        return hasBonus == other.hasBonus
                && combo.equals(other.combo);
    }
     
    /**
     * @param text description of a TileOpt to construct
     */
    static public TileOpt fromDescription(String text) {
        TileOpt result = new TileOpt();
        
        String copy = text;
        if (StringExt.last(text) == BONUS_CHARACTER) {
            result.hasBonus = true;
            copy = StringExt.shorten(text, 1);
        }
        result.combo = Combo.fromDescription(copy);
        
        return result;
    }
    
    /**
     * @param text the description to be compared
     */
    public boolean matchesDescription(String text) {
        String description = description();

        // Purge both strings, so that only graphic characters are compared.
        description = StringExt.purge(description);
        final String match = StringExt.purge(text);

        return description.equals(match);
    }
    
    public String toString() {
        String result = combo.toString();
        
        if (hasBonus)
            result += BONUS_CHARACTER;
        
        return result;
    }
}
