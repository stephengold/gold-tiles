/// File:     History.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  History class for the Gold Tile Game
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

import java.util.ArrayList;

public class History {
    // per-instance fields
    private int redoIndex = 0;
    private ArrayList< Turn > turns = new ArrayList<>();

    // methods, sorted by name

    public void add(int handIndex, Tiles dealt) {
        assert handIndex >= 0;
        assert dealt != null;
        assert !canRedo();

        final Turn turn = new Turn(handIndex, dealt);
        turns.add(turn);
        redoIndex++;
    }

    public void add(int handIndex, int mustPlay, ReadMove move, int points,
            Tiles draw, Tiles warm)
    {
        assert handIndex >= 0;
        assert mustPlay >= 0;
        assert move != null;
        assert points >= 0;
        assert draw != null;

        clearToEnd();
        final Turn turn = new Turn(handIndex, mustPlay, move, points,
                draw, warm);
        turns.add(turn);
        redoIndex++;
    }

    public void add(Turn turn) {
        clearToEnd();
        turns.add(turn);
    }

    public boolean canRedo() {
        return redoIndex < size();
    }

    public boolean canUndo() {
        if (redoIndex < 1) {
            return false;
        } else {
            final Turn turn = turns.get(redoIndex - 1);
            return !turn.wasDrawOnly();
        }
    }

    private void clearToEnd() {
        while (canRedo()) {
            turns.remove(redoIndex);
        }
    }

    public int count(Turn turn) {
        assert turn != null;

        int count = 0;
        for (Turn current : turns) {
            if (current == turn) {
                count ++;
            }
        }

        return count;
    }

    public int findLastPlace() {
        for (int i = size() - 1; i >= 0; i--) {
            final Turn turn = turns.get(i);
            if (turn.didPlace() || turn.wasDrawOnly()) {
                return i;
            }
        }

        throw new AssertionError();
    }

    public int getRedoIndex() {
        return redoIndex;
    }

    public int size() {
        return turns.size();
    }

    public Turn undo() {
        assert canUndo();

        redoIndex--;
        return turns.get(redoIndex);
    }
}
