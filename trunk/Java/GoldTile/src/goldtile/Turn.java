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
    final private Tiles warm;  // tiles played by the previous move, if any

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
        draw = new Tiles(dealt);
        warm = new Tiles();
    }

    // a move by an established hand
    public Turn(int handIndex, int mustPlay, ReadMove move,
            int points, Tiles draw, Tiles warm)
    {
        assert handIndex >= 0;
        assert mustPlay >= 0;
        assert move != null;
        assert points >= 0;
        assert draw != null;
        assert warm != null;

        this.handIndex = handIndex;
        this.mustPlay = mustPlay;
        this.points = points;
        this.move = new Move(move);
        this.draw = new Tiles(draw);
        this.warm = new Tiles(warm);
    }

    // methods, sorted by name

    public Tiles copyDraw() {
        return new Tiles(draw);
    }

    public Tiles copyWarm() {
        return new Tiles(warm);
    }

    public boolean didPlace() {
        return move.doesPlace();
    }

    public int getHandIndex() {
        return handIndex;
    }

    public ReadMove getMove() {
        return move;
    }

    public int getMustPlay() {
        return mustPlay;
    }

    public int getPoints() {
        return points;
    }

    public boolean wasDrawOnly() {
        return move.isPass() && !draw.isEmpty();
    }
}
