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
    protected Board board = new Board();
    private Cells hintedCells = null;
    private Fraction skipProbability = null;
    protected Game game = null;
    private Hint hintStrength = null;
    private int playedTileCnt = 0;
    private Tile activeTile = null;
    private Tiles playableTiles = new Tiles();
    private Tiles swapTiles = new Tiles();

    // constructor
    
    Partial(Game g, Hint strength, Fraction skip) {
        reset(g, strength, skip);
    }
    
    // methods
    
    public boolean contains(Tile tile) {
        return false; // TODO    
    }
    
    public int countSwap() {
        return 0; // TODO
    }
    
    public boolean hasGame() {
        return game != null;    
    }
    
    public Tile getActiveTile() {
        return activeTile;
    }
    
    public Tile getCellTile(Cell cell) {
        return board.getCell(cell);
    }
    
    public boolean isActive(Tile tile) {
        return tile == null && activeTile == null || tile.equals(activeTile);
    }
    
    public boolean isEmpty(Cell cell) {
        return board.getCell(cell) == null;
    }
    
    public boolean isHinted(Cell cell) {
        return true; // TODO
    }
    
    public boolean isInSwap(Tile tile) {
        return false; // TODO
    }
    
    // reset method invoked by takeback
    public void reset() {
        activeTile = null;
        hintedCells = null;
        playedTileCnt = 0;
        swapTiles.clear();
        if (hasGame()) {
            board = game.copyBoard();
            playableTiles = game.copyPlayableTiles();
        } else {
            board.clear();
            playableTiles.clear();
        }
    }
    
    // reset method invoked at the start of a turn
    public void reset(Fraction skip) {
        assert skip.toDouble() < 1.0 : skip;

        skipProbability = skip;
        reset();
    }
    
    // reset method invoked at the start of a game
    public void reset(Game g, Hint strength, Fraction skip) {
        game = g;
        hintStrength = strength;
        reset(skip);
    }
}
