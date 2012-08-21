// File:     ReadGameOpt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ReadGameOpt interface for the Gold Tile Game
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

public interface ReadGameOpt {
    public int[] copyValueMax();
    public long countCombos();
    public boolean doesBoardWrap();

    public int getAttrCount();
    public int getAttrValueCount(int iAttr);
    public int getBoardHeight();
    public int getBoardWidth();
    public Fraction getBonusFraction();
    public int getClonesPerCombo();
    public Grid getGrid();
    public int getHandsDealt();
    public int getHandSize();
    public Attr getLastAttr(int iAttr);
    public int getMaxAttrValue(int iAttr);
    public long getMillisecondsPerHand();
    public int getSecondsPerHand();
    public int getStuckThreshold();
    public GameStyle getStyle();
    public int getTilesPerCombo();
    
    public boolean hasFiniteHeight();
    public boolean hasFiniteWidth();
    public boolean isChallenge();
    public String reportAttrs();
    public void validate();
}
