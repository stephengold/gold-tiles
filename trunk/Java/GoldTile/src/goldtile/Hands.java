// File:     Hands.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Hands class for the Gold Tile Game
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
import java.util.ListIterator;

public class Hands extends java.util.LinkedList< Hand > {
    // constants
    private static final String PREFIX = "hands{";
    private static final String SEPARATOR = " ";
    private static final String SUFFIX = "}";

    // methods

    public Hand getNextWorking(Hand hand) {
        assert hand != null;
        
        int iHand = indexOf(hand);
        assert iHand >= 0 : hand;
        
        for (;;) {
            iHand++;
            if (iHand >= size()) {
                iHand = 0;
            }
            final Hand result = get(iHand);
            if (!result.hasResigned()) {
                return result;
            }
        }
    }   

    public boolean hasAnyGoneOut() {
        for (Hand hand : this) {
            if (hand.hasGoneOut()) {
                return true;
            }        
        }
        
        return false;
    }
    
    public boolean haveAllResigned() {
        for (Hand hand : this) {
            if (!hand.hasResigned()) {
                return false;
            }   
        }
        
        return true;
    }
    
    @Override
    public String toString() {
        String result = PREFIX;

        boolean firstFlag = true;
        for (Hand hand : this) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }

            result += hand.toString();
        }       
        result += SUFFIX;

        return result;
    }
}
