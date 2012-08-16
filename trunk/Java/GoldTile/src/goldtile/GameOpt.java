// File:     GameOpt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  GameOpt class for the Gold Tile Game
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
import java.util.Date;

public class GameOpt {
    // constants
    final private static int BONUS_PERCENT_DEFAULT = 0;
    final private static int CLONES_PER_COMBO_DEFAULT = 2;
    final private static int HANDS_DEALT_DEFAULT = 2;
    final private static int HANDS_DEALT_MIN = 1;
    final private static int HAND_SIZE_DEFAULT = 6;
    final private static int HAND_SIZE_MIN = 1;
    final private static int MINUTES_PER_HAND_DEFAULT = 30;
    final private static int MINUTES_PER_HAND_MIN = 2;
    final private static long SEED_DEFAULT = 123L;

    // per-instance fields
    private boolean boardWrapFlag;   
                      // used only if board is finite on one or both dimensions
    private boolean randomizeFlag;
    private GameStyle style;
    private Grid grid;
    private int attrCount;
    private int boardHeight;
    private int boardWidth;
    private int clonesPerCombo;
    private int handsDealt;      // number of hands dealt
    private int handSize;        // maximum number of tiles in a hand
    private int maxAttrValues[]; // maximum value of each attribute
    private int minutesPerHand;  // used only with GameStyle.CHALLENGE
    public long seed;            // used only if randomizeFlag == false
    public Percent bonusPercent;    // bonus tile percentage
    public Rules rules;
    
    // constructors
    
    public GameOpt() {
        minutesPerHand = MINUTES_PER_HAND_DEFAULT;
        randomizeFlag = true;
        seed = SEED_DEFAULT;
        style = GameStyle.getDefault();
        
        standardize();
        validate();
    }
    
    public GameOpt(GameOpt other) {
        assert other != null;
        
        boardWrapFlag = other.boardWrapFlag;
        randomizeFlag = other.randomizeFlag;
        style = other.style;
        grid = other.grid;
        attrCount = other.attrCount;
        boardHeight = other.boardHeight;
        boardWidth = other.boardWidth;
        clonesPerCombo = other.clonesPerCombo;
        handsDealt = other.handsDealt;
        handSize = other.handSize;
        
        maxAttrValues = new int[other.maxAttrValues.length];
        System.arraycopy(other.maxAttrValues, 0,
                             maxAttrValues, 0, other.maxAttrValues.length);
        
        minutesPerHand = other.minutesPerHand;
        seed = other.seed;
        bonusPercent = new Percent(other.bonusPercent);
        rules = other.rules;
        
        validate();
    }
    
    // methods
    
    public static GameOpt chooseConsole() {
        GameOpt result = new GameOpt();
        
        final String attrReport = result.reportAttrs();
        Global.print(attrReport);

        result.handsDealt = -1;
        while (result.handsDealt < HANDS_DEALT_MIN) {
            Global.print("Deal how many hands? ");
            String line = Global.readLine();
            try {
                result.handsDealt = Integer.parseInt(line);
            } catch (NumberFormatException exception) {
            }
        }
        Global.print("\n");

        result.handSize = -1;
        while (result.handSize < HAND_SIZE_MIN) {
            Global.print("How many tiles per hand? ");
            String line = Global.readLine();
            try {
                result.handSize = Integer.parseInt(line);
            } catch (NumberFormatException exception) {
            }
        }
        Global.print("\n");

        /*
         * Clone tiles so that there are enough to fill each hand at 
         * least three times.
         */
        final long tilesNeeded = 3 * result.handSize * result.handsDealt;
        final long comboCount = result.countCombos();
        result.clonesPerCombo = (int)(tilesNeeded / comboCount);
        
        return result;
    }
    
    public int[] copyValueMax() {
        int[] result = new int[attrCount];
        System.arraycopy(maxAttrValues, 0, result, 0, attrCount);
                
        return result;
    }
    
    public long countCombos() {
        long result = 1;
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            result *= getAttrValueCount(iAttr);
        }
        
        return result;
    }

    public boolean doesBoardWrap() {
        return boardWrapFlag;
    }
    
    public int getAttrCount() {
        return attrCount;
    }
    
    public int getAttrValueCount(int iAttr) {
        return 1 + getMaxAttrValue(iAttr);
    }
    
    public int getBoardHeight() {
        return boardHeight;
    }
    
    public int getBoardWidth() {
        return boardWidth;
    }

    public Grid getGrid() {
        return grid;
    }
    
    public int getHandsDealt() {
        return handsDealt;   
    }
    
    public int getHandSize() {
        return handSize;
    }
    
    public int getClonesPerCombo() {
        return clonesPerCombo;
    }
    
    public int getMaxAttrValue(int iAttr) {
        return maxAttrValues[iAttr];
    }
    
    public int getTilesPerCombo() {
        return 1 + getClonesPerCombo();
    }

    public boolean hasFiniteHeight() {
        return boardHeight < Cell.HEIGHT_MAX;
    }

    public boolean hasFiniteWidth() {
        return boardWidth < Cell.WIDTH_MAX;
    }
    
    public boolean isChallenge() {
        return style == GameStyle.CHALLENGE;    
    }
    
    public String reportAttrs() {
        String result = "Each tile has ";
        result += StringExt.plural(attrCount, "attribute") + ":\n";
        
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            result += " The " + StringExt.ordinal(iAttr + 1);
            result += " attribute ranges from ";

            final AttrMode mode = AttrMode.getConsoleDefault(iAttr);
            result += mode.attrToString(0) + " to ";
            final int valueMax = getMaxAttrValue(iAttr);
            result += mode.attrToString(valueMax) + ".\n";
        }
        result += "\n";

        return result;
    }
    

    public void reseedGenerator() {
        long s = seed; 
        if (randomizeFlag || style != GameStyle.DEBUG) {
            final Date date = new Date();
            seed = date.getTime();
        }
        Global.reseedGenerator(seed);
    }
    
    public void standardize() {
        attrCount = Combo.ATTR_COUNT_DEFAULT;
        maxAttrValues = new int[attrCount];    
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            maxAttrValues[iAttr] = Combo.VALUE_COUNT_DEFAULT - 1;
        }
        boardHeight = Cell.HEIGHT_MAX;
        boardWidth = Cell.WIDTH_MAX;
        bonusPercent = new Percent(BONUS_PERCENT_DEFAULT);
        clonesPerCombo = CLONES_PER_COMBO_DEFAULT;
        boardWrapFlag = false;
        grid = Grid.getDefault();
        handsDealt = HANDS_DEALT_DEFAULT;
        handSize = HAND_SIZE_DEFAULT;
        // don't set minutesPerHand
        // don't set randomizeFlag
        rules = Rules.STANDARD;
        // don't set seed
        // don't set style
    }
    
    public void validate() {
        assert attrCount <= Combo.ATTR_COUNT_MAX : attrCount;
        assert attrCount >= Combo.ATTR_COUNT_MIN : attrCount;
        
        assert Global.isEven(boardHeight) : boardHeight;
        assert boardHeight <= Cell.HEIGHT_MAX : boardHeight;
        assert boardHeight >= Cell.HEIGHT_MIN : boardHeight;
        
        assert Global.isEven(boardWidth) : boardWidth;
        assert boardWidth >= Cell.WIDTH_MIN : boardWidth;
        assert boardWidth <= Cell.WIDTH_MAX : boardWidth;
        
        assert bonusPercent != null;
        assert grid != null;
        assert handsDealt >= HANDS_DEALT_MIN : handsDealt;
        assert handSize >= HAND_SIZE_MIN : handSize;
        assert maxAttrValues != null;
        assert maxAttrValues.length >= attrCount : maxAttrValues.length;
        assert minutesPerHand >= MINUTES_PER_HAND_MIN : minutesPerHand;
        assert rules != null;
        assert style == GameStyle.DEBUG || randomizeFlag : style;
        assert style != null;
    }

}
