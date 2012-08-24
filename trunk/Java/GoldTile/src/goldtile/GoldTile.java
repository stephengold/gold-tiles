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
    public static boolean autoNewGame = false;
    public static boolean consoleEnabled = true;
    public static boolean debugging = true;
    public static boolean guiEnabled = true;
    public static boolean initialGame = true;
    public static boolean isClient = true;
    
    public static Display control = Display.GUI;
    
    // methods
    
    private static Game initialGame() {
        if (!initialGame) {
            return null;
        }
        
        // options
        GameOpt gameOpt;
        HandOpts handOpts;
 
        switch (control) {
            case CONSOLE:
                // read options from the user via the console
                gameOpt = GameOpt.chooseConsole();
                handOpts = HandOpts.chooseConsole(gameOpt);
                break;
                
            case GUI:
                assert guiEnabled;
                
                // TODO new game wizard
                gameOpt = new GameOpt();
                handOpts = new HandOpts();
                for (int iHand = 0; iHand < gameOpt.getHandsDealt(); iHand++) {
                    final HandOpt handOpt = new HandOpt("User");
                    handOpts.addLast(handOpt);
                }
                break;
                
            default:
                throw new AssertionError();
        }
        
        // Create the game.
        final Game result = new Game(gameOpt, handOpts);
        
        return result;
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if (!consoleEnabled) {
            Console.disable();
        }
        
        final Game game = initialGame();
        
        if (guiEnabled) {
            javax.swing.SwingUtilities.invokeLater(new Runnable() {
                @Override 
                public void run()  {
                    new GameFrame(game);
                }
            });
        }
        
        switch (control) {
            case CONSOLE:
                game.playConsole();
                break;
                
            case GUI:
                assert guiEnabled;
                break;
                
            default:
                throw new AssertionError();
        }
    }   
}