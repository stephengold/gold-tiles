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

public class GameOpt 
    implements ReadGameOpt
{
    // constants
    final public static Dim BOARD_HEIGHT_DEFAULT = new Dim(12);
    final public static Dim BOARD_WIDTH_DEFAULT = new Dim(12);
    final private static int BONUS_PERCENT_DEFAULT = 0;
    final private static int CLONES_PER_COMBO_DEFAULT = 2;
    final private static int HANDS_DEALT_DEFAULT = 2;
    final private static int HANDS_DEALT_MIN = 1;
    final private static int HAND_SIZE_DEFAULT = 6;
    final private static int HAND_SIZE_MIN = 1;
    final public static int MINUTES_PER_HAND_DEFAULT = 30;
    final public static int MINUTES_PER_HAND_MAX = 120;
    final public static int MINUTES_PER_HAND_MIN = 2;
    final private static int STUCK_THRESHOLD_DEFAULT = 7;
    final private static int STUCK_THRESHOLD_MIN = 1;
    final public static long SEED_DEFAULT = 12345L;

    // per-instance fields, sorted by type
    private boolean boardWrapFlag;   
                      // used only if board is finite on one or both dimensions
    private boolean randomizeFlag;
    private Dim boardHeight;
    private Dim boardWidth;
    private GameStyle style;
    private Grid grid;
    private int attrCount;
    private int clonesPerCombo;
    private int handsDealt;      // number of hands dealt
    private int handSize;        // maximum number of tiles in a hand
    private int maxAttrValues[]; // maximum value of each attribute
    private int minutesPerHand;  // used only in challenge-style games
    private int stuckThreshold;  // turns before game is declared "stuck" 
    private long seed;           // used only in debug-style with randomizeFlag == false
    private Percent bonusPercent;    // bonus tile percentage
    private Rules rules;
    
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
        stuckThreshold = other.stuckThreshold;
        seed = other.seed;
        bonusPercent = new Percent(other.bonusPercent);
        rules = other.rules;
        
        validate();
    }
    
    // methods
    
    public static GameOpt chooseConsole() {
        final GameOpt result = new GameOpt();
        
        result.setRandomize(false);
        
        final String attrReport = result.reportAttrs();
        Console.print(attrReport);

        result.handsDealt = -1;
        while (result.handsDealt < HANDS_DEALT_MIN) {
            String line = Console.readLine("Deal how many hands? ");
            try {
                result.handsDealt = Integer.parseInt(line);
            } catch (NumberFormatException exception) {
            }
        }
        Console.printLine();

        result.handSize = -1;
        while (result.handSize < HAND_SIZE_MIN) {
            String line = Console.readLine("How many tiles per hand? ");
            try {
                result.handSize = Integer.parseInt(line);
            } catch (NumberFormatException exception) {
            }
        }
        Console.printLine();

        /*
         * Clone tiles so that there are enough to fill each hand at 
         * least three times.
         */
        final long tilesNeeded = 3 * result.handSize * result.handsDealt;
        final long comboCount = result.countCombos();
        result.clonesPerCombo = (int)(tilesNeeded / comboCount);
        
        return result;
    }
    
    @Override
    public int[] copyValueMax() {
        int[] result = new int[attrCount];
        System.arraycopy(maxAttrValues, 0, result, 0, attrCount);
                
        return result;
    }
    
    @Override
    public long countCombos() {
        long result = 1;
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            result *= getAttrValueCount(iAttr);
        }
        
        return result;
    }

    @Override
    public boolean doesBoardWrap() {
        return boardWrapFlag;
    }
    
    @Override
    public int getAttrCount() {
        return attrCount;
    }
    
    @Override
    public int getAttrValueCount(int iAttr) {
        return 1 + getMaxAttrValue(iAttr);
    }
    
    @Override
    public Dim getBoardHeight() {
        return boardHeight;
    }
    
    @Override
    public Dim getBoardWidth() {
        return boardWidth;
    }

    @Override
    public Fraction getBonusFraction() {
        return bonusPercent.toFraction();    
    }
    
    @Override
    public long getCellCount() {
        final long product = boardWidth.times(boardHeight);
        if (product == Long.MAX_VALUE) {
            return product;
        } else {
            return product / grid.getSparsity();
        }    
    }
    
    @Override
    public int getClonesPerCombo() {
        return clonesPerCombo;
    }
    
    @Override
    public Grid getGrid() {
        return grid;
    }
    
    @Override
    public int getHandsDealt() {
        return handsDealt;   
    }
    
    @Override
    public int getHandSize() {
        return handSize;
    }
    
    @Override
    public Attr getLastAttr(int iAttr) {
        assert iAttr >= 0;

        return new Attr(getMaxAttrValue(iAttr));
    }
    
    @Override
    public int getMaxAttrValue(int iAttr) {
        assert iAttr >= 0;
        
        return maxAttrValues[iAttr];
    }

    @Override
    public long getMillisecondsPerHand() {
        return getSecondsPerHand() * Global.MILLISECONDS_PER_SECOND;
    }
    
    @Override
    public int getMinutesPerHand() {
        return minutesPerHand;
    }
    
    @Override
    public Rules getRules() {
        return rules;
    }
    
    @Override
    public int getSecondsPerHand() {
        return getMinutesPerHand() * Global.SECONDS_PER_MINUTE;
    }
    
    @Override
    public long getSeed() {
        return seed;
    }
    
    @Override
    public int getStuckThreshold() {
        return stuckThreshold;
    }
    
    @Override
    public GameStyle getStyle() {
        return style;
    }
    
    @Override
    public int getTilesPerCombo() {
        return 1 + getClonesPerCombo();
    }

    @Override
    public Topology getTopology() {
        if (hasFiniteHeight() && hasFiniteWidth()) {
            return doesBoardWrap() ? Topology.TORUS : Topology.RECTANGLE;
        } else if (hasFiniteHeight()) {
            return doesBoardWrap() ? Topology.HCYLINDER : Topology.HSTRIP;
        } else if (hasFiniteWidth()) {
            return doesBoardWrap() ? Topology.VCYLINDER : Topology.VSTRIP;
        } else {
            return Topology.FLAT;
        }
    }

    @Override
    public boolean hasFiniteHeight() {
        return !boardHeight.isEndless();
    }

    @Override
    public boolean hasFiniteWidth() {
        return !boardWidth.isEndless();
    }
    
    @Override
    public boolean isRandomized() {
        return randomizeFlag;
    }
    
    @Override
    public String reportAttrs() {
        String result = "Each tile has ";
        result += StringExt.plural(attrCount, "attribute") + ":\n";
        
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final AttrMode mode = AttrMode.getConsoleDefault(iAttr);

            result += String.format(" The %s attribute ranges from %s to %s.\n",
                    StringExt.ordinal(iAttr + 1),
                    mode.attrToString(new Attr(0)),
                    mode.attrToString(getLastAttr(iAttr)) );
        }
        result += "\n";

        return result;
    }
    
    public void reseedGenerator() {
        if (randomizeFlag || !style.isDebug()) {
            final Date date = new Date();
            seed = date.getTime();
        }
        Global.reseedGenerator(seed);
    }
    
    public void setBoardHeight(Dim boardHeight) {
        this.boardHeight = boardHeight;
    }
    
    public void setBoardWidth(Dim boardWidth) {
        this.boardWidth = boardWidth;
    }
    
    public void setBoardWrap(boolean doesWrap) {
        this.boardWrapFlag = doesWrap;
    }
    
    public void setGrid(Grid grid) {
        this.grid = grid;
    }
    
    public void setMinutesPerHand(int minutesPerHand) {
        this.minutesPerHand = minutesPerHand;
    }
    
    public void setRandomize(boolean randomizeFlag) {
        this.randomizeFlag = randomizeFlag;
    }
    
    public void setRules(Rules rules) {
        this.rules = rules;
    }
    
    public void setSeed(long seed) {
        this.seed = seed;
    }
    
    public void setStyle(GameStyle style) {
        assert style != null;
        
        this.style = style;
    }
    
    final public void standardize() {
        attrCount = Combo.ATTR_COUNT_DEFAULT;
        maxAttrValues = new int[attrCount];    
        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            maxAttrValues[iAttr] = Attr.MAX_DEFAULT;
        }
        boardHeight = Dim.endless();
        boardWidth = Dim.endless();
        bonusPercent = new Percent(BONUS_PERCENT_DEFAULT);
        clonesPerCombo = CLONES_PER_COMBO_DEFAULT;
        boardWrapFlag = false;
        grid = Grid.getDefault();
        handsDealt = HANDS_DEALT_DEFAULT;
        handSize = HAND_SIZE_DEFAULT;
        // don't set minutesPerHand
        stuckThreshold = STUCK_THRESHOLD_DEFAULT;
        // don't set randomizeFlag
        rules = Rules.STANDARD;
        // don't set seed
        // don't set style
    }

    @Override
    final public void validate() {
        assert attrCount <= Combo.ATTR_COUNT_MAX : attrCount;
        assert attrCount >= Combo.ATTR_COUNT_MIN : attrCount;
        assert bonusPercent != null;
        assert grid != null;
        assert handsDealt >= HANDS_DEALT_MIN : handsDealt;
        assert handSize >= HAND_SIZE_MIN : handSize;
        assert maxAttrValues != null;
        assert maxAttrValues.length >= attrCount : maxAttrValues.length;
        assert minutesPerHand >= MINUTES_PER_HAND_MIN : minutesPerHand;
        assert stuckThreshold > STUCK_THRESHOLD_MIN: stuckThreshold;
        assert rules != null;
        assert style != null && style != GameStyle.NONE;
    }

}
