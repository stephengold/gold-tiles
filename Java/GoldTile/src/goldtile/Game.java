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
    final private GameOpt opt;                // immutable
    final private Hands hands;                // all hands being played
    private int mustPlay = 0;  // min number of tiles to play, 0 after the 1st turn
    private String bestRunReport = null;
    private String filespec = null;            // associated file for load/save
    private String firstTurnMessage = null;
    private Tiles stockBag = null;
    private Turn redo = null;   // current index in the history, null means end
    final private Turns history = new Turns();
    
    // constructors
    
    public Game(GameOpt opt, HandOpts handOpts) {
        assert opt != null;
        opt.validate();
        assert handOpts != null;
        assert handOpts.size() == opt.getHandsDealt();

        // Save a copy of the options.
        this.opt = new GameOpt(opt);

        clientFlag = true; // TODO
        
        // Intialize static data of the Cell and Tile classes.
        Cell.setStatic(opt);
        Tile.setStatic(opt);
        
        // Re-seed the psedo-random generator.
        opt.reseedGenerator();
        
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
        Global.print("\nPlaced ");
        Global.print(StringExt.plural(countStock(), "tile"));
        Global.print(" in the stock bag.\n");
        
        hands = new Hands(handOpts);
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
        Global.print("\n");
        
        // The hand with the best "run" gets to go first.
        findBestRun();

        // Set the redo iterator to the end of the history.
        redo = null;

        // Pretend this game has been saved, for convenience.
        unsavedChangeFlag = false;

        assert isPaused();
        assert !canUndo();
        assert !canRedo();

    }

    // methods, sorted by name
    
    public void add(Turn turn) {
        assert !getPlayable().isClockRunning();

        if (redo != null) {
           history.clearToEnd(redo);
        }
        history.addLast(turn);
        redo = null;
        unsavedChangeFlag = true;
    }
    
    @Override
    final public boolean canRedo() {
        return redo != null;
    }
    
    @Override
    final public boolean canUndo() {
        if (redo == null) {
            return history.size() > opt.getHandsDealt();
        } else {
            return false; // TODO
        }
    }

    @Override
    public UserMessage checkMove(Move move) {
        assert move != null;
        
        final UserMessage result = board.checkMove(move);
        if (result != null) {
            return result;

        } else if (mustPlay > 0
                && !move.isResignation()
                && move.countTilesPlaced() != mustPlay)
        {
            // first turn but didn't resign, nor play correct number of tiles
            assert move.countTilesPlaced() < mustPlay : mustPlay;
            return UserMessage.FIRST_TURN;

        } else if (move.isPureSwap() 
                && move.size() > countStock()) {
            // swap but not enough tiles in stock
            return UserMessage.STOCK;

        } else {
            return null;
        }
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
        Global.print(bestRunReport);
        
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
    
    public void finishTurn(Move move) {
        assert move != null;
        assert isLegalMove(move);
        assert getPlayable().isClockRunning();

        stopClock();

        Tiles draw = new Tiles();
        int points = 0;
        
        if (move.isResignation()) {
            final Tiles tiles = hands.resign();
            stockBag.addAll(tiles);

        } else {
            // Remove played/swapped tiles from the playable hand.
            final Tiles tiles = move.copyTiles();
            hands.removeTiles(tiles);

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
                Global.print(String.format("%s returned %s to the stock bag.\n",
                        getPlayable().getName(), 
                        StringExt.plural(tileCount, "tile")));

            } else if (move.doesPlace()) {
                // Place tiles on the board.
                board.place(move);

                // Update the hand's score.
                points = board.score(move);
                hands.addScore(points);
           }
       }

       final int handIndex = hands.getPlayableIndex();
       history.add(handIndex, mustPlay, move, points, draw);

       //  If it *was* the first turn, it no longer is.
       mustPlay = 0;

       assert !getPlayable().isClockRunning();
    }
    
    private void firstTurnConsole() {
        assert GoldTile.control == Display.CONSOLE;
        assert isFirstTurn();
        assert isPaused();
        
        Global.print("\n");
        startClock();
        
        final ReadHand playable = getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();
        
        Move move;
        if (handOpt.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();
            
            move = playable.chooseMoveAutomatic(this);

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
                
                Global.print("\nThat isn't a legal move because:\n ");
                Global.print(reason.message);
                Global.print("\nPlease try again ...\n\n");
                Global.print(firstTurnMessage);
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
        } else if (history.lastPlaceIndex() + opt.getStuckThreshold() <= 
                   history.findIndex(redo))
        {
            return Ending.STUCK;
        } else {
           return null;
        }
    }
    
    @Override
    public ReadGameOpt getGameOpt() {
        return opt;
    }
    
    @Override
    public ReadHand getHand(int iHand) {
        return hands.get(iHand);
    }
    
    @Override
    public int getMustPlay() {
        return mustPlay;
    }
    
    @Override
    public ReadHand getPlayable() {
        return hands.getPlayable();
    }
    
    @Override
    public int getPlayableIndex() {
        return hands.getPlayableIndex();
    }
    
    @Override
    public int getSeconds(int iHand) {
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

    @Override
    public int[] getUnplayableIndices() {
        return hands.getUnplayableIndices();
    }
    
    @Override
    public boolean isFirstTurn() {
        return getMustPlay() > 0;
    }
    
    @Override
    public boolean isLegalMove(Move move) {
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

    public void nextHand() {
        assert !getPlayable().isClockRunning();

        // Skip over hands which have resigned.
        hands.nextWorking();
        unsavedChangeFlag = true;

        assert !getPlayable().isClockRunning();
    }
        
    private void nextTurnConsole() {
        assert GoldTile.control == Display.CONSOLE;
        assert !isFirstTurn();
        assert isPaused();

        startClock();
        Global.print(describeStatus());
        
        final ReadHand playable = getPlayable();
        final ReadHandOpt handOpt = playable.getOpt();

        Move move;
        if (handOpt.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();
            
            move = playable.chooseMoveAutomatic(this);

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
                
                Global.print("\nThat isn't a legal move because:\n ");
                Global.print(reason.message);
                Global.print("\nPlease try again ...\n\n");
                Global.print(describeStatus());
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
        Global.print(reportEndBonus());
        
        if (hands.size() > 1) {
            // Display final scores.
            Global.print(reportScores(Tense.PAST));
            Global.print("\n");
        }
    }
    
    private Tiles pullTiles(int count, int handIndex) {
        Tiles result = null;
        
        if (clientFlag) {
            try {
               result = stockBag.pullRandom(count);
            } catch (Tiles.PullEmptyException exception) {
               // caller will detect
            }
        } else {  // server
            // TODO
        }
        
        hands.addTiles(handIndex, result);

        final int actualCount = result.size();
        if (actualCount > 0) {
            Global.print(String.format("%s drew %s from the stock bag: %s.\n",
                    getPlayable().getName(),
                    StringExt.plural(actualCount, "tile"),
                    result.describe()));
        }

        return result;
    }

    public void removeTiles(Tiles tiles) {
        hands.removeTiles(tiles);    
    }
    
    private String reportEndBonus() {
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
                        hands.addScore(pointCount);

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
}
