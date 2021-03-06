// File:     ReadGame.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ReadGame interface for the Gold Tile Game
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

public interface ReadGame {
    public boolean canRedo();
    public boolean canUndo();
    public UserMessage checkMove(ReadMove move);
    public Board copyBoard();
    public int countStock();

    public ReadBoard getBoard();
    public Game.Ending getEnding();
    public ReadHand getHand(int iHand);
    public int getMustPlay();
    public ReadGameOpt getOpt();
    public ReadHand getPlayable();
    public int getPlayableIndex();
    public int[] getUnplayableIndices();
    public Strings getUserNames();
    public int getSeconds(int iHand);

    public boolean isFirstTurn();
    public boolean isLegalMove(ReadMove move);
    public boolean isOutOfTime();
    public boolean isOver();
    public boolean isPaused();
    public boolean isPlayable(int iHand);
    public boolean isStockEmpty();
    public boolean isWarmTile(Tile tile);
}
