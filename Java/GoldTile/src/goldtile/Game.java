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
    // constants
    final private static int STUCK_THRESHOLD = 7;  
            // number of turns before game is declared "stuck" 

    // fields   
    private Board board = new Board(); // extensible playing surface
    final private boolean amClient;
    private boolean hasUnsavedChanges = false;
    final private GameOpt opt;
    private Hand playable = null;  // whose turn it is
    private Hands hands = new Hands(); // all hands being played
    private HandOpts handOpts = null;
    private int mustPlay = 0;  // min number of tiles to play, 0 after the 1st turn
    public String bestRunReport = null;
    public String filespec = null;     // associated file for load/save
    public String firstTurnMessage = null;
    private Tiles stockBag = null;
    private Turn redo = null;          // current index in the history
    final private Turns history = new Turns();
    
    // constructor
    
    public Game(GameOpt opt, HandOpts handOpts) {
        assert opt != null;
        opt.validate();
        assert handOpts != null;
        assert handOpts.size() == opt.getHandsDealt();

        // Save a copy of the options.
        this.opt = new GameOpt(opt);

        amClient = true; // TODO
        
        // Intialize static data of the Cell and Tile classes.
        Cell.setStatic(opt);
        Tile.setStatic(opt);
        
        // Re-seed the psedo-random generator.
        opt.reseedGenerator();
        
        // Add tiles to the stock bag.
        stockBag = new Tiles();
        if (amClient) {
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
            hands.addLast(hand);
        }
        
        assert hands.size() == opt.getHandsDealt();
        assert playable == null;
    }

    // methods
    
    public void activateNextHand() {
        // TODO
    }
    
    public boolean canRedo() {
        return !redo.isDummy();
    }
    
    public boolean canUndo() {
        return false; // TODO
    }

    public UserMessage checkMove(Move move) {
        // TODO
        return null;    
    }
    
    public Board copyBoard() {
        assert board != null;
        
        return new Board(board);
    }
    
    public GameOpt copyOpt() {
        return new GameOpt(opt);
    }
    
    public Hand copyPlayableHand() {
        assert playable != null;
        
        return new Hand(playable);
    }

    public Tiles copyPlayableTiles() {
        assert playable != null;
        
        return playable.copyContents();
    }
    
    final public int countStock() {
        assert stockBag != null;
        
        return stockBag.size();
    }

    private void finishTurn(Move move) {
        // TODO assert isLegalMove(move);
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

        if (rMove.InvolvesSwap()) {
            // Return the swapped tiles to the stock bag.
            mStockBag.Merge(tiles);
            std::cout << miPlayableHand->Name() << " put " << ::plural(count, "tile")
                << " back into the stock bag." << std::endl;

        } else {
            // Place played tiles on the board.
            mBoard.PlayMove(rMove);

            // Update the hand's score.
            ScoreType const points = mBoard.ScoreMove(rMove);
            miPlayableHand->AddScore(points);
            turn.SetPoints(points);
        }
    }

    AddTurn(turn);

    //  If it was the first turn, it no longer is.
    mMustPlay = 0;

    // This game definitely has some unsaved changes now.
    mUnsavedChanges = true;

    ASSERT(!IsClockRunning());
    return true;

    }
    
    private void firstTurnConsole() {
        assert playable != null;
        assert isPaused();
        
        Global.print("\n");
        startClock();
        
        Move move;
        if (playable.isAutomatic()) {
            assert amClient;
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
                
                Global.print("\nThat isn't a legal move because:\n");
                Global.print(reason.message);
                Global.print("\nTry again ...\n\n");
                Global.print(firstTurnMessage);
            }
        }
        finishTurn(move);
    }
    
    private void findBestRun() {
        assert board != null;
        assert board.size() == 0 : board.size();
        assert hands != null;
        assert hands.size() > 0 : hands.size();
        
        bestRunReport = "";
        mustPlay = 0;
        playable = hands.getFirst();
        
        for (Hand hand : hands) {
            final Tiles run = hand.getLongestRun();
            final int runLength = run.size();

            bestRunReport += hand.name;
            bestRunReport += " has a run of ";
            bestRunReport += StringExt.plural(runLength, "tile");
            bestRunReport += ": " + run.describe() + "\n";

            if (runLength > mustPlay) {
                mustPlay = runLength;
                playable = hand;
            } 
        }
        bestRunReport += "\n";

        // Make sure the run will fit on the board.
        mustPlay = Cell.limitPlay(mustPlay);

        firstTurnMessage = "";
        firstTurnMessage += playable.name;
        firstTurnMessage += " plays first and must place "; 
        firstTurnMessage += StringExt.plural(mustPlay, "tile");
        firstTurnMessage += " on the (empty) board.\n";

        bestRunReport += firstTurnMessage;
        Global.print(bestRunReport);
        
        assert bestRunReport != null;
        assert firstTurnMessage != null;
        assert mustPlay > 0 : mustPlay;
        assert playable != null;
    }
    
    public void initialize() {
        assert playable == null : playable;
        
        // The deal:  each hand pulls tiles from the stock bag.
        final int handSize = opt.getHandSize();
        for (Hand hand : hands) {
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
        redo = new Turn();

        // Pretend this game has been saved, for convenience.
        hasUnsavedChanges = false;

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
    
    public boolean isOver() {
        return false;
    }

    public boolean isPaused() {
        return !isClockRunning() && !isOver();
    }

    private void nextTurnConsole() {
        // TODO        
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
        
        if (amClient) {
            try {
               result = stockBag.pullRandom(count);
            } catch (Tiles.PullEmptyException exception) {
               // caller will notice
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
        String result = "";
        
        return result;
    }
    
    private String reportScores() {
        String result = "";
        
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
}
