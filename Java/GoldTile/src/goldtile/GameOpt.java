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
    // constants, sorted by type
    final public static Dim BOARD_HEIGHT_DEFAULT = new Dim(12);
    final public static Dim BOARD_WIDTH_DEFAULT = BOARD_HEIGHT_DEFAULT;
    final public static Fraction BONUS_FRACTION_DEFAULT = new Fraction(0.0);
    final public static Fraction BONUS_FRACTION_MIN = new Fraction(0.0);
    final public static int ATTR_COUNT_DEFAULT = 2;
    final public static int ATTR_COUNT_MAX = 5;
    final public static int ATTR_COUNT_MIN = 2;
    final public static int CLONES_PER_COMBO_DEFAULT = 2;
    final public static int CLONES_PER_COMBO_MIN = 0;
    final public static int HANDS_DEALT_DEFAULT = 2;
    final public static int HANDS_DEALT_MIN = 1;
    final public static int HAND_SIZE_DEFAULT = 6;
    final public static int HAND_SIZE_MIN = 1;
    final public static int MINUTES_PER_HAND_DEFAULT = 30;
    final public static int MINUTES_PER_HAND_MAX = 120;
    final public static int MINUTES_PER_HAND_MIN = 2;
    final private static int STUCK_THRESHOLD_DEFAULT = 7;
    final private static int STUCK_THRESHOLD_MIN = 1;
    final public static long SEED_DEFAULT = 12345L;

    // per-instance fields, sorted by type
    final private Attr lastAttrs[] = new Attr[ATTR_COUNT_MAX];
                                             // maximum value of each attribute
    private boolean boardWrap;
                      // used only if board is finite on one or both dimensions
    private boolean randomize;
    private Dim boardHeight;
    private Dim boardWidth;
    private Fraction bonusFraction;  // fraction of tiles with bonus value
    private GameStyle style;
    private Grid grid;
    private int attrCount;
    private int clonesPerCombo;
    private int handsDealt;      // number of hands dealt
    private int handSize;        // maximum number of tiles in a hand
    private int minutesPerHand;  // used only in challenge-style games
    private int stuckThreshold;  // turns before game is declared "stuck"
    private long seed;  // used only in debug-style with randomizeFlag == false
    private Rules rules;

    // constructors

    public GameOpt() {
        minutesPerHand = MINUTES_PER_HAND_DEFAULT;
        randomize = true;
        seed = SEED_DEFAULT;
        style = GameStyle.getDefault();

        standardize();
        validate();
    }

    public GameOpt(ReadGameOpt other) {
        assert other != null;

        boardWrap = other.doesBoardWrap();
        randomize = other.isRandomized();
        style = other.getStyle();
        grid = other.getGrid();
        attrCount = other.getAttrCount();
        boardHeight = other.getBoardHeight();
        boardWidth = other.getBoardWidth();
        clonesPerCombo = other.getClonesPerCombo();
        handsDealt = other.getHandsDealt();
        handSize = other.getHandSize();
        for (int iAttr = 0; iAttr < ATTR_COUNT_MAX; iAttr++) {
            lastAttrs[iAttr] = other.getAttrLast(iAttr);
        }
        minutesPerHand = other.getMinutesPerHand();
        stuckThreshold = other.getStuckThreshold();
        seed = other.getSeed();
        bonusFraction = other.getBonusFraction();
        rules = other.getRules();

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
         * Clone tiles so that there are enough to fill every hand at
         * least three times.
         */
        final long tilesNeeded = 3 * result.handSize * result.handsDealt;
        final long comboCount = result.countCombos();
        result.clonesPerCombo = (int)(tilesNeeded / comboCount);

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
    public long countTiles() {
        return countCombos() * (1 + clonesPerCombo);
    }

    @Override
    public boolean doesBoardWrap() {
        return boardWrap;
    }

    @Override
    public int getAttrCount() {
        return attrCount;
    }

    @Override
    public Attr getAttrLast(int iAttr) {
        assert iAttr >= 0;
        assert iAttr < lastAttrs.length : iAttr;

        return lastAttrs[iAttr];
    }

    @Override
    public int getAttrLastValue(int iAttr) {
        assert iAttr >= 0;
        assert iAttr < lastAttrs.length : iAttr;

        return  getAttrLast(iAttr).intValue();
    }

    @Override
    public int getAttrValueCount(int iAttr) {
        assert iAttr >= 0;
        assert iAttr < lastAttrs.length : iAttr;

        return 1 + getAttrLastValue(iAttr);
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
        return bonusFraction;
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
        return randomize;
    }

    @Override
    public String reportAttrs() {
        String result = "Tiles have ";
        result += StringExt.plural(attrCount, "attribute") + ":\n";

        for (int iAttr = 0; iAttr < attrCount; iAttr++) {
            final AttrMode mode = AttrMode.getConsoleDefault(iAttr);

            result += String.format(" The %s attribute ranges from %s to %s.\n",
                    StringExt.ordinal(iAttr + 1),
                    mode.attrToString(new Attr(0)),
                    mode.attrToString(getAttrLast(iAttr)) );
        }
        result += "\n";

        return result;
    }

    public void reseedGenerator() {
        if (randomize || !style.isDebug()) {
            final Date date = new Date();
            seed = date.getTime();
        }
        Global.reseedGenerator(seed);
    }

    public void setAttrCount(int attrCount) {
        assert attrCount >= ATTR_COUNT_MIN;
        assert attrCount <= ATTR_COUNT_MAX;

        this.attrCount = attrCount;
    }

    public void setAttrValueCount(int iAttr, int count) {
       assert iAttr >= 0;
       assert iAttr < ATTR_COUNT_MAX;
       assert count >= Attr.COUNT_MIN;
       assert count <= Attr.COUNT_MAX;

       lastAttrs[iAttr] = new Attr(count - 1);
    }

    public void setBoardHeight(Dim boardHeight) {
        this.boardHeight = boardHeight;
    }

    public void setBoardWidth(Dim boardWidth) {
        this.boardWidth = boardWidth;
    }

    public void setBoardWrap(boolean doesWrap) {
        this.boardWrap = doesWrap;
    }

    public void setBonusFraction(Fraction bonusFraction) {
        assert bonusFraction != null;

        this.bonusFraction = bonusFraction;
    }

    public void setClonesPerCombo(int clonesPerCombo) {
        assert clonesPerCombo >= CLONES_PER_COMBO_MIN;

        this.clonesPerCombo = clonesPerCombo;
    }

    public void setGrid(Grid grid) {
        assert grid != null;

        this.grid = grid;
    }

    public void setHandSize(int handSize) {
        assert handSize >= HAND_SIZE_MIN;

        this.handSize = handSize;
    }

    public void setHandsDealt(int handsDealt) {
        assert handsDealt >= HANDS_DEALT_MIN;

        this.handsDealt = handsDealt;
    }

    public void setMinutesPerHand(int minutesPerHand) {
        this.minutesPerHand = minutesPerHand;
    }

    public void setRandomize(boolean randomizeFlag) {
        this.randomize = randomizeFlag;
    }

    public void setRules(Rules rules) {
        assert rules != null;

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
        attrCount = ATTR_COUNT_DEFAULT;
        for (int iAttr = 0; iAttr < ATTR_COUNT_MAX; iAttr++) {
            lastAttrs[iAttr] = new Attr(Attr.LAST_DEFAULT);
        }
        boardHeight = Dim.endless();
        boardWidth = Dim.endless();
        bonusFraction = BONUS_FRACTION_DEFAULT;
        clonesPerCombo = CLONES_PER_COMBO_DEFAULT;
        boardWrap = false;
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
        assert attrCount <= ATTR_COUNT_MAX : attrCount;
        assert attrCount >= ATTR_COUNT_MIN : attrCount;
        for (int iAttr = 0; iAttr < ATTR_COUNT_MAX; iAttr++) {
            assert lastAttrs[iAttr].intValue() >= Attr.LAST_MIN;
            assert lastAttrs[iAttr].intValue() <= Attr.LAST_MAX;
        }
        assert boardHeight != null;
        assert boardWidth != null;
        assert bonusFraction != null;
        assert clonesPerCombo >= CLONES_PER_COMBO_MIN : clonesPerCombo;
        assert grid != null;
        assert handsDealt >= HANDS_DEALT_MIN : handsDealt;
        assert handSize >= HAND_SIZE_MIN : handSize;
        assert lastAttrs != null;
        assert lastAttrs.length >= attrCount : lastAttrs;
        assert minutesPerHand >= MINUTES_PER_HAND_MIN : minutesPerHand;
        assert stuckThreshold >= STUCK_THRESHOLD_MIN : stuckThreshold;
        assert rules != null;
        assert style != null;
        assert style != GameStyle.NONE;
    }
}
