// File:     Board.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Board class for the Gold Tile Game
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

public class Board
    extends BaseBoard
    implements ReadBoard
{
    // constructors

    public Board() {
        super();
    }

    public Board(BaseBoard other) {
        super(other);
    }

    // methods, sorted by name

    private boolean areAllEmpty(Cells cells) {
        assert cells != null;

        for (Cell cell : cells) {
            if (!isEmpty(cell)) {
                return false;
            }
        }

        return true;
    }

    private boolean areAllInConnectedRun(Cells cells, Direction axis) {
        assert axis != null;
        assert Cell.isScoringAxis(axis);
        assert cells != null;


        if (cells.size() < 2) {
            return true;
        } else {
            final Cell sample = cells.first();
            final Cells run = getRun(sample, axis);
            return run.containsAll(cells);
        }
    }

    private boolean areAllRunsCompatible(Cells cells, Direction axis) {
        assert axis != null;
        assert Cell.isScoringAxis(axis);
        assert cells != null;

        final Cells doneCells = new Cells();  // to avoid repeating runs

        for (Cell start : cells) {
            if (!doneCells.contains(start)) {
                final Cells runCells = getRun(start, axis);
                final Tiles runTiles = getTiles(runCells);
                if (!runTiles.areAllCompatible()) {
                    return false;
                }
                doneCells.addAll(runCells);
            }
        }

        return true;
    }

    @Override
    public UserMessage checkMove(ReadMove move) {
        assert move != null;

        // A pass (no tiles played or swapped) is always valid.
        if (move.isPass()) {
            return null;
        }

        // Check for repeated tiles.
        if (move.repeatsTile()) {
            return UserMessage.REPEATED_TILE;
        }

        if (move.involvesSwap()) {
            if (move.isResignation()) {
                // a valid resignation
                return null;
            } else if (!move.isPureSwap()) {
                return UserMessage.SWAP;
            }
            // a valid swap
            return null;
        }
        assert move.doesPlace() : move;

        // Check for repeated cells.
        if (move.repeatsCell()) {
           return UserMessage.REPEATED_CELL;
        }

        // Get the set of board cells to be used.
        final Cells cells = move.copyCells();

        // Make sure all those cells are empty.
        if (!areAllEmpty(cells)) {
            return UserMessage.EMPTY_CELL;
        }

        if (isEmpty()) {
            if (!cells.isAnyStart()) {
                return UserMessage.START;
            }
        } else if (!doesAnyHaveUsedNeighbor(cells)) {
            return UserMessage.NEIGHBOR;
        }

        // Make a copy of the board and place the tiles on it.
        final Board after = new Board(this);
        move.place(after);

        Direction axisOfPlay = null;
        if (cells.size() > 1) {
            // Ensure all the cells used lie in a single connected run.
            for (Direction axis : getGameOpt().getGrid().getScoringAxes()) {
                if (after.areAllInConnectedRun(cells, axis)) {
                    axisOfPlay = axis;
                }
            }
            if (axisOfPlay == null) {
                return UserMessage.SCORING_AXIS;
            }
        }

        // Check compatibility of all runs involving these tiles
        for (Direction axis : getGameOpt().getGrid().getScoringAxes()) {
            if (!after.areAllRunsCompatible(cells, axis)) {
                if (axis.verticalFlag) {
                    return UserMessage.COLUMN_COMPATIBILITY;
                } else if (axis.horizontalFlag) {
                    return UserMessage.ROW_COMPATIBILITY;
                } else if (axis.diagonalFlag) {
                    return UserMessage.DIAGONAL_COMPATIBILITY;
                } else {
                    throw new AssertionError(axis);
                }
            }
        }

        return null;
    }

    @Override
    public boolean contains(Tile tile) {
        return find(tile) != null;
    }

    // Convert the entire board to a String for console output.
    @Override
    public String describe() {
        final int width = getGameOpt().getAttrCount() + 4;

        // blanks in upper left corner
        String result = StringExt.pad("", width);

        // column tags in top row
        for (int column = -getWestMax(); column <= getEastMax(); column++) {
            final String columnTag = String.format("%d", column);
            result += StringExt.pad(columnTag, width);
        }
        result += "\n";

        for (int row = getNorthMax(); row >= -getSouthMax(); row--) {
            final String rowTag = String.format("%d", row);
            result += StringExt.pad(rowTag, width);

            for (int column = -getWestMax(); column <= getEastMax(); column++) {
                final Cell cell = new Cell(row, column);
                final Tile tile = getContent(cell);
                if (tile == null) {
                    // Empty cell:  fill with dots.
                    result += " " + StringExt.repeat(width - 1, '.');
                } else {
                    final String description = tile.describe();
                    final String padded = StringExt.pad(description, width - 3);
                    result += String.format(" [%s]", padded);
                }
            }
            result += "\n";
        }

        return result;
    }

    private boolean doesAnyHaveUsedNeighbor(Cells cells) {
        assert cells != null;

        for (Cell cell : cells) {
            if (hasUsedNeighbor(cell)) {
                return true;
            }
        }

        return false;
    }

    @Override
    public int getBottomUseRow() {
        final Grid grid = getGameOpt().getGrid();
        return -(grid.getRowFringe() + getSouthMax());
    }

    private static ReadGameOpt getGameOpt() {
        return Game.getInstance().getOpt();
    }

    @Override
    public int getLeftUseColumn() {
        final Grid grid = getGameOpt().getGrid();
        return -(grid.getColumnFringe() + getWestMax());
    }

    @Override
    public int getRightUseColumn() {
        final Grid grid = getGameOpt().getGrid();
        return grid.getColumnFringe() + getEastMax();
    }

    private Cells getRun(Cell start, Direction direction) {
        assert start != null;
        assert direction != null;
        assert Cell.isScoringAxis(direction) : direction;
        assert start.isValid() : start;
        assert !isEmpty(start) : getContent(start);

        final Cells result = new Cells(start);

        /*
         * look in the negative direction
         * stop at first invalid/empty/duplicate cell
         */
        Cell current = start;
        Direction opposite = direction.getOpposite();
        for (;;) {
            final Cell previous = new Cell(current, opposite);
            if (!previous.isValid()
                    || isEmpty(previous)
                    || result.contains(previous))
            {
                break;
            }
            result.add(previous);
            current = previous;
        }

        /*
         * look in the positive direction
         * stop at first invalid/empty/duplicate cell
         */
        current = start;
        for (;;) {
            final Cell next = new Cell(current, direction);
            if (!next.isValid()
                    || isEmpty(next)
                    || result.contains(next))
            {
                break;
            }
            result.add(next);
            current = next;
        }

        assert !result.isEmpty() : start;
        return result;
    }

    private Tiles getTiles(Cells cells) {
        assert cells != null;

        final Tiles result = new Tiles();

        for (Cell cell : cells) {
            final Tile tile = getContent(cell);
            assert tile != null : cell;
            result.add(tile);
        }

        return result;
    }

    @Override
    public int getTopUseRow() {
        final Grid grid = getGameOpt().getGrid();

        return grid.getRowFringe() + getNorthMax();
    }

    @Override
    public boolean hasUsedNeighbor(Cell cell) {
        assert cell != null;
        assert cell.isValid() : cell;

        for (Direction direction : Direction.values()) {
            if (cell.hasNeighbor(direction)) {
                final Cell look = new Cell(cell, direction);
                assert look.isValid() : look;
                if (!isEmpty(look)) {
                    return true;
                }
            }
        }

        return false;
    }

    @Override
    public boolean isEmpty() {
        return size() == 0;
    }

    @Override
    public boolean isEmpty(Cell cell) {
        assert cell != null;

        return getContent(cell) == null;
    }

    @Override
    public boolean mightUse(Cell cell) {
        assert cell != null;

        final int column = cell.getColumn();
        if (column < getLeftUseColumn()) {
            return false;
        } else if (column > getRightUseColumn()) {
            return false;
        }

        final int row = cell.getRow();
        if (row < getBottomUseRow()) {
            return false;
        } else if (row > getTopUseRow()) {
            return false;
        }

        return true;
    }

    public void place(TileCell tileCell) {
        assert tileCell != null;

        final Cell cell = tileCell.getDestination();
        assert cell != null;

        final Tile tile = tileCell.getTile();
        assert tile != null;
        assert isEmpty(cell) : cell;

        place(cell, tile);
    }

    private int score(Cell start, Direction axis) {
        assert start != null;
        assert start.isValid();
        assert !isEmpty(start);
        assert Cell.isScoringAxis(axis);

        final Cells cells = getRun(start, axis);
        final int runLength = cells.size();

        int result = 0;

        if (runLength > 1) {
            result = runLength; // base score

            final Tiles tiles = getTiles(cells);
            assert tiles.areAllCompatible() : tiles;

            final int bonusFactor = tiles.getBonusFactor();
            result *= bonusFactor;

            // special bonus for two-attribute games
            if (getGameOpt().getAttrCount() == 2) {
                // Determine the common attribute.
                final int commonAttrIndex = tiles.firstMatchingAttr();
                final int otherAttrIndex = 1 - commonAttrIndex;
                final int maxLength = getGameOpt().
                        getAttrValueCount(otherAttrIndex);

                // double the bonus if at max length
                assert runLength <= maxLength : runLength;
                if (runLength == maxLength) {
                    result *= 2;
                }
            }
        }

        return result;
    }

    // score an (already-placed) move
    @Override
    public int score(ReadMove move) {
        assert move != null;

        int result = 0;
        final Cells cells = move.copyCells();

        for (Direction axis : getGameOpt().getGrid().getScoringAxes()) {
            final Cells done = new Cells();

            for (Cell start : cells) {
                if (!done.contains(start)) {
                    result += score(start, axis);

                    final Cells run = getRun(start, axis);
                    done.addAll(run);
                }
            }
        }

        return result;
    }

    public void unplace(TileCell tileCell) {
        assert tileCell != null;

        final Cell cell = tileCell.getDestination();
        assert cell != null;

        final Tile tile = tileCell.getTile();
        assert tile != null;
        assert getContent(cell).equals(tile);

        clear(cell);
    }
}
