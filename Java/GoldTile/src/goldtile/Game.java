// File:     Game.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Game class for the Gold Tile Game
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

public class Game
    implements ReadGame
{
    // enums, sorted by name
    public static enum Ending { RESIGNED, STUCK, WENT_OUT; };
    public static enum Tense { PAST, PRESENT; };

    // per-instance fields, sorted by type
    private Board board = new Board();        // extensible playing surface
    final private boolean clientFlag;         // immutable
    private boolean unsavedChangeFlag = false;
    final private Hands hands;                // all hands being played
    private int mustPlay = 0;  // min number of tiles to play, 0 after the 1st turn
    final private ReadGameOpt opt;            // immutable
    private String bestRunReport = null;
    private String filespec = null;           // file for load/save
    private String firstTurnMessage = null;
    final private Tiles stockBag;
    private Tiles warmTiles = new Tiles(); // tiles played on the previous turn
    final private History history = new History();

    // static fields
    private static Game currentInstance = null;

    // constructors

    public Game(GameOpt opt, HandOpt[] handOpts) {
        assert opt != null;
        opt.validate();
        assert handOpts != null;
        assert handOpts.length >= opt.getHandsDealt();

        // Replace the active game.
        currentInstance = this;

        // Save a copy of the options.
        this.opt = new GameOpt(opt);

        clientFlag = true; // TODO

        // Re-seed the psedo-random generator.
        opt.reseedGenerator();

        // Make a new batch of tiles.
        Tile.newGame(opt);

        // Add tiles to the stock bag.
        stockBag = new Tiles();
        if (clientFlag) {
            for (int i = 0; i < opt.getTilesPerCombo(); i++) {
                // Generate all possible tiles.
                stockBag.addAllCombos();
            }
        } else {
            // TODO
        }
        Console.printf("\nPlaced %s in the stock bag.\n",
            StringExt.plural(countStock(), "tile") );

        hands = new Hands(opt.getHandsDealt(), handOpts);
        assert hands.size() == opt.getHandsDealt();

        // The deal:  each hand pulls tiles from the stock bag.
        final int handSize = opt.getHandSize();
        int iHand = 0;
        for (Hand hand : hands) {
            assert !hand.isClockRunning();
            final Tiles dealt = pullTiles(handSize, iHand);
            assert dealt.size() == handSize : dealt;

            // Record the deal in the history.
            history.add(iHand, dealt);
            iHand++;
        }
        assert history.size() == opt.getHandsDealt();
        Console.printLine();

        // The hand with the best "run" gets to go first.
        findBestRun();

        // Pretend this game has been saved, for convenience.
        unsavedChangeFlag = false;

        assert isPaused();
        assert !canUndo();
        assert !canRedo();
    }

    // methods, sorted by name

    @Override
    final public boolean canRedo() {
        return history.canRedo();
    }

    @Override
    final public boolean canUndo() {
        return history.canUndo();
    }

    @Override
    public UserMessage checkMove(ReadMove move) {
        assert move != null;

        final UserMessage result = board.checkMove(move);
        if (result != null) {
            return result;

        } else if (mustPlay > 0
                && !move.isResignation()
                && move.countCellsUsed() != mustPlay)
        {
            // first turn but didn't resign, nor play correct number of tiles
            assert move.countCellsUsed() < mustPlay : mustPlay;
            return UserMessage.FIRST_TURN;

        } else if (move.isPureSwap()
                && move.size() > countStock())
        {
            // swap but not enough tiles in stock
            return UserMessage.STOCK;

        } else {
            return null;
        }
    }

    public static void closeCurrentInstance() {
        currentInstance = null;
    }

    @Override
    public Board copyBoard() {
        return new Board(board);
    }

    @Override
    final public int countStock() {
        assert stockBag != null;

        return stockBag.size();
    }

    private String describeScores() {
        String result = "";

        for (Hand hand : hands) {
            result += hand.describeScore();
        }

        return result;
    }

    private String describeStatus() {
        final int stock = countStock();

        return String.format(
                "%s\n%s's turn, %s remaining in the stock bag\n\n%s\n",
                describeScores(),
                getPlayable().getName(),
                StringExt.plural(stock, "tile"),
                board.describe());
    }

    private void findBestRun() {
        assert board != null;
        assert board.isEmpty() : board;
        assert hands != null;
        assert hands.size() > 0 : hands;

        final Hands.BestRun bestRun = hands.findBestRun();

        // Make sure the run will fit on the board.
        mustPlay = Cell.limitPlay(bestRun.length);

        firstTurnMessage = String.format(
                "%s plays first and must place %s on the (empty) board.\n",
                getPlayable().getName(),
                StringExt.plural(mustPlay, "tile"));

        bestRunReport = bestRun.report + "\n" + firstTurnMessage;
        Console.print(bestRunReport);

        assert bestRunReport != null;
        assert firstTurnMessage != null;
        assert mustPlay > 0 : mustPlay;
    }

    private Strings findWinningHands() {
        assert hands.size() > 0;

        final int topScore = hands.findMaxScore();
        final Strings result = new Strings();

        for (Hand hand : hands) {
            if (hand.getScore() == topScore) {
                result.addLast(hand.getName());
            }
        }

        return result;
    }

    public void finishTurn(ReadMove move) {
        assert move != null;
        assert isLegalMove(move);
        assert getPlayable().isClockRunning();

        stopClock();

        Tiles draw = new Tiles();
        int points = 0;
        final Tiles saveWarmTiles = warmTiles;

        if (move.isResignation()) {
            final Tiles tiles = hands.resignPlayable();
            stockBag.addAll(tiles);
            warmTiles = new Tiles();

        } else {
            // Remove played/swapped tiles from the playable hand.
            final Tiles tiles = move.copyTiles();
            hands.removePlayableTiles(tiles);

            // Attempt to draw replacement tiles from the stock bag.
            final int tileCount = tiles.size();
            if (tileCount > 0) {
                final int iHand = hands.getPlayableIndex();
                draw = pullTiles(tileCount, iHand);
                assert !move.involvesSwap() || draw.size() == tileCount;
            }

            if (move.isPureSwap()) {
                // Return tiles to the stock bag.
                stockBag.addAll(tiles);
                warmTiles = new Tiles();
                Console.printf("%s returned %s to the stock bag.\n",
                        getPlayable().getName(),
                        StringExt.plural(tileCount, "tile") );

            } else if (move.doesPlace()) {
                // Place tiles on the board.
                move.place(board);

                // Update the hand's score.
                points = board.score(move);
                hands.addScorePlayable(points);
                warmTiles = new Tiles(tiles);

            } else {
                assert move.isPass();
                warmTiles = new Tiles();
            }
       }

       final int handIndex = hands.getPlayableIndex();
       history.add(handIndex, mustPlay, move, points, draw, saveWarmTiles);

       //  If it *was* the first turn, it no longer is.
       mustPlay = 0;
       
       unsavedChangeFlag = true;

       assert !getPlayable().isClockRunning();
    }

    private void firstTurnConsole() {
        assert GoldTile.control == Display.CONSOLE;
        assert isFirstTurn();
        assert isPaused();

        Console.printLine();
        if (GoldTile.enableGui) {
            //invokeLater();
        }
        startClock();

        final ReadHand playable = getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();

        ReadMove move;
        if (handOpt.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();

            move = playable.chooseMoveAutomatic();

        } else if (handOpt.isRemote()) {
            move = playable.chooseMoveRemote();

        } else {
            assert handOpt.isLocalUser();

            for (;;) {
                move = playable.chooseMoveConsole(mustPlay);
                final UserMessage reason = checkMove(move);
                if (reason == null) {
                    break;
                }

                Console.printf(
                        "\nThat isn't a legal move because:\n %s\n",
                        reason.message);
                Console.print("Please try again ...\n\n");
                Console.print(firstTurnMessage);
            }
        }

        finishTurn(move);
    }

    @Override
    public ReadBoard getBoard() {
        return board;
    }

    @Override
    public Ending getEnding() {
        /*
         * The game is over if and only if:
         *     (1) the stock bag is empty and a hand has gone out
         * or (2) all hands have resigned
         * or (3) none of the last 7 moves placed any tiles on the board.
         */
        if (isStockEmpty() && hands.hasAnyGoneOut()) {
            return Ending.WENT_OUT;
        } else if (hands.haveAllResigned()) {
            return Ending.RESIGNED;
        } else if (history.findLastPlace() + opt.getStuckThreshold() <=
                   history.getRedoIndex())
        {
            return Ending.STUCK;
        } else {
           return null;
        }
    }

    @Override
    public ReadHand getHand(int iHand) {
        return hands.get(iHand);
    }

    public static Game getInstance() {
        return currentInstance;
    }

    @Override
    public int getMustPlay() {
        return mustPlay;
    }

    @Override
    public ReadGameOpt getOpt() {
        return opt;
    }

    @Override
    public ReadHand getPlayable() {
        return hands.getPlayable();
    }

    @Override
    public int getPlayableIndex() {
        return hands.getPlayableIndex();
    }

    /**
     * Reads the clock of the specified hand and converts it
     * into seconds for display.
     *
     * @param iHand
     * @return seconds remaining (if the game has a time limit)
     *       or elapsed seconds (if no time limit)
     */
    @Override
    public int getSeconds(int iHand) {
        assert iHand >= 0;
        assert iHand < hands.size();

        // Read a hand's clock.
        int result = hands.get(iHand).getSecondsUsed();

        final GameStyle style = opt.getStyle();
        if (style.hasTimeLimit()) {
            // counting down
            final int timeLimit = opt.getSecondsPerHand();
            result = timeLimit - result;
        }

        return result;
    }

    public static GameStyle getStyle() {
        if (currentInstance != null) {
            final ReadGameOpt opt = currentInstance.getOpt();
            return opt.getStyle();
        } else {
            return GameStyle.NONE;
        }
    }

    @Override
    public int[] getUnplayableIndices() {
        return hands.getUnplayableIndices();
    }

    @Override
    public Strings getUserNames() {
        final Strings result = new Strings();

        for (Hand hand : hands) {
            final ReadHandOpt handOpt = hand.getOpt();
            if (handOpt.isLocalUser()) {
                final String name = handOpt.getPlayerName();
                if (!result.contains(name)) {
                    result.add(name);
                }
            }
        }

        return result;
    }

    public static boolean hasInstance() {
        return currentInstance != null;
    }

    @Override
    public boolean isFirstTurn() {
        return getMustPlay() > 0;
    }

    @Override
    public boolean isLegalMove(ReadMove move) {
        assert move != null;

        final UserMessage reason = checkMove(move);

        return reason == null;
    }

    @Override
    public boolean isOutOfTime() {
        if (!opt.getStyle().hasTimeLimit()) {
            return false;
        }

        final long have = opt.getMillisecondsPerHand();
        final long used = getPlayable().getMillisecondsUsed();

        return (used >= have);
    }

    @Override
    public boolean isOver() {
        return getEnding() != null;
    }

    @Override
    final public boolean isPaused() {
        return !getPlayable().isClockRunning() && !isOver();
    }

    @Override
    public boolean isPlayable(int iHand) {
        return hands.getPlayableIndex() == iHand;
    }

    @Override
    public boolean isStockEmpty() {
        return countStock() == 0;
    }

    @Override
    public boolean isWarmTile(Tile tile) {
        assert tile != null;

        return warmTiles.contains(tile);
    }

    public void nextHand() {
        assert !getPlayable().isClockRunning();
        assert !isOver();

        // Skip over hands which have resigned.
        hands.nextPlayable();
        unsavedChangeFlag = true;

        assert !getPlayable().isClockRunning();
    }

    private void nextTurnConsole() {
        assert GoldTile.control == Display.CONSOLE;
        assert !isFirstTurn();
        assert isPaused();

        startClock();
        Console.print(describeStatus());

        final ReadHand playable = getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();

        ReadMove move;
        if (handOpt.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();

            move = playable.chooseMoveAutomatic();

        } else if (handOpt.isRemote()) {
            move = playable.chooseMoveRemote();

        } else {
            assert handOpt.isLocalUser() : handOpt;

            for (;;) {
                move = playable.chooseMoveConsole(mustPlay);
                final UserMessage reason = checkMove(move);
                if (reason == null) {
                    break;
                }

                Console.printf("\nThat isn't a legal move because:\n %s\n",
                        reason.message);
                Console.print("Please try again ...\n\n");
                Console.print(describeStatus());
            }
        }

        finishTurn(move);
    }

    public void playConsole() {
        assert GoldTile.control == Display.CONSOLE;

        firstTurnConsole();

        while (!isOver()) {
            nextHand();
            nextTurnConsole();
        }
        Console.print(reportEndBonus());

        if (hands.size() > 1) {
            // Display final scores.
            Console.printf("%s\n", reportScores(Tense.PAST) );
        }
    }

    private Tiles pullTiles(int count, int handIndex) {
        assert count >= 0 : count;
        assert handIndex >= 0 : handIndex;
        assert handIndex < hands.size() : handIndex;

        Tiles result = null;

        if (clientFlag) {
            result = stockBag.pullRandom(count);
        } else {  // server
            // TODO
        }

        hands.addTiles(handIndex, result);

        final int actualCount = result.size();
        if (actualCount > 0) {
            Console.printf("%s drew %s from the stock bag.\n",
                    getPlayable().getName(),
                    StringExt.plural(actualCount, "tile") );
        }

        return result;
    }

    public void redoTurn() {
        assert opt.getStyle().allowsUndo();
        assert !getPlayable().isClockRunning();
        assert canRedo();

        final Turn turn = history.redoTurn();
        final int handIndex = turn.getHandIndex();
        assert hands.getPlayableIndex() == handIndex;

        final ReadMove move = turn.getMove();
        if (move.isResignation()) {
            final Tiles tiles = hands.resignPlayable();
            stockBag.addAll(tiles);
            warmTiles = new Tiles();
        } else {
            // Remove played/swapped tiles from the hand.
            final Tiles tiles = move.copyTiles();
            hands.removePlayableTiles(tiles);

            // Re-draw replacement tiles from the stock bag.
            final Tiles draw = turn.copyDraw();
            hands.addTiles(handIndex, draw);
            stockBag.removeAll(draw);

            if (move.involvesSwap()) {
               // Return swapped tiles to the stock bag.
                stockBag.addAll(tiles);
                warmTiles = new Tiles();

            } else { // play or pass
                // Place the played tiles on the board.
                move.place(board);

                // Update the warm tiles.
                warmTiles = new Tiles(tiles);

                // Update the hand's score.
                final int points = turn.getPoints();
                assert points == board.score(move) : points;
                hands.addScorePlayable(points);
            }
        }

        //  If it was the first turn, it no longer is.
        mustPlay = 0;
        
        unsavedChangeFlag = true;

        if (!isOver()) {
            nextHand();
        }
    }

    public void removeTiles(Tiles tiles) {
        assert tiles != null;

        hands.removePlayableTiles(tiles);
        unsavedChangeFlag = true;
    }

    public String reportBestRun() {
        assert bestRunReport != null;

        return bestRunReport;
    }


    public String reportEndBonus() {
        assert isOver();

        String result = "";

        final Ending ending = getEnding();
        final ReadHand playable = getPlayable();
        final String handName = playable.getName();

        if (ending == Ending.WENT_OUT) {
            // Start writing the report.
            result += String.format("%s went out with %s.\n\n",
                    handName,
                    StringExt.plural(playable.getScore(), "point") );

            // The playable hand scores a point for each tile in another hand.
            for (Hand hand : hands) {
                if (hand != playable) {
                    final int tileCount = hand.countContents();
                    final int pointCount = tileCount; // TODO?

                    if (pointCount > 0) {
                        hands.addScorePlayable(pointCount);

                        result += String.format(
                                "Add %s for the %s held by %s.\n",
                                StringExt.plural(pointCount, "point"),
                                StringExt.plural(tileCount, "tile"),
                                handName);
                    }
                }
            }
            result += "\n";
        }

        result += String.format("%s ended up with %s.\n\n",
            handName,
            StringExt.plural(playable.getScore(), "point"));

        return result;
    }

    private String reportScores(Tense tense) {
        assert tense != null;
        assert hands.size() > 0;

        String result = "";

        final Strings winningHands = findWinningHands();
        if (winningHands.size() == 1) {
            final String winner = winningHands.getFirst();
            result += String.format("%s %s the game.\n",
                    winner,
                    tense == Tense.PAST ? "won" : "is winning");
        } else {
            assert !winningHands.isEmpty();
            final String winners = winningHands.join(" and ");
            result += String.format("%s %s for first place.\n",
                    winners,
                    tense == Tense.PAST ? "tied" : "are tied");
        }

        return result;
    }

    public void restart() {
        assert !getPlayable().isClockRunning();

        // Return all played tiles to the stock bag.
        final Tiles playedTiles = board.getTiles();
        stockBag.addAll(playedTiles);
        board.clear();

        // Return all hand tiles to the stock bag.
        final Tiles handTiles = hands.restart();
        stockBag.addAll(handTiles);

        // Redo the initial draws.
        history.restart();

        for (int iHand = 0; iHand < hands.size(); iHand++) {
            final Turn turn = history.redoTurn();
            assert turn.wasDrawOnly();

            final Tiles drawTiles = turn.copyDraw();
            assert drawTiles.size() == opt.getHandSize();

            final int handIndex = turn.getHandIndex();
            hands.addTiles(handIndex, drawTiles);

            stockBag.removeAll(drawTiles);
        }

        // The hand with the best "run" gets to go first.
        findBestRun();

        unsavedChangeFlag = true;

        assert isPaused();
        assert !canUndo();
    }

    public void startClock() {
        assert !isOver();
        assert !getPlayable().isClockRunning();

        hands.startClock();
        assert getPlayable().isClockRunning();
    }

    public void stopClock() {
        assert getPlayable().isClockRunning();

        hands.stopClock();
    }

    public void togglePause() {
        assert !isOver();

        if (getPlayable().isClockRunning()) {
            hands.stopClock();
        } else {
            hands.startClock();
        }
    }

    public String toString() {
        String result = "";
        
        // TODO
        
        return result;
    }
    
    public void undoTurn() {
        assert opt.getStyle().allowsUndo();
        assert canUndo();

        final Turn turn = history.undoTurn();
        final int handIndex = turn.getHandIndex();
        hands.setPlayable(handIndex);

        //  Roll back the must-play info.
        mustPlay = turn.getMustPlay();

        final ReadMove move = turn.getMove();
        final Tiles tiles = move.copyTiles();
        if (move.isResignation()) {
            hands.unResign(handIndex, tiles);
            stockBag.removeAll(tiles);
        } else {
            // Return drawn tiles to the stock bag.
            final Tiles draw = turn.copyDraw();
            hands.removePlayableTiles(draw);
            stockBag.addAll(draw);

            // Add played/swapped tiles back into the hand.
            hands.addTiles(handIndex, tiles);


            if (move.involvesSwap()) {
               // Remove swapped tiles from the stock bag.
                stockBag.removeAll(tiles);

            } else { // play or pass
                // Remove played tiles from the board.
                move.unplace(board);

                // Roll back the hand's score.
                final int points = turn.getPoints();
                hands.subtractScorePlayable(points);
            }
        }

        // Roll back the warm tiles.
        warmTiles = turn.copyWarm();
        
        unsavedChangeFlag = true;
    }
}
