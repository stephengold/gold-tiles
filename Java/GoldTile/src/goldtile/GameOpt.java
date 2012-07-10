// File:     GameOpt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  TileOpt class for the Gold Tile Game
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
import java.util.*;
public class GameOpt {
    private short    attrCnt;
    private int      boardHeight;
    private int      boardWidth;
    public Percent   bonusPercent;    // bonus tile percentage
    private short    clonesPerCombo;
    public boolean   doesBoardWrap;   
                      // used only if board is finite on one or both dimensions
    public Grid      grid;
    private short    handsDealt;      // number of hands dealt
    private short    handSize;        // maximum number of tiles in a hand
    private short    maxAttrValues[]; // maximum value of each attribute
    private short    minutesPerHand;  // used only with GAME_STYLE_CHALLENGE
    public boolean   randomizeFlag;
    public Rules     rules;
    private long     seed;            // used only if mRandomizeFlag == false
    public GameStyle style;
    
    // static fields
    
    final static private short BONUS_PERCENT_DEFAULT = 0;
    final static private short CLONES_PER_COMBO_DEFAULT = 2;
    final static private short HANDS_DEALT_DEFAULT = 2;
    final static private short HANDS_DEALT_MIN = 1;
    final static private short HAND_SIZE_DEFAULT = 6;
    final static private short HAND_SIZE_MIN = 1;
    final static private short MINUTES_PER_HAND_DEFAULT = 30;
    final static private short MINUTES_PER_HAND_MIN = 2;
    final static private long SEED_DEFAULT = 123L;

    // constructors
    
    public GameOpt() {
        minutesPerHand = MINUTES_PER_HAND_DEFAULT;
        randomizeFlag = true;
        seed = SEED_DEFAULT;
        style = GameStyle.defaultStyle();
        
        standardize();
        validate();
    }
    
    // methods
    
    public short attrCnt() {
        return attrCnt;
    }
    
    public int boardHeight() {
        return boardHeight;
    }
    
    public int boardWidth() {
        return boardWidth;
    }
    
    public boolean hasFiniteHeight() {
        return boardHeight < Cell.HEIGHT_MAX;
    }

    public boolean hasFiniteWidth() {
        return boardWidth < Cell.WIDTH_MAX;
    }
    
    public void reseedGenerator() {
        long s = seed; 
        if (randomizeFlag || style != GameStyle.GAME_STYLE_DEBUG) {
            final Date date = new Date();
            seed = date.getTime();
        }
        Fraction.reseedGenerator(seed);
    }
    
    public void standardize() {
        attrCnt = Combo.ATTR_CNT_DEFAULT;
        maxAttrValues = new short[attrCnt];    
        for (short iAttr = 0; iAttr < attrCnt; iAttr++) {
            maxAttrValues[iAttr] = Combo.VALUE_CNT_DEFAULT - 1;
       }
       boardHeight = Cell.HEIGHT_MAX;
       boardWidth = Cell.WIDTH_MAX;
       bonusPercent = new Percent(BONUS_PERCENT_DEFAULT);
       clonesPerCombo = CLONES_PER_COMBO_DEFAULT;
       doesBoardWrap = false;
       grid = Grid.defaultGrid();
       handsDealt = HANDS_DEALT_DEFAULT;
       handSize = HAND_SIZE_DEFAULT;
       // don't set minutesPerHand
       // don't set randomizeFlag
       rules = Rules.STANDARD;
       // don't set seed
       // don't set style
    }
    
    public short tilesPerCombo() {
        short cnt = clonesPerCombo;
        cnt++;
        
        return cnt;
    }
    
    public void validate() {
        assert attrCnt >= Combo.ATTR_CNT_MIN;
        assert attrCnt <= Combo.attrCntMax();
        assert Global.isEven(boardHeight);
        assert boardHeight >= Cell.HEIGHT_MIN;
        assert boardHeight <= Cell.HEIGHT_MAX;
        assert Global.isEven(boardWidth);
        assert boardWidth >= Cell.WIDTH_MIN;
        assert boardWidth <= Cell.WIDTH_MAX;
        assert bonusPercent != null;
        if (Global.consoleFlag) {
            assert !doesBoardWrap;
            assert grid == Grid.GRID_4WAY;
        }
        assert grid != null;
        assert handsDealt >= HANDS_DEALT_MIN;
        assert handSize >= HAND_SIZE_MIN;
        assert maxAttrValues != null;
        assert maxAttrValues.length >= attrCnt;
        assert minutesPerHand >= MINUTES_PER_HAND_MIN;
        assert rules != null;
        assert style == GameStyle.GAME_STYLE_DEBUG || randomizeFlag;
        assert style != null;
        if (Global.consoleFlag) {
            assert style != GameStyle.GAME_STYLE_CHALLENGE;
        }
    }

    public short[] valueMax() {
        short[] result = new short[attrCnt];
        System.arraycopy(maxAttrValues, 0, result, 0, attrCnt);
                
        return result;
    }
}
