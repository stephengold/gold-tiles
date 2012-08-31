/// File:     Turns.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Turns class for the Gold Tile Game
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

public class Turns extends java.util.LinkedList< Turn > {
    // methods, sorted by name
    
    public void add(int handIndex, Tiles dealt) {
        final Turn turn = new Turn(handIndex, dealt);
        add(turn);
    }
    
    public void add(int handIndex, int mustPlay, ReadMove move, int points, 
            Tiles draw) {
        final Turn turn = new Turn(handIndex, mustPlay, move, points, draw);
        add(turn);
    }
    
    public void clearToEnd(Turn turn) {
        assert turn != null;
        assert count(turn) == 1;
        
        final ListIterator iTurn = listIterator();
        
        // Advance the iterator to just past the specified turn.
        Turn prev = null;
        while (iTurn.hasNext() && prev != turn) {
            prev = (Turn)iTurn.next();
        }

        // Remove the specified turn and everything after it.
        while (iTurn.hasNext()) {
            iTurn.remove();
            iTurn.next();
        }
        iTurn.remove();
    }
    
    public int count(Turn turn) {
        assert turn != null;
        
        int count = 0;
        for (Turn current : this) {
            if (current == turn) {
                count ++;
            }
        }
        
        return count;
    }
    
    public int findIndex(Turn turn) {
        if (turn == null) {
            return size();
        }
        
        assert count(turn) == 1;
        return indexOf(turn);
    }
    
    public int lastPlaceIndex() {
        final java.util.Iterator iTurn = descendingIterator();
        
        int result = size();
        
        while (iTurn.hasNext()) {
            final Turn turn = (Turn)iTurn.next();
            result--;
            if (turn.didPlace() || turn.wasDrawOnly()) {
                break;
            }
        }
        
        return result;
    }
}
