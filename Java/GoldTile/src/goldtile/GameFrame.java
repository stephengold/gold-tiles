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

import java.awt.Toolkit;

public class GameFrame
    extends javax.swing.JFrame
{
    public GameFrame() {
        setTitle("Gold Tile Game");
        
        if (GoldTile.control == Display.GUI) {
            // Closing this window will terminate the application.
            setDefaultCloseOperation(EXIT_ON_CLOSE);
        }
        
        // Create the menu bar.
        final MenuBar menuBar = new MenuBar();
        menuBar.addListeners();
        setJMenuBar(menuBar);
        
        // Create the panel for the client area.
        final GamePanel clientArea = new GamePanel(menuBar, this);
        menuBar.clientArea = clientArea;

        // Lay out the frame.
        final java.awt.Container content = getContentPane();
        content.add(clientArea, java.awt.BorderLayout.CENTER);
        pack();
        
        // initial bounds:  centered and covering 64% of the screen
        final Toolkit toolkit = Toolkit.getDefaultToolkit();
        final Area screen = new Area(toolkit.getScreenSize());
        final Area area = screen.shrink(new Fraction(0.2)); // shrink 20%
        final int x = (screen.width - area.width)/2;      // center horizontally
        final int y = (screen.height - area.height)/2;    // center vertically
        setBounds(x, y, area.width, area.height);

        // Display the frame.
        setVisible(true);

        if (GoldTile.control == Display.CONSOLE) {
            return;
        }
        if (!Game.haveInstance() && GoldTile.autoOfferGame) {
            clientArea.offerNewGame();
        } else {
            // Associate the initial game (if any) with the client area's GameView.
            clientArea.view.changeGame();
        }
    }
}
