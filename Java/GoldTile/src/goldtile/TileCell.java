// File:     TileCell.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  TileCell class for the Gold Tile Game
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

public class TileCell implements Comparable {
    // classes
    final public static class Choice {
        final public String string;
        public TileCell tileCell = null;

        public Choice(String string) {
            this.string = string;
        }
    }

    // constants
    final private static String SEPARATOR = "@";
    final private static String SEPARATOR_REGEX = "[@]";
    final private static String SWAP = "swap";

    // per-instance fields (immutable)
    final private Cell destination; // null means "swap area"
    final private Tile tile;

    // constructors

    public TileCell(Tile tile) {
        assert tile != null;

        this.destination = null;
        this.tile = tile;
    }

    public TileCell(Tile tile, Cell destination) {
        assert tile != null;

        this.destination = destination;
        this.tile = tile;
    }

    public TileCell(String text, boolean remote)
        throws ParseException
    {
        assert text != null;

        final String[] parts = text.split(SEPARATOR_REGEX);
        if (parts.length != 2) {
            throw new ParseException();
        }

        final String first = parts[0];
        final String second = parts[1];

        final int tileId = Integer.parseInt(first);
        tile = new Tile(tileId, remote);

        if (second.equals(SWAP)) {
            destination = null;
        } else {
            destination = new Cell(second);
        }
    }

    // methods, sorted by name

    public static Choice chooseConsole(Tiles available, Strings alternatives) {
        assert available != null;
        assert alternatives != null;

        final Tile.Choice choice = Tile.chooseConsole(available, alternatives);
        final Choice result = new Choice(choice.string);
        if (choice.tile != null) {
            final Cell cell = Cell.chooseConsole("swap");
            result.tileCell = new TileCell(choice.tile, cell);
        }

        return result;
    }

    @Override
    public int compareTo(Object object) {
        assert object != null;
        TileCell other = (TileCell)object;

        if (!tile.equals(other.tile)) {
            return tile.compareTo(other.tile);

        } else if (destination == null) {
            if (other.destination == null) {
                return 0;
            } else {
                return 1;
            }

        } else if (other.destination == null) {
            return -1;

        } else {
            return destination.compareTo(other.destination);
        }
    }

    public boolean equals(TileCell other) {
        if (other == null) {
            return false;
        }

        final boolean bothSwap =
                destination == null &&
                other.destination == null;
        final boolean sameCell =
                destination != null &&
                destination.equals(other.destination);
        final boolean sameTile = tile.equals(other.tile);

        return  sameTile && (bothSwap || sameCell);
    }

    public String describe() {
        String result = String.format("%s%s", tile.describe(), SEPARATOR);

        if (isSwap()) {
            result += SWAP;
        } else {
            result += destination.toString();
        }

        return result;
    }

    public Cell getDestination() {
        return destination;
    }

    public Tile getTile() {
        return tile;
    }

    public boolean isSwap() {
        return destination == null;
    }

    @Override
    public String toString() {
        String result = String.format("%d%s", tile.getId(), SEPARATOR);

        if (isSwap()) {
            result += SWAP;
        } else {
            result += destination.toString();
        }

        return result;
    }
}
