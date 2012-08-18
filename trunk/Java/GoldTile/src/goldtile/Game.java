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

public class Game {
    // enums
    public static enum Ending {
        RESIGNED,
        STUCK,
        WENT_OUT;
    };

    // constants
    final private static int STUCK_THRESHOLD = 7;  
            // number of turns before game is declared "stuck" 

    // per-instance fields   
    private Board board = new Board();        // extensible playing surface
    final private boolean clientFlag;
    private boolean unsavedChangeFlag = false;
    final private GameOpt opt;
    private Hand playable = null;             // whose turn it is
    final private Hands hands = new Hands();  // all hands being played
    private int mustPlay = 0;  // min number of tiles to play, 0 after the 1st turn
    public String bestRunReport = null;
    public String filespec = null;            // associated file for load/save
    public String firstTurnMessage = null;
    private Tiles stockBag = null;
    private Turn redo = null;   // current index in the history, null means end
    final private Turns history = new Turns();
    
    // constructor
    
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
        
        // Generate a list of player names, duplicates included.
        Strings names = handOpts.getAllPlayerNames();

        // Construct hands and give each one a unique name.
        for (HandOpt handOpt : handOpts) {
            String handName = handOpt.getPlayerName();
            if (names.count(handName) > 1) {
                handName = names.inventUnique(handName, "'s ", " hand");
                names.addLast(handName);
            }

            final Hand hand = new Hand(handName, handOpt);
            assert !hand.isClockRunning();
            hands.addLast(hand);
        }
        
        assert hands.size() == opt.getHandsDealt();
        assert playable == null;
    }

    // methods
    
    public void activateNextHand() {
        assert !isClockRunning();

        // Skip over hands which have resigned.
        playable = hands.getNextWorking(playable);
        unsavedChangeFlag = true;

        assert !isClockRunning();
    }
    
    public void add(Turn turn) {
        assert !isClockRunning();

        if (redo != null) {
           history.clearToEnd(redo);
        }
        history.addLast(turn);
        redo = null;
        unsavedChangeFlag = true;
    }
    
    public boolean canRedo() {
        return redo != null;
    }
    
    public boolean canUndo() {
        if (redo == null) {
            return history.size() > opt.getHandsDealt();
        } else {
            return false; // TODO
        }
    }

    private UserMessage checkMove(Move move) {
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

    public Board copyBoard() {
        assert board != null;
        
        return new Board(board);
    }
    
    public GameOpt copyOpt() {
        return new GameOpt(opt);
    }
    
    public Tiles copyPlayableTiles() {
        assert playable != null;
        
        return playable.copyContents();
    }
    
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
                playable.name,
                StringExt.plural(stock, "tile"),
                board.describe());      
    }

    private void finishTurn(Move move) {
        assert move != null;
        assert isLegalMove(move);
        assert isClockRunning();

        stopClock();

        Turn turn = new Turn(move, playable, mustPlay);

        if (move.isResignation()) {
            stockBag.addAll(playable.resign());

        } else {
            // Remove played/swapped tiles from the hand.
            final Tiles tiles = move.copyTiles();
            playable.remove(tiles);

            // Attempt to draw replacement tiles from the stock bag.
            final int tileCount = tiles.size();
            if (tileCount > 0) {
                final Tiles draw = pullTiles(tileCount, playable);
                assert draw.size() == tileCount || !move.involvesSwap();
                turn.setDraw(draw);
            }

            if (move.isPureSwap()) {
                // Return tiles to the stock bag.
                stockBag.addAll(tiles);
                playable.printName();
                Global.print(" returned ");
                Global.print(StringExt.plural(tileCount, "tile"));
                Global.print(" to the stock bag.\n");

            } else if (move.doesPlace()) {
                // Place tiles on the board.
                board.place(move);

                // Update the hand's score.
                final int points = board.score(move);
                playable.addScore(points);
                turn.setPoints(points);
           }
       }

       add(turn);

       //  If it was the first turn, it no longer is.
       mustPlay = 0;

       assert !isClockRunning();
    }
    
    private void findBestRun() {
        assert board != null;
        assert board.isEmpty() : board;
        assert hands != null;
        assert hands.size() > 0 : hands;
        
        bestRunReport = "";
        mustPlay = 0;
        playable = hands.getFirst();
        
        for (Hand hand : hands) {
            final Tiles run = hand.getLongestRun();
            final int runLength = run.size();

            bestRunReport += String.format("%s has a run of %s: %s.\n",
                    hand.name,
                    StringExt.plural(runLength, "tile"),
                    run.describe());

            if (runLength > mustPlay) {
                mustPlay = runLength;
                playable = hand;
            } 
        }
        bestRunReport += "\n";

        // Make sure the run will fit on the board.
        mustPlay = Cell.limitPlay(mustPlay);

        firstTurnMessage = String.format(
                "%s plays first and must place %s on the (empty) board.\n",
                playable.name, 
                StringExt.plural(mustPlay, "tile"));

        bestRunReport += firstTurnMessage;
        Global.print(bestRunReport);
        
        assert bestRunReport != null;
        assert firstTurnMessage != null;
        assert mustPlay > 0 : mustPlay;
        assert playable != null;
    }
    
    private Strings findWinningHands() {
        assert hands.size() > 0;

        final int topScore = findMaxScore();
        final Strings result = new Strings();
        
        for (Hand hand : hands) {
            if (hand.getScore() == topScore) {
                result.addLast(hand.name);
            }
        }

        return result;    
    }
    
    private int findMaxScore() {
        assert hands.size() > 0;
        
        final Hand first = hands.getFirst();       
        int result = first.getScore();

        for (Hand hand : hands) {
            final int score = hand.getScore();
            if (score > result) {
                result = score;
            }
        }

        return result;    
    }

    private void firstTurnConsole() {
        assert playable != null;
        assert isPaused();
        
        Global.print("\n");
        startClock();
        
        Move move;
        if (playable.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();
            
            move = playable.chooseMoveAutomatic(this);

        } else if (playable.isRemote()) {
            move = playable.chooseMoveRemote();
            
        } else {
            assert playable.isLocalUser();
            
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
        } else if (history.lastPlaceIndex() + STUCK_THRESHOLD == 
                   history.findIndex(redo))
        {
            return Ending.STUCK;
        } else {
           return null;
        }
    }
    
    public void initialize() {
        assert history.isEmpty();
        assert playable == null : playable;
        
        // The deal:  each hand pulls tiles from the stock bag.
        final int handSize = opt.getHandSize();
        for (Hand hand : hands) {
            assert !hand.isClockRunning();
            final Tiles dealt = pullTiles(handSize, hand);
            assert dealt.size() == handSize : dealt;

            // Record the deal in the history.
            final Turn turn = new Turn(hand);
            history.addLast(turn);
        }
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
    
    public boolean isChallenge() {
        return opt.isChallenge();
    }
    
    public boolean isClockRunning() {
         return playable.isClockRunning();
    }
    
    public boolean isLegalMove(Move move) {
        assert move != null;
        
        final UserMessage reason = checkMove(move);
        
        return reason == null;
    }
    
    public boolean isOver() {
        return getEnding() != null;
    }

    public boolean isPaused() {
        return !isClockRunning() && !isOver();
    }

    public boolean isStockEmpty() {
        return countStock() == 0;
    }
    
    private void nextTurnConsole() {
        assert isPaused();

        startClock();
        Global.print(describeStatus());

        Move move;
        if (playable.isAutomatic()) {
            assert clientFlag;
            assert !canRedo();
            
            move = playable.chooseMoveAutomatic(this);

        } else if (playable.isRemote()) {
            move = playable.chooseMoveRemote();
            
        } else {
            assert playable.isLocalUser();
            
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
        firstTurnConsole();
        
        while (!isOver()) {
            activateNextHand();
            nextTurnConsole();
        }
        Global.print(reportEndBonus());
        
        // Display final scores.
        Global.print(reportScores());
        Global.print("\n");
    }
    
    private Tiles pullTiles(int count, Hand hand) {
        Tiles result = null;
        
        if (clientFlag) {
            try {
               result = stockBag.pullRandom(count);
            } catch (Tiles.PullEmptyException exception) {
               // the caller will notice
            }
        } else {  // server
            // TODO
        }
        
        hand.add(result);

        final int actualCnt = result.size();
        if (actualCnt > 0) {
            hand.printName();
            Global.print(" drew ");
            Global.print(StringExt.plural(actualCnt, "tile"));
            Global.print(" from the stock bag: ");
            Global.print(result.describe());
            Global.print("\n");
        }

        return result;
    }

    private String reportEndBonus() {
        assert isOver();
        String result = "";
        
        final Ending ending = getEnding();

        if (ending == Ending.WENT_OUT) {
            // Start writing the report.
            result += String.format("%s went out with %s.\n\n",
                    playable.name,
                    StringExt.plural(playable.getScore(), "point") );

            // The playable hand scores a point for each tile in another hand.
            for (Hand hand : hands) {
                if (hand != playable) {  
                    final int tileCount = hand.size();
                    final int pointCount = tileCount; // TODO?
                
                    if (pointCount > 0) {
                        playable.addScore(pointCount);

                        result += String.format(
                                "Add %s for the %s held by %s.\n", 
                                StringExt.plural(pointCount, "point"),
                                StringExt.plural(tileCount, "tile"),
                                playable.name);
                    }
                }
            }
            result += "\n";
        }

        result += String.format("%s ended up with %s.\n\n",
            playable.name,
            StringExt.plural(playable.getScore(), "point"));

        if (hands.size() > 1) {
            final Strings winningHands = findWinningHands();
            if (winningHands.size() == 1) {
                final String winner = winningHands.getFirst();
                result += String.format("%s won the game.\n", winner);
            } else {
                assert !winningHands.isEmpty();
                final String winners = winningHands.join(" and ");
                result += String.format("%s tied for first place.\n", winners);
            }
        }

        return result;
    }
    
    private String reportScores() {
        String result = "";
        
        // TODO
        return result;        
    }
    
    public void startClock() {
        assert !isOver();
        assert !isClockRunning();

        playable.startClock();
        assert isClockRunning();
    }
    
    public void stopClock() {
        assert isClockRunning();
        
        playable.stopClock();
    }
    
    public void togglePause() {
        assert !isOver();
        
        if (isClockRunning()) {
            stopClock();
        } else {
            startClock();
        }
    }
}
