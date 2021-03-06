// File:     HintStrength.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  HintStrength enum for the Gold Tile Game
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

public enum HintStrength {
    // values
    NONE,
    UNUSED,
    CONNECTED,
    USABLE_BY_PLAYABLE,
    USABLE_BY_ACTIVE;

    // methods

    public static HintStrength getStrongDefault() {
        return USABLE_BY_ACTIVE;
    }

    public static HintStrength getWeakDefault() {
        return NONE;
    }

    public boolean isStrong() {
        return this != NONE && this != UNUSED;
    }
}
