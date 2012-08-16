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

public class Hand {
    
    // per-instance fields
    private boolean clockRunningFlag = false;
    private boolean resignedFlag = false;
    final private HandOpt opt;
    private int score = 0;
    private long milliseconds = 0;     // total time charged to the hand
    private long startTime = 0;
    final public String name;        // the "name" of the hand
    private Tiles contents = new Tiles();
    
    // constructor
    
    public Hand(String name, HandOpt opt) {
        this.name = new String(name);
        this.opt = new HandOpt(opt);
    }
    
    public Hand(Hand other) {
        assert other != null;
        
        clockRunningFlag = other.clockRunningFlag;
        resignedFlag = other.resignedFlag;
        opt = new HandOpt(other.opt);
        score = other.score;
        milliseconds = other.milliseconds;
        startTime = other.startTime;
        name = other.name;
        contents = new Tiles(other.contents);
    }
    
    // methods
    
    public void add(Tiles tiles) {
        contents.addAll(tiles);    
    }
    
    public Move chooseMoveAutomatic(Game game) {
        // TODO
        return null;
    }
    
    public Move chooseMoveConsole(int mustPlay) {
        assert mustPlay >= 0;
        assert isClockRunning();
        assert isLocalUser();

        final String description = describeContents();
        Global.print(description);
        
        return Move.chooseConsole(contents, mustPlay); 
    }
    
    public Move chooseMoveRemote() {
        // TODO
        return null;
    }
    
    public Tiles copyContents() {
        return new Tiles(contents);
    }
    
    public String describeContents() {
         String result = name;
         final int count = contents.size();
         result +=  " holds ";
         result += StringExt.plural(count, "tile");
         result += ": ";
         result += contents.describe();
         result += ".\n";

         return result;
    }
    
    public Tiles getLongestRun() {
        return contents.getLongestRun();
    }
    
    public boolean hasResigned() {
        return resignedFlag;    
    }
    
    public boolean isAutomatic() {
        return opt.isAutomatic();
    }
    
    public boolean isClockRunning() {
        return clockRunningFlag;
    }
    
    public boolean isLocalUser() {
        return opt.isLocalUser();
    }
    
    public boolean isRemote() {
        return opt.isRemote();
    }
    
    public void printName() {
        Global.print(name);
    }
    
    public void remove(Tiles tiles) {
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
