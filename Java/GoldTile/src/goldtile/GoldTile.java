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
    public static boolean consoleFlag = true;
    public static boolean debugFlag = true;
    public static boolean guiFlag = false;
    public static Display control = Display.CONSOLE;
    public static boolean initialGameFlag = true;
    
    // methods
    
    private static Game initialGame() {
        if (!initialGameFlag) {
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
                assert guiFlag;
                
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
        result.initialize();
        
        return result;
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if (!consoleFlag) {
            Global.disableConsole();
        }
        
        final Game game = initialGame();
        
        if (guiFlag) {
            javax.swing.SwingUtilities.invokeLater(new Runnable() {
                @Override 
                public void run()  {
                    GameFrame frame = new GameFrame(game);
                }
            });
        }
        
        switch (control) {
            case CONSOLE:
                game.playConsole();
                break;
                
            case GUI:
                assert guiFlag;
                break;
                
            default:
                throw new AssertionError();
        }
    }
    
}