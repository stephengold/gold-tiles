// File:     Hands.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Hands class for the Gold Tile Game
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

public class Hands
    extends java.util.ArrayList< Hand >
{
    // classes
    public class BestRun {
        public int length;
        public String report;
    }

    // constants
    private static final String PREFIX = "hands{";
    private static final String SEPARATOR = " ";
    private static final String SUFFIX = "}";

    // per-instance fields

    private int playable = 0;

    // constructors

    public Hands(int count, ReadHandOpt[] opts) {
        super();

        assert count > 0;
        assert count <= opts.length;

        // Generate a list of player names, including duplicates.
        final Strings names = HandOpt.getAllPlayerNames(count, opts);

        // Construct hands and give each one a unique name.
        for (int iHand = 0; iHand < count; iHand++) {
            final ReadHandOpt opt = opts[iHand];
            String handName = opt.getPlayerName();
            if (names.count(handName) > 1) {
                handName = names.inventUnique(handName, "'s ", " hand");
                names.addLast(handName);
            }

            final Hand hand = new Hand(handName, opt);
            assert !hand.isClockRunning();
            add(hand);
        }

        assert size() == count;
        assert playable == 0;
    }

    // methods, sorted by name

    public void addScorePlayable(int points) {
        get(playable).addScore(points);
    }

    public void addTiles(int iHand, Tiles tiles) {
        get(iHand).addTiles(tiles);
    }

    public BestRun findBestRun() {
        assert size() > 0;

        BestRun result = new BestRun();

        result.length = 0;
        result.report = "";
        playable = 0;

        int iHand = 0;
        for (Hand hand : this) {
            final Tiles run = hand.findLongestRun();
            final int runLength = run.size();

            result.report += String.format("%s has a run of %s.\n",
                    hand.getName(), StringExt.plural(runLength, "tile"));

            if (runLength > result.length) {
                result.length = runLength;
                playable = iHand;
            }
            iHand++;
        }

        return result;
    }

    public int findMaxScore() {
        assert size() > 0;

        final Hand first = get(0);
        int result = first.getScore();

        for (Hand hand : this) {
            final int score = hand.getScore();
            if (score > result) {
                result = score;
            }
        }

        return result;
    }

    public ReadHandOpt[] getOpts() {
        final ReadHandOpt[] result = new ReadHandOpt[size()];

        for (int iHand = 0; iHand < size(); iHand++) {
            result[iHand] = get(iHand).getOpt();
        }
        return result;
    }

    public ReadHand getPlayable() {
        return get(playable);
    }

    public int getPlayableIndex() {
        return playable;
    }

    public int[] getUnplayableIndices() {
        int[] result = new int[size() - 1];

        for (int iHand = 0; iHand < size(); iHand++) {
            if (iHand < playable) {
                result[iHand] = iHand;
            } else if (iHand > playable) {
                result[iHand - 1] = iHand;
            }
        }

        return result;
    }

    public boolean hasAnyGoneOut() {
        for (Hand hand : this) {
            if (hand.hasGoneOut()) {
                return true;
            }
        }

        return false;
    }

    public boolean haveAllResigned() {
        for (Hand hand : this) {
            if (!hand.hasResigned()) {
                return false;
            }
        }

        return true;
    }

    public void nextPlayable() {
        assert !get(playable).isClockRunning();

        int watchDog = size();

        for (;;) {
            playable++;
            if (playable >= size()) {
                playable = 0;
            }
            assert !getPlayable().isClockRunning();
            if (!getPlayable().hasResigned()) {
                return;
            }
            --watchDog;
            if (watchDog < 0) {
                // wrapped all the way aroud and didn't find a playable hand
                throw new AssertionError();
            }
        }
    }

    public void removePlayableTiles(Tiles tiles) {
        get(playable).removeTiles(tiles);
    }

    public Tiles resignPlayable() {
        return get(playable).resign();
    }

    /**
     * Empty all hands of their contents (in other words, their tiles).
     *
     * @return all the tiles removed
     */
    public Tiles restart() {
        assert !getPlayable().isClockRunning();

        final Tiles result = new Tiles();

        for (Hand hand : this) {
            final Tiles contents = hand.restart();
            result.addAll(contents);
        }

        return result;
    }

    public void setPlayable(int iHand) {
        assert iHand >= 0;
        assert iHand < size();
        assert !getPlayable().isClockRunning();

        playable = iHand;
        assert !getPlayable().isClockRunning();
    }

    public void startClock() {
        get(playable).startClock();
    }

    public void stopClock() {
        get(playable).stopClock();
    }

    public void subtractScorePlayable(int points) {
        get(playable).subtractScore(points);
    }

    @Override
    public String toString() {
        String result = PREFIX;

        boolean firstFlag = true;
        for (Hand hand : this) {
            if (firstFlag) {
                firstFlag = false;
            } else {
                result += SEPARATOR;
            }

            result += hand.toString();
        }
        result += SUFFIX;

        return result;
    }

    public void unResign(int iHand, Tiles tiles) {
        assert iHand >= 0;
        assert iHand < size();

        get(iHand).unResign(tiles);
    }
}
