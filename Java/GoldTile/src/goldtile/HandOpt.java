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

public class HandOpt implements ReadHandOpt {
    // constants
    public static Fraction SKIP_PROBABILITY_DEFAULT = new Fraction(0.0);
    
    // per-instance fields
    private boolean automaticFlag;
    private boolean remoteFlag;
    private Fraction skipProbability; // ignored except in automatic hands
    private String playerName;

    // constructors
    
    public HandOpt(String playerName) {
        automaticFlag = false;
        remoteFlag = false;
        skipProbability = new Fraction(SKIP_PROBABILITY_DEFAULT);
        this.playerName = playerName;
    }
    
    public HandOpt(HandOpt other) {
        assert other.skipProbability != null;
        
        automaticFlag = other.automaticFlag;
        remoteFlag = other.remoteFlag;
        skipProbability = new Fraction(other.skipProbability);
        playerName = other.playerName;
    }
    
    // methods
    
    public static HandOpt chooseConsole(int iHand) {
        assert iHand >= 0;

        String playerName = "";
        while (playerName.isEmpty()) {
            System.out.print("Who will play the "); 
            System.out.print(StringExt.ordinal(iHand + 1));
            System.out.println(" hand?");
            System.out.print(
                    " ('computer' or 'network' or the name of a local user) ");
            playerName = Global.readLine();
            playerName = StringExt.normalizeName(playerName);
        }

        HandOpt result = new HandOpt(playerName);
        switch (playerName) {
            case "Computer":
                result.automaticFlag = true;
                result.remoteFlag = false;
                // TODO set skip probability
                break;
                
            case "Network":
                result.automaticFlag = false;
                result.remoteFlag = true;
                playerName = "";
                while (playerName.isEmpty()) {
                    System.out.print("Name of network player? ");
                    playerName = Global.readLine();
                    playerName = StringExt.normalizeName(playerName);
                }
                // TODO set address
                break;

            default:
                result.automaticFlag = false;
                result.remoteFlag = false;
        }
        
        return result;
    }
    
    @Override
    public String getPlayerName() {
        return playerName;
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
    
    public boolean isValid() {
        if (automaticFlag && remoteFlag) {
            return false;
        }
        if (automaticFlag && skipProbability.toDouble() >= 1.0) {
            return false;
        }
        if (playerName.isEmpty()) {
            return false;
        }
        
        return true;
    }
}
