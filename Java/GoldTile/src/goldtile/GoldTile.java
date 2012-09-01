// File:     GoldTile.java
// Location: Java
// Purpose:  main class for the Gold Tile Game
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

public class GoldTile {
    // configuration flags

    public static boolean autoOfferGame = true;
    public static boolean beClient = true;
    public static boolean debugging = true;
    public static boolean enableConsole = true;
    public static boolean enableGui = true;
    public static boolean standardGame = false;

    public static Display control = Display.GUI;

    // methods, sorted by name

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if (!enableConsole) {
            Console.disable();
        }

        if (standardGame) {
            standardGame();
        } else if (autoOfferGame && control == Display.CONSOLE) {
            offerGameConsole();
        }

        if (enableGui) {
            javax.swing.SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run()  {
                    new GameFrame();
                }
            });
        }

        switch (control) {
            case CONSOLE:
                Game.getInstance().playConsole();
                break;

            case GUI:
                assert enableGui;
                break;

            default:
                throw new AssertionError();
        }
    }

    private static Game offerGameConsole() {
        // read options from the user via the console
        final GameOpt gameOpt = GameOpt.chooseConsole();
        final int handsDealt = gameOpt.getHandsDealt();
        final HandOpt[] handOpts = new HandOpt[handsDealt];
        for (int iHand = 0; iHand < handsDealt; iHand++) {
            handOpts[iHand] = HandOpt.chooseConsole(iHand);
        }

        // Create the game.
        return new Game(gameOpt, handOpts);
    }

    private static Game standardGame() {
        // options
        final GameOpt gameOpt = new GameOpt();
        final int handsDealt = gameOpt.getHandsDealt();
        final HandOpt[] handOpts = new HandOpt[handsDealt];
        for (int iHand = 0; iHand < handsDealt; iHand++) {
            handOpts[iHand] = new HandOpt("User");
        }

        // Create the game.
        return new Game(gameOpt, handOpts);
    }
}