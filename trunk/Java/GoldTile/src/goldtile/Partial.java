// File:     Partial.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Partial class for the Gold Tile Game
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

public class Partial {
    // enums
    public static enum Active { EXCLUDED, INCLUDED };

    // constants
    final private static Fraction SUGGEST_SKIP_PROBABILITY = new Fraction(0.0);

    // per-instance fields (all mutable), sorted by type
    private Board board = new Board();
    private Cells hintedCells = null;       // cached data, null means invalid
    private Fraction skipProbability = null;
    private HintStrength hintStrength = null;
    private int playedTileCount = 0;        // cached data, keep updated
    private Integer pointCount = null;      // cached data, null means invalid
    private Tile activeTile = null;
    private Tiles playableTiles = new Tiles();
    private Tiles swapTiles = new Tiles();

    // constructors

    public Partial() {
        takeBackTiles();
        hintStrength = HintStrength.NONE;
        skipProbability = HandOpt.SKIP_PROBABILITY_DEFAULT;
    }

    private Partial(Partial other) {
        copy(other);
    }

    // methods, sorted by name

    public void activate(Tile tile) {
        assert isPlayable(tile);
        assert Game.hasInstance();

        if (activeTile == null || !tile.equals(activeTile)) {
            activeTile = tile;

            hintedCells = null;
            // no effect on playedTileCount
            // no effect on pointCount
        }
    }

    /**
     * Add cells from "base" to "hintedCells" if they are valid
     * uses for the specified Tile.
     */
    private void addValidUses(ReadMove move, Tile tile, Cells base) {
        assert move != null;
        assert tile != null;
        assert base != null;

        for (Cell cell : base) {
            if (!hintedCells.contains(cell) &&
                    isValidNextStep(move, cell, tile))
            {
                hintedCells.add(cell);
            }
        }
    }

    public void assertActive(Place expectedPlace) {
        final Place actualPlace = findActiveTile();
        assert actualPlace.equals(expectedPlace) : actualPlace;
    }

    public ReadMove autoPlay() {
        assert Game.hasInstance();
        final ReadHandOpt handOpt = Game.getInstance().getPlayable().getOpt();
        assert handOpt.isAutomatic();

        final HintStrength saveStrength = hintStrength;
        final Fraction probability = handOpt.getSkipProbability();
        ReadMove result;
        try {
            final Partial best = findBestMove(probability);
            result = best.getMove(Partial.Active.INCLUDED);
            assert Game.getInstance().checkMove(result) == null;
        } catch (InterruptedException exception) {
            result = new Move(playableTiles);  // resign
        }

        return result;
    }

    private void boardToHand() {
        assertActive(Place.BOARD);

        final Cell cell = board.find(activeTile);
        board.clear(cell);

        hintedCells = null;
        --playedTileCount;
        pointCount = null;

        assertActive(Place.HAND);
    }

    public boolean canSwapAll() {
        return Game.hasInstance() &&
                Game.getInstance().countStock() >= countPlayableTiles();
        // note: doesn't consider Game.mustPlay
    }

    public void changeHand() {
        final ReadHand playable = Game.getInstance().getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();
        skipProbability = handOpt.getSkipProbability();

        takeBackTiles();
    }

    final public void copy(Partial other) {
        board = new Board(other.board);
        hintedCells = (other.hintedCells == null) ? null :
                new Cells(other.hintedCells);
        skipProbability = other.skipProbability;
        hintStrength = other.hintStrength;
        playedTileCount = other.playedTileCount;
        pointCount = (other.pointCount == null) ? null :
                new Integer(other.pointCount);
        activeTile = other.activeTile;
        playableTiles = new Tiles(other.playableTiles);
        swapTiles = new Tiles(other.swapTiles);
    }

    public int countHandTiles() {
        assert countPlayableTiles() >= countPlayedTiles() + countSwappedTiles()
                : countPlayedTiles();

        return countPlayableTiles() - countPlayedTiles() - countSwappedTiles();
    }

    public int countHintedCells() {
        if (hintedCells == null) {
            updateHintedCells();
            assert hintedCells != null;
        }
        return hintedCells.size();
    }

    public int countPlayableTiles() {
        return playableTiles.size();
    }

    public int countPlayedTiles() {
        return playedTileCount;
    }

    public int countSwappedTiles() {
        return swapTiles.size();
    }

    public Tile deactivate() {
        assert Game.hasInstance();

        final Tile result = activeTile;

        if (activeTile != null) {
            activeTile = null;

            hintedCells = null;
            // no effect on playedTileCount
            // no effect on pointCount
        }

        return result;
    }

    public Place find(Tile tile) {
        if (isInSwap(tile)) {
            assert !board.contains(tile) : board.find(tile);
            return Place.SWAP_AREA;
        } else if (board.contains(tile)) {
            return Place.BOARD;
        } else if (isPlayable(tile)) {
            return Place.HAND;
        } else {
            return null;
        }
    }

    public Place findActiveTile() {
        return find(activeTile);
    }

    static public Partial findBestMove(Fraction skipProbability)
            throws InterruptedException
    {
        assert skipProbability != null;
        assert Game.hasInstance();

        if (Game.getInstance().getMustPlay() > 0) {
            // Consider ALL possibile moves.
            skipProbability = new Fraction(0.0);
        }

        final Partial partial = new Partial();
        partial.takeBackTiles();
        partial.setSkipProbability(skipProbability);
        partial.setHintStrength(HintStrength.USABLE_BY_ACTIVE);

        Partial result = new Partial(partial); // a temporary copy

        // Search for good plays.
        result = partial.findBestPlay(result);
        assert result.playedTileCount >= Game.getInstance().getMustPlay();

        if (result.getPointCount() == 0) {
            // Construct a swap.
            result.takeBackTiles();

            int swapCount = result.countPlayableTiles();
            final int maxSwap = Game.getInstance().countStock();
            if (swapCount > maxSwap) {
                swapCount = maxSwap;
            }

            // TODO be more selective in choosing tiles to swap
            for (Tile tile : result.playableTiles) {
                if (swapCount <= 0) {
                    break;
                }
                result.activate(tile);
                result.handToSwap();
                swapCount--;
            }
        }

        // note: result may incorporate an active tile

        return result;
    }

    // RECURSIVE
    private Partial findBestPlay(Partial bestSoFar)
            throws InterruptedException
    {
        assert bestSoFar != null;
        assert Game.hasInstance();

        Partial result = bestSoFar;
        if (getPointCount() > bestSoFar.getPointCount()) {
            result.copy(this);
        }

        final Tile saveActive = deactivate();

        for (Tile tile : playableTiles) {
            if (isInHand(tile) && !skipProbability.randomBoolean()) {
                if (Thread.interrupted()) {
                    throw new InterruptedException();
                }
                activate(tile);
                updateHintedCells();
                for (Cell cell : hintedCells) {
                    if (Thread.interrupted()) {
                        throw new InterruptedException();
                    }
                    handToBoard(cell);
                    result = findBestPlay(bestSoFar);
                    boardToHand();
                }
                deactivate();
            }
        }

        if (saveActive != null) {
            activate(saveActive);
        }

        return result;
    }

    public Cell firstHintedCell() {
        if (hintedCells == null) {
            updateHintedCells();
            assert hintedCells != null;
        }
        assert hintedCells.size() > 0;

        return hintedCells.first();
    }

    public Tile getActiveTile() {
        return activeTile;
    }

    public ReadBoard getBoard() {
        return board;
    }

    public Tiles getBoardTiles() {
        final Tiles result = new Tiles();

        for (Tile tile : playableTiles) {
            if (board.contains(tile)) {
                result.add(tile);
            }
        }

        return result;
    }

    public HintStrength getHintStrength() {
        return hintStrength;
    }

    public ReadMove getMove(Active includeActive) {
        final Move result = new Move();

        for (Tile tile : playableTiles) {
            if (includeActive == Active.INCLUDED || !isActive(tile)) {
                final Cell destination = board.find(tile);
                if (destination != null || isInSwap(tile)) {
                    result.add(tile, destination);
                }
            }
        }

        return result;
    }

    public int getPointCount() {
        if (pointCount == null) {
            updatePointCount();
            assert pointCount != null;
        }

        return pointCount;
    }


    public void handToBoard(Cell cell) {
        assert cell != null;
        assert cell.isValid();
        assert board.isEmpty(cell);
        assertActive(Place.HAND);

        board.place(cell, activeTile);

        hintedCells = null;
        playedTileCount++;
        pointCount = null;

        assertActive(Place.BOARD);
    }

    public void handToSwap() {
        assertActive(Place.HAND);

        swapTiles.add(activeTile);

        hintedCells = null;
        // no effect on playedTileCount
        // no effect on pointCount

        assertActive(Place.SWAP_AREA);
    }

    public boolean hasActiveTile() {
        return activeTile != null;
    }

    public boolean isActive(Tile tile) {
        if (tile == null) {
            return activeTile == null;
        } else {
            return tile.equals(activeTile);
        }
    }

    public boolean isHinted(Cell cell) {
        assert cell != null;

        if (hintedCells == null) {
            updateHintedCells();
            assert hintedCells != null;
        }

        return hintedCells.contains(cell);
    }

    public boolean isInHand(Tile tile) {
        if (tile == null) {
            return false;
        } else {
            return isPlayable(tile) && !isInSwap(tile) && !board.contains(tile);
        }
    }

    public boolean isInSwap(Tile tile) {
        if (tile == null) {
            return false;
        } else {
            return swapTiles.contains(tile);
        }
    }

    public boolean isPass() {
        return countPlayedTiles() + countSwappedTiles() == 0;
    }

    public boolean isPlayable(Tile tile) {
        if (tile == null) {
            return false;
        } else {
            return playableTiles.contains(tile);
        }
    }

    /**
     * Check whether a hypothetical next step would be a legal
     * partial move.
     * @param base
     * @param cell
     * @param tile
     * @return true or false
     */
    private boolean isValidNextStep(ReadMove base, Cell cell, Tile tile) {
        assert base != null;
        assert cell != null;
        assert tile != null;
        assert Game.hasInstance();

        final Move move = new Move(base);
        move.add(tile, cell);
        final UserMessage reason = Game.getInstance().checkMove(move);

        return reason == null || reason == UserMessage.FIRST_TURN;
    }

    public void moveActiveTile(Place from, Place to, Cell toCell) {
        assert from != null;
        assert to != null;
        assert isPlayable(activeTile);

        // Move tile to hand -- temporarily in many cases.
        switch (from) {
            case BOARD:
                boardToHand();
                break;

            case SWAP_AREA:
                swapToHand();
                break;
        }

        // Move tile from hand to destination.
        switch (to) {
            case BOARD:
                assert toCell != null;
                assert toCell.isValid();

                handToBoard(toCell);
                break;

            case SWAP_AREA:
                handToSwap();
                break;
        }
    }

    public void nextHand() {
        Game.getInstance().nextHand();
        changeHand();
    }

    public void setHintStrength(HintStrength strength) {
        if (strength != null && !strength.equals(hintStrength)) {
            hintStrength = strength;
            hintedCells = null;
        }
    }

    public void setSkipProbability(Fraction skipProbability) {
        assert skipProbability != null;
        assert !skipProbability.isUnity();

        this.skipProbability = skipProbability;
    }

    public Partial suggest() {
        assert Game.hasInstance();
        assert Game.getInstance().getPlayable().getOpt().isLocalUser();

        final HintStrength saveStrength = hintStrength;

        Partial best;
        try {
            best = findBestMove(SUGGEST_SKIP_PROBABILITY);
            best.deactivate();
            best.setHintStrength(saveStrength);
        } catch (InterruptedException exception) {
            best = null;
        }

        return best;
    }

    public void swapAllTiles() {
        if (countSwappedTiles() < countPlayableTiles()) {
            takeBackTiles();
            swapTiles = new Tiles(playableTiles);
        }
    }

    private void swapToHand() {
        assertActive(Place.SWAP_AREA);

        swapTiles.remove(activeTile);
        hintedCells = null;
        // no effect on pointCount

        assertActive(Place.HAND);
    }

    final public void takeBackTiles() {
        activeTile = null;
        swapTiles.clear();

        hintedCells = null;
        playedTileCount = 0;
        pointCount = 0;

        if (Game.hasInstance()) {
            // TODO might be more efficient to modify the existing board copy
            board = Game.getInstance().copyBoard();
            playableTiles = Game.getInstance().getPlayable().copyContents();
        } else {
            board.clear();
            playableTiles.clear();
        }
    }

    private void updateHintedCells() {
        hintedCells = new Cells();
        if (hintStrength == HintStrength.NONE) {
            return;
        }

        // Add all valid empty cells.
        for (int row = board.getBottomUseRow();
                row <= board.getTopUseRow(); row++)
        {
            for (int column = board.getLeftUseColumn();
                    column <= board.getRightUseColumn(); column++)
            {
                final Cell cell = new Cell(row, column);
                final Cell wrapCell = cell.wrap();
                if (wrapCell.isValid() &&
                    board.isEmpty(wrapCell) &&
                    !hintedCells.contains(wrapCell))
                {
                    hintedCells.add(cell);
                }
            }
        }
        if (hintStrength == HintStrength.UNUSED) {
            return;
        }

        /*
         * Remove any cells that are neither the start cell nor
         * the neighbor of a used cell.
         */
        Cells base = new Cells(hintedCells);
        for (Cell cell : base) {
            if (!cell.isStart() && !board.hasUsedNeighbor(cell)) {
                hintedCells.remove(cell);
            }
        }
        if (hintStrength == HintStrength.CONNECTED) {
            return;
        }

        /*
         * Remove any cells that are not ...
         *   USABLE_BY_PLAYABLE:  usable by any playable tile
         *   USABLE_BY_ACTIVE:    usable by the active tile
         */
        base = new Cells(hintedCells);
        hintedCells.clear();
        final ReadMove move = getMove(Active.EXCLUDED);
        for (Tile tile : playableTiles) {
             boolean includeTile;
             if (hintStrength == HintStrength.USABLE_BY_ACTIVE && hasActiveTile()) {
                 includeTile = isActive(tile);
            } else {
                includeTile = (!board.contains(tile) || isActive(tile));
            }
            if (includeTile) {
                addValidUses(move, tile, base);
            }
        }
    }

    private void updatePointCount() {
        assert Game.hasInstance();

        pointCount = new Integer(0);

        final int mustPlay = Game.getInstance().getMustPlay();
        if (mustPlay == 0 || countPlayedTiles() >= mustPlay) {
            final ReadMove move = getMove(Active.INCLUDED);
            pointCount = board.score(move);
        }
    }
}
