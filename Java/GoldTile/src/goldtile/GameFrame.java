// File:     GameFrame.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  GameFrame class for the Gold Tile Game
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

public class GameFrame extends javax.swing.JFrame {
    public GameFrame(Game game) {
        setTitle("Gold Tile Game");
        if (GoldTile.control == Display.GUI) {
            setDefaultCloseOperation(EXIT_ON_CLOSE);
        }
        
        // Create the menu bar.
        final MenuBar menuBar = new MenuBar();
        setJMenuBar(menuBar);
        
        // Create the panel for the client area.
        final GamePanel clientArea = new GamePanel(menuBar);
        menuBar.clientArea = clientArea;

        // Lay out the frame.
        final java.awt.Container content = getContentPane();
        content.add(clientArea, java.awt.BorderLayout.CENTER);
        pack();
        
        // Associate the game (if any) with the clientArea.
        clientArea.view.setGame(game);
        
        // Display the frame.
        setVisible(true);
    }
}
