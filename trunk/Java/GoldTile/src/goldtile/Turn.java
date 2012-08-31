// File:     Turn.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Turn class for the Gold Tile Game
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

public class Turn {
    // per-instance fields (immutable)
    final private int handIndex;
    final private int mustPlay;
    final private int points;
    final private ReadMove move;   // move first, then draw
    final private Tiles draw;  // move first, then draw
    
    // constructors
    
    // a newly dealt hand
    public Turn(int handIndex, Tiles dealt) {
        assert handIndex >= 0;
        assert dealt != null;
        assert dealt.size() > 0;
        
        this.handIndex = handIndex;
        mustPlay = 0;
        points = 0;
        move = new Move();  // record a "pass"
        this.draw = new Tiles(dealt);
    }
    
    // a move by an established hand
    public Turn(int handIndex, int mustPlay, ReadMove move, 
            int points, Tiles draw) 
    {
        assert handIndex >= 0;
        assert draw != null;
        
        this.handIndex = handIndex;
        this.mustPlay = mustPlay;
        this.points = points;
        this.move = new Move(move);
        this.draw = new Tiles(draw);
    }
    
    // methods, sorted by name
    
    public boolean didPlace() {
        return move.doesPlace();
    }
    
    public boolean wasDrawOnly() {
        return move.isPass() && !draw.isEmpty(); 
    }
}
