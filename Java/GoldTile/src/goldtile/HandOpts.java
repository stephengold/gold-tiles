// File:     HandOpts.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  HandOpts class for the Gold Tile Game
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

public class HandOpts extends java.util.LinkedList< HandOpt > {       
    // methods
    
    public static HandOpts chooseConsole(GameOpt gameOpt) {
        assert gameOpt != null;

        final int handsDealt = gameOpt.getHandsDealt();
        
        HandOpts result = new HandOpts();
        for (int iHand = 0; iHand < handsDealt; iHand++) {
            HandOpt handOpt = HandOpt.chooseConsole(iHand);
            result.addLast(handOpt);
        }
        
        return result;
    }
    
    public Strings getAllPlayerNames() {
        Strings result = new Strings();
        
        for (HandOpt opt : this) {
            result.addLast(opt.getPlayerName());    
        }
        
        return result;
    }
}
