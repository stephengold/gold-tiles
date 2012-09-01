// File:     Dim.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Dim class for the Gold Tile Game
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

public class Dim {
    // constants, sorted by type
    final private static int MAX = 0x40000000;
    final private static int MIN = 4;          // must be even
    final private static String ENDLESS = "endless";

    // per-instance fields (immutable)
    final private int count;

    // constructors

    public Dim(int count) {
        assert isValid(count) : count;

        this.count = count;
    }

    public Dim(String text) {
        assert text != null;

        if (text.equals(ENDLESS)) {
            count = MAX;
        } else try {
            count = Integer.parseInt(text);
        } catch (NumberFormatException exception) {
            throw new AssertionError();
        }

        assert isValid(count) : count;
    }

    // methods, sorted by name

    public static int clip(int value) {
        if (value > MAX) {
            return MAX;
        } else if (value < MIN) {
            return MIN;
        } else if (Global.isOdd(value)) {
            return value - 1;
        } else {
            return value;
        }
    }

    public static Dim endless() {
        return new Dim(MAX);
    }

    public static String endlessString() {
        return ENDLESS;
    }

    public boolean equals(Dim other) {
        return count == other.count;
    }

    public boolean isEndless() {
        return count == MAX;
    }

    public boolean isValidIndex(int index) {
        return (index < count/2) && (index >= -count/2);
    }

    public static boolean isValid(int value) {
        if (value > MAX) {
            return false;
        } else if (value < MIN) {
            return false;
        } else if (Global.isOdd(value)) {
            return false;
        } else {
            return true;
        }
    }

    public long times(Dim other) {
        if (isEndless() || other.isEndless()) {
            return Long.MAX_VALUE;
        } else {
            return count * other.count;
        }
    }

    public int toInt() {
        return count;
    }

    @Override
    public String toString() {
        if (isEndless()) {
            return ENDLESS;
        } else {
            return Integer.toString(count);
        }
    }

    public int wrapIndex(int index) {
        int result;

        if (index >= 0) {
            final int numWraps = (index + count/2) / count;
            assert numWraps >= 0 : numWraps;
            result = index - numWraps * count;
        } else {
            final int numWraps = (count/2 - index - 1) / count;
            assert numWraps >= 0 : numWraps;
            result = index + numWraps * count;
        }

        assert isValidIndex(result);
        return result;
    }
}
