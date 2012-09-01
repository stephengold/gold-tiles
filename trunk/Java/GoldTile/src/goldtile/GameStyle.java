// File:     GameStyle.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  GameStyle enum for the Gold Tile Game
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

public enum GameStyle {
    // values
    NONE,      // null game
    DEBUG,     // allows peeking, undo, all hints; clock is optional
    PRACTICE,  // no peeking; allows undo, all hints; clock is optional
    FRIENDLY,  // no peeking, no undo; allows all hints; clock is optional
    CHALLENGE; // no peeking, no undo, no hints; time limits

    // methods, sorted by name

    public boolean allowsHints() {
        return !isChallenge();
    }

    public boolean allowsPeeking() {
        return isDebug();
    }

    public boolean allowsUndo() {
        return isDebug() || isPractice();
    }

    public static GameStyle getDefault() {
        if (GoldTile.debugging) {
            return DEBUG;
        } else {
            return PRACTICE;
        }
    }

    public boolean hasTimeLimit() {
        return isChallenge();
    }

    public boolean isChallenge() {
        return this == CHALLENGE;
    }

    public boolean isDebug() {
        return this == DEBUG;
    }

    public boolean isFriendly() {
        return this == FRIENDLY;
    }

    public boolean isPractice() {
        return this == PRACTICE;
    }

    public boolean showClocks() {
        return isChallenge() || isDebug();
    }
}
