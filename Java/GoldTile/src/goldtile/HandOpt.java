// File:     HandOpt.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  HandOpt class for the Gold Tile Game
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

import java.net.InetAddress;
import java.net.UnknownHostException;

public class HandOpt
    implements ReadHandOpt
{
    // constants
    final public static Fraction SKIP_PROBABILITY_DEFAULT = new Fraction(0.0);
    final public static int LEVEL_DEFAULT = 8;
    final public static int LEVEL_MAX = 8;
    final public static int LEVEL_MIN = 0;
    final public static String IP_ADDRESS_DEFAULT = "172.0.0.1";
    final public static String COMPUTER = "Computer";

    // per-instance fields (all mutable)
    private boolean automaticFlag;
    private boolean remoteFlag;
    private int level;          // ignored except in automatic hands
    private String ipAddress;   // ignored except in remote hands
    private String playerName;  // ignored in automatic hands

    // constructors

    public HandOpt(String playerName) {
        assert playerName != null;

        remoteFlag = false;
        level = LEVEL_DEFAULT;
        ipAddress = IP_ADDRESS_DEFAULT;

        if (playerName.equals(COMPUTER)) {
            automaticFlag = true;
            this.playerName = "";
        } else {
            automaticFlag = false;
            this.playerName = playerName;
        }

        assert isValid();
    }

    public HandOpt(ReadHandOpt other) {
        assert other != null;
        assert other.isValid();

        automaticFlag = other.isAutomatic();
        remoteFlag = other.isRemote();
        level = other.getLevel();
        ipAddress = other.getIpAddress();
        playerName = other.getSavedName();

        assert isValid();
    }

    // methods, sorted by name

    public static HandOpt chooseConsole(int iHand) {
        assert iHand >= 0;

        String playerName = "";
        while (playerName.isEmpty()) {
            Console.printf("Who will play the %s hand?\n",
                    StringExt.ordinal(iHand + 1) );
            Console.print(
                    " ('computer' or 'network' or the name of a local user) ");
            playerName = Console.readLine();
            playerName = StringExt.normalizeName(playerName);
        }

        HandOpt result;
        switch (playerName) {
            case COMPUTER:
                result = new HandOpt(COMPUTER);
                // TODO set level
                break;

            case "Network":
                while (playerName.isEmpty()) {
                    playerName = Console.readLine("Name of network player? ");
                    playerName = StringExt.normalizeName(playerName);
                }
                result = new HandOpt(playerName);
                result.setRemote();
                // TODO set address
                break;

            default:
                result = new HandOpt(playerName);
        }

        return result;
    }

    public static Strings getAllPlayerNames(int count, ReadHandOpt[] opts) {
        final Strings result = new Strings();

        for (int iHand = 0; iHand < count; iHand++) {
            final ReadHandOpt opt = opts[iHand];
            result.addLast(opt.getPlayerName());
        }

        return result;
    }

    @Override
    public String getIpAddress() {
        return ipAddress;
    }

    @Override
    public int getLevel() {
        return level;
    }

    @Override
    public String getPlayerName() {
        if (isAutomatic()) {
            return COMPUTER;
        } else {
            return playerName;
        }
    }

    @Override
    public String getSavedName() {
        return playerName;
    }

    @Override
    public Fraction getSkipProbability() {
        return new Fraction(0.1 * (double)(LEVEL_MAX - level));
    }

    @Override
    public boolean isAutomatic() {
        return automaticFlag;
    }

    @Override
    public boolean isLocalUser() {
        return !automaticFlag && !remoteFlag;
    }

    @Override
    public boolean isRemote() {
        return remoteFlag;
    }

    @Override
    final public boolean isValid() {
        if (automaticFlag && remoteFlag) {
            return false;
        } else if (level < LEVEL_MIN) {
            return false;
        } else if (level > LEVEL_MAX) {
            return false;
        } else if (playerName == null) {
            return false;
        } else if (playerName.equals(COMPUTER)) {
            return false;
        } else try {
            final String normal = InetAddress.getByName(ipAddress).getHostAddress();
            return ipAddress.equals(normal);
        } catch (UnknownHostException exception) {
            return false;
        }
    }

    public void setAutomatic() {
        automaticFlag = true;
        remoteFlag = false;
    }

    public void setIpAddress(String address) {
        assert ipAddress != null;

        ipAddress = address;
    }

    public void setLevel(int level) {
        assert level >= LEVEL_MIN;
        assert level <= LEVEL_MAX;

        this.level = level;
    }

    public void setLocalUser() {
        automaticFlag = false;
        remoteFlag = false;
    }

    public void setPlayerName(String name) {
        assert name != null;
        assert !name.isEmpty();
        assert !name.equals(COMPUTER);

        playerName = name;
    }

    public void setRemote() {
        automaticFlag = false;
        remoteFlag = true;
    }
}
