// File:     Move.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Move class for the Gold Tile Game
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

import java.util.TreeSet;

public class Move
    implements ReadMove
{
    // constants
    final private static String PREFIX = "move{";
    final private static String RESIGN = "resign";
    final private static String SEPARATOR = " ";
    final private static String SEPARATOR_REGEX = "[ ]";
    final private static String SUFFIX = "}";

    // per-instance fields, sorted by type
    final private boolean resignFlag;   // immutable
    final java.util.Set<TileCell> set;  // mutable

    // constructors

    // construct a "pass"
    public Move() {
        resignFlag = false;
        set = new TreeSet<>();

        assert isPass();
    }

    public Move(ReadMove other) {
        assert other != null;

        resignFlag = other.isResignation();
        set = other.copySet();

        assert other.equals(this);
    }

    public Move(String text, boolean remote)
        throws ParseException
    {
        assert text != null;

        final boolean hasPrefix = text.startsWith(PREFIX);
        final boolean hasSuffix = text.endsWith(SUFFIX);
        if (!hasPrefix || !hasSuffix) {
            throw new ParseException();
        }

        final int endIndex = text.length() - PREFIX.length();
        final String body = text.substring(PREFIX.length(), endIndex);
        final String[] words = body.split(SEPARATOR_REGEX);

        boolean resign = false;
        set = new TreeSet();

        for (String word : words) {
            if (word.equals(RESIGN)) {
                resign = true;
            } else {
                final TileCell tileCell = new TileCell(word, remote);
                add(tileCell);
            }
        }

        resignFlag = resign;
    }

    // construct a resignation
    public Move(Tiles discard) {
        assert discard != null;

        resignFlag = true;
        set = new TreeSet<>();

        for (Tile tile : discard) {
            final TileCell tileCell = new TileCell(tile);
            add(tileCell);
        }

        assert isResignation();
    }

    // methods, sorted by name

    private void add(TileCell tileCell) {
        assert tileCell != null;

        final boolean newElement = set.add(tileCell);
        assert newElement;
    }

    public void add(Tile tile, Cell destination) {
        assert tile != null;

        final TileCell tileCell = new TileCell(tile, destination);
        add(tileCell);
    }

    public static ReadMove chooseConsole(Tiles available, int mustPlay) {
        assert available != null;
        assert mustPlay >= 0;

        final Move result = new Move();

        for (;;) {
            final Strings alternatives = new Strings();
            if (result.isPass()) {
                if (mustPlay == 0) {
                   alternatives.addLast("pass");
                }
                alternatives.addLast("resign");
            } else if (result.size() >= mustPlay) {
                alternatives.addLast("move");
            }

            TileCell.Choice choice =
                    TileCell.chooseConsole(available, alternatives);

            switch (choice.string) {
                case "move":
                case "pass":
                    return result;

                case "resign":
                    return new Move(available);

                default:
                    result.set.add(choice.tileCell);
            }
        }
    }

    @Override
    public Cells copyCells() {
        final Cells result = new Cells();

        for (TileCell tileCell : set) {
            final Cell cell = tileCell.getDestination();
            if (cell != null) {
                result.add(cell);
            }
        }

        return result;
    }

    @Override
    public TreeSet<TileCell> copySet() {
        return new TreeSet<>(set);
    }

    @Override
    public Tiles copyTiles() {
        final Tiles result = new Tiles();

        for (TileCell tileCell : set) {
            result.add(tileCell.getTile());
        }

        return result;
    }

    @Override
    public int countTilesPlaced() {
        int result = 0;

        for (TileCell tileCell : set) {
            if (tileCell.getDestination() != null) {
                ++ result;
            }
        }

        return result;
    }

    @Override
    public String describe() {
        String result;

        if (isResignation()) {
            result = String.format("resigned, returning %s to the stock bag",
                    StringExt.plural(size(), "tile"));

        } else if (isPureSwap()) {
            result = "swapped " + StringExt.plural(size(), "tile");

        } else if (isPass()) {
            result = "passed";

        } else if (!involvesSwap()) {
            result = "played ";
            boolean firstFlag = true;
            for (TileCell tileCell : set) {
                if (firstFlag) {
                    firstFlag = false;
                } else {
                    result += ", ";
                }
                result += tileCell.describe();
            }

        } else {
            result = "played and swapped tiles"; // illegal move
        }

        return result;
    }

    @Override
    public boolean doesPlace() {
        return !resignFlag && !set.isEmpty() && !isPureSwap();
    }

    @Override
    final public boolean equals(Move other) {
        if (other == null) {
            return false;
        }

        return resignFlag == other.isResignation() &&
                set.equals(other.set);
    }

    /**
     * Implement this move on a Partial.
     * @param partial
     */
    @Override
    public void implement(Partial partial) {
        assert partial != null;

        partial.takeBack();
        for (TileCell tileCell : set) {
            final Tile tile = tileCell.getTile();
            partial.activate(tile);
            if (tileCell.isSwap()) {
                partial.handToSwap();
            } else {
                final Cell cell = tileCell.getDestination();
                partial.handToBoard(cell);
            }
        }
        partial.deactivate();
    }

    @Override
    public boolean involvesSwap() {
        for (TileCell tileCell : set) {
            if (tileCell.isSwap()) {
                return true;
            }
        }

        return false;
    }

    @Override
    final public boolean isPass() {
        return !resignFlag && set.isEmpty();
    }

    @Override
    public boolean isPureSwap() {
        if (resignFlag) {
            return false;
        }

        for (TileCell tileCell : set) {
            if (!tileCell.isSwap()) {
                return false;
            }
        }

        return true;
    }

    @Override
    final public boolean isResignation() {
        return resignFlag;
    }

    @Override
    public void place(Board board) {
        assert board != null;
        assert doesPlace();

        for (TileCell tileCell : set) {
            board.place(tileCell);
        }
    }

    @Override
    public boolean repeatsCell() {
        if (set.size() < 2) {
            return false;
        }

        final Cells cellsSeen = new Cells();

        for (TileCell tileCell : set) {
            final Cell cell = tileCell.getDestination();
            if (cell != null) {
                if (cellsSeen.contains(cell)) {
                    return true;
                } else {
                    cellsSeen.add(cell);
                }
            }
        }

        return false;
    }

    @Override
    public boolean repeatsTile() {
        if (set.size() < 2) {
            return false;
        }

        final Tiles tilesSeen = new Tiles();

        for (TileCell tileCell : set) {
            final Tile tile = tileCell.getTile();
            assert tile != null;

            if (tilesSeen.contains(tile)) {
                return true;
            } else {
                tilesSeen.add(tile);
            }
        }

        return false;
    }

    @Override
    public int size() {
        return set.size();
    }

    @Override
    public String toString() {
        String result = PREFIX;

        if (resignFlag) {
            result += RESIGN + SEPARATOR;
        }

        boolean firstFlag = true;
        for (TileCell tileCell : set) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }
            result += tileCell.toString();
        }
        result += SUFFIX;

        return result;
    }
}
