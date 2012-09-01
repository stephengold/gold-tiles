// File:     Fraction.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Fraction class for the Gold Tile Game
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

public class Fraction {
    // constants
    final private static double MIN = 0.0;
    final private static double UNITY = 1.0;
    final private static double MAX = UNITY;

    // per-instance fields (immutable)
    final private double value;

    // constructors

    public Fraction(double value) {
        assert value >= MIN : value;
        assert value <= MAX : value;

        this.value = value;
    }

    // methods, sorted by name

    public boolean equals(Fraction other) {
        return value == other.value;
    }

    public boolean isUnity() {
        return value == UNITY;
    }

    public boolean randomBoolean() {
        if (value > 0.0) {
            if (value < UNITY) {
                final Fraction random = Global.nextFraction();
                return random.value < value;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    public double toDouble() {
        return value;
    }

    public double toDouble(boolean invertFlag) {
        if (invertFlag) {
            return UNITY - value;
        } else {
            return value;
        }
    }

    public double toPercent() {
        return 100.0 * value;
    }

    @Override
    public String toString() {
        return Double.toString(value);
    }
}
