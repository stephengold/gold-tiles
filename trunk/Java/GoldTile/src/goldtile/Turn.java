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
    // per-instance fields
    final private Hand hand;
    final private int mustPlay;        // immutable
    private int points = 0;
    final private Move move;           // move first, then draw
    private Tiles draw = new Tiles();  // move first, then draw
    
    // constructors
    
    // a newly dealt hand
    public Turn(Hand hand) {
        this.hand = hand;
        mustPlay = 0;
        move = new Move();
        draw = hand.copyContents();
    }
    
    // a move
    public Turn(Move move, Hand hand, int mustPlay) {
        this.hand = hand;
        this.mustPlay = mustPlay;
        this.move = new Move(move);
    }
    
    // methods
    
    public boolean didPlace() {
        return move.doesPlace(); 
    }
    
    public void setDraw(Tiles draw) {
        assert draw != null;
        
        this.draw = new Tiles(draw);
    }
    
    public void setPoints(int points) {
        this.points = points;
    }

    public boolean wasDrawOnly() {
        return move.isPass() && !draw.isEmpty(); 
    }
    
}
