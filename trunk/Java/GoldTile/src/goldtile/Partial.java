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
    
    // per-instance fields (all mutable), sorted by type
    private Board board = new Board();
    private Cells hintedCells = null;       // cached data, null means invalid
    private Fraction skipProbability = null;
    private Game game = null;
    private HintStrength hintStrength = null;
    private int playedTileCount = 0;        // cached data, keep updated
    private Integer pointCount = null;      // cached data, null means invalid
    private Tile activeTile = null;
    private Tiles playableTiles = new Tiles();
    private Tiles swapTiles = new Tiles();

    // constructors
    
    public Partial() {
        setGame(null);
    }
    
    private Partial(Partial other) {
        copy(other);       
    }
    
    // methods, sorted by name
    
    public void activate(Tile tile) {
        assert isPlayable(tile);
        assert hasGame();

        if (activeTile == null || !tile.equals(activeTile)) {
            activeTile = tile;
        
            hintedCells = null;
            // no effect on playedTileCount
            // no effect on pointCount
        }
    }
    
    /*
     * Add cells from "base" to "hintedCells" if they are valid
     * uses for the specified Tile.
     */
    private void addValidUses(Move move, Tile tile, Cells base) {
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
        return hasGame() && game.countStock() >= countPlayable();
        // note: doesn't consider Game.mustPlay
    }
    
    private void copy(Partial other) {
        board = new Board(other.board);
        hintedCells = (other.hintedCells == null) ? null : 
                new Cells(other.hintedCells);
        skipProbability = other.skipProbability;
        game = other.game;
        hintStrength = other.hintStrength;
        playedTileCount = other.playedTileCount;
        pointCount = (other.pointCount == null) ? null : 
                new Integer(other.pointCount);
        activeTile = other.activeTile;
        playableTiles = new Tiles(other.playableTiles);
        swapTiles = new Tiles(other.swapTiles);
    }
    
    public int countHand() {
        assert countPlayable() >= countPlayed() + countSwapped() 
                : countPlayed();

        return countPlayable() - countPlayed() - countSwapped();
    }
    
    public int countHintedCells() {
        if (hintedCells == null) {
            updateHintedCells();
            assert hintedCells != null;
        }
        return hintedCells.size();    
    }
    
    public int countPlayable() {
        return playableTiles.size();
    }
    
    public int countPlayed() {
        return playedTileCount;
    }
    
    public int countSwapped() {
        return swapTiles.size();
    }
    
    public Tile deactivate() {
        assert hasGame();
        
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
    
    // RECURSIVE
    private Partial findBestMove(Partial bestSoFar) {
        assert bestSoFar != null;
        assert hasGame();

        Partial result = bestSoFar;
        if (getPointCount() > bestSoFar.getPointCount()) {
            result.copy(this);
        }
        
        final Tile saveActive = deactivate();
                
        for (Tile tile : playableTiles) {
            if (isInHand(tile) && !skipProbability.randomBoolean()) {
                // TODO check for cancellation
                activate(tile);
                updateHintedCells();
                for (Cell cell : hintedCells) {
                    // TODO check for cancellation
                    handToBoard(cell);
                    result = findBestMove(bestSoFar);
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
    
    public void finishTurn(Move move) {
        assert move != null;
        
        game.finishTurn(move);
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
    
    public ReadGame getGame() {
        return game;
    }
    
    public HintStrength getHintStrength() {
        return hintStrength;
    }
    
    public Move getMove(Active includeActive) {
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
    
    public ReadHand getPlayable() {
        return game.getPlayable();    
    }
    
    public int getPointCount() {
        if (pointCount == null) {
            updatePointCount();
            assert pointCount != null;
        }
        
        return pointCount;
    }
    
    public GameStyle getStyle() {
        if (hasGame()) {
            final ReadGameOpt opt = game.getGameOpt();
            return opt.getStyle();
        } else {            
            return GameStyle.NONE;
        }
    }
    
    private void handToBoard(Cell cell) {
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
 
    private void handToSwap() {
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
    
    public boolean hasGame() {
        return game != null;    
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
        return countPlayed() + countSwapped() == 0;    
    }
    
    public boolean isPlayable(Tile tile) {
        if (tile == null) {
            return false;
        } else {
            return playableTiles.contains(tile);
        }
    }
    
    /*
     * Check whether a hypothetical next step would be a legal
     * partial move.
     */
    private boolean isValidNextStep(Move base, Cell cell, Tile tile) {
        assert base != null;
        assert cell != null;
        assert tile != null;
        assert hasGame();

        final Move move = new Move(base);
        move.add(tile, cell);
        final UserMessage reason = game.checkMove(move);

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
        game.nextHand();

        final ReadHand playable = game.getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();
        skipProbability = handOpt.getSkipProbability();

        takeBack();
    }
 
    // the start of a new game
    final public void setGame(Game game) {
        this.game = game;
        
        final ReadGameOpt gameOpt = (game == null) ? null : 
                game.getGameOpt();
        final ReadHandOpt handOpt = (game == null) ? null : 
                game.getPlayable().getOpt();
        final HintStrength strength = HintStrength.getDefault(gameOpt, handOpt);
        setHintStrength(strength);
        
        skipProbability = HandOpt.SKIP_PROBABILITY_DEFAULT;
        takeBack();
    }

    public void setHintStrength(HintStrength strength) {
        if (strength != null && !strength.equals(hintStrength)) {
            hintStrength = strength;  
            hintedCells = null;
        }
    }
    
    public void setSkipProbability(Fraction skipProbability) {
        this.skipProbability = skipProbability;
    }
    
    public void startClock() {
        assert hasGame();
        
        game.startClock();
    }
    
    public void suggest() {
        assert hasGame();

        final HintStrength saveStrength = hintStrength;
        setHintStrength(HintStrength.USABLE_BY_ACTIVE);
        takeBack();
        
        Partial bestSoFar = new Partial(this); // a temporary copy
        bestSoFar = findBestMove(bestSoFar);

        if (bestSoFar.getPointCount() == 0) {
            if (canSwapAll()) {
                swapAll();
            } // TODO - partial swaps
        } else {
            copy(bestSoFar);
            deactivate();
            setHintStrength(saveStrength);
        }    
    }
    
    public void swapAll() {
        assert !hasActiveTile();
        assert canSwapAll();

        if (countSwapped() < countPlayable()) {
            swapTiles = new Tiles(playableTiles);
        
            hintedCells = null;
            playedTileCount = 0;
            pointCount = 0;
        }
    }
    
    private void swapToHand() {
        assertActive(Place.SWAP_AREA);
        
        swapTiles.remove(activeTile);
        hintedCells = null;
        // no effect on pointCount
        
        assertActive(Place.HAND);
    }
    
    final public void takeBack() {
        activeTile = null;
        swapTiles.clear();
        
        hintedCells = null;
        playedTileCount = 0;
        pointCount = null;
        
        if (hasGame()) {
            board = game.copyBoard();
            playableTiles = getPlayable().copyContents();
        } else {
            board.clear();
            playableTiles.clear();
        }
    }
    
    public void togglePause() {
        assert hasGame();
        
        game.togglePause();
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
         * Remove any cells that are not usable ...
         *   USABLE_BY_PLAYABLE:  by any playable tile
         *   USABLE_BY_ACTIVE:    by the active tile
         */
        base = new Cells(hintedCells);
        hintedCells.clear();
        final Move move = getMove(Active.EXCLUDED);
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
        assert hasGame();
        
        pointCount = new Integer(0);
        
        final int mustPlay = game.getMustPlay();
        if (mustPlay == 0 || countPlayed() >= mustPlay) {
            final Move move = getMove(Active.INCLUDED);
            pointCount = board.score(move);
        }
    }
}
