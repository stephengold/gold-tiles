// File:     Hand.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Hand class for the Gold Tile Game
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

public class Hand 
    implements ReadHand
{   
    // per-instance fields
    private boolean clockRunningFlag = false;
    private boolean resignedFlag = false;
    final private HandOpt opt;
    private int score = 0;
    private long milliseconds = 0;    // total time charged to the hand
    private long startTime = 0;
    final private String name;        // unique name for the hand
    private Tiles contents = new Tiles();
    
    // constructors
    
    public Hand(String name, HandOpt opt) {
        assert name != null;
        assert opt != null;
        
        this.opt = new HandOpt(opt);
        this.name = name;
    }
    
    public Hand(Hand other) {
        assert other != null;
        
        this.clockRunningFlag = other.clockRunningFlag;
        this.resignedFlag = other.resignedFlag;
        this.opt = new HandOpt(other.opt);
        this.score = other.score;
        this.milliseconds = other.milliseconds;
        this.startTime = other.startTime;
        this.name = other.name;
        this.contents = new Tiles(other.contents);
    }

    // methods
    
    public void addScore(int pointCount) {
        assert !isClockRunning();
        assert pointCount >= 0 : pointCount;

        final int newScore = score + pointCount;
        assert newScore >= score : score; // check for wraparound
        score = newScore;
    }
    
    public void addTiles(Tiles tiles) {
        contents.addAll(tiles);    
    }
    
    @Override
    public Move chooseMoveAutomatic(Game game) {
        // TODO
        return null;
    }
    
    @Override
    public Move chooseMoveConsole(int mustPlay) {
        assert mustPlay >= 0;
        assert isClockRunning();
        assert opt.isLocalUser();

        final String description = describeContents();
        Global.print(description);
        
        return Move.chooseConsole(contents, mustPlay); 
    }
    
    @Override
    public Move chooseMoveRemote() {
        // TODO
        return null;
    }
    
    @Override
    public Tiles copyContents() {
        return new Tiles(contents);
    }
    
    @Override
    public int countContents() {
        return contents.size();
    }
    
    @Override
    public String describeContents() {
        final int count = contents.size();

        return String.format("%s holds %s: %s.\n",
                name,
                StringExt.plural(count, "tile"), 
                contents.describe());
    }
    
    @Override
    public String describeScore() {
        return String.format("%s has %s.\n",
                name, 
                StringExt.plural(score, "point"));
    }
    
    @Override
    public Tiles findLongestRun() {
        return contents.findLongestRun();
    }
    
    @Override
    public long getMillisecondsUsed() {
        return milliseconds;
    }
    
    @Override
    public String getName() {
        return name;
    }
    
    @Override
    public ReadHandOpt getOpt() {
        return opt;    
    }
    
    @Override
    public int getSecondsUsed() {
        return (int)(getMillisecondsUsed()/Global.MILLISECONDS_PER_SECOND);
    }
    
    @Override
    public int getScore() {
        return score;
    }
    
    @Override
    public boolean hasGoneOut() {
        return contents.size() == 0 && !resignedFlag;
    }
    
    @Override
    public boolean hasResigned() {
        return resignedFlag;    
    }
    
    @Override
    public boolean isClockRunning() {
        return clockRunningFlag;
    }
    
    @Override
    public boolean isDisconnected() {
        return false; // TODO
    }
    
    public void removeTiles(Tiles tiles) {
        contents.removeAll(tiles);
    }
    
    public Tiles resign() {
        assert !isClockRunning();
        assert !hasResigned();

        final Tiles tiles = contents;
        contents = new Tiles();
        resignedFlag = true;

        assert hasResigned();
        return tiles;
    }
    
    public void startClock() {
        assert !clockRunningFlag;

        startTime = System.currentTimeMillis();
        clockRunningFlag = true;
    }
    
    public void stopClock() {
        assert clockRunningFlag;

        milliseconds = System.currentTimeMillis();
        clockRunningFlag = false;
    }    
}
