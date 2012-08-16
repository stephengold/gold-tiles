// File:     MenuBar.java
// Location: Java
// Purpose:  MenuBar class for the Gold Tile Game
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
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

public class MenuBar 
    extends javax.swing.JMenuBar 
    implements java.awt.event.ActionListener
{
    public GamePanel clientArea = null;
    
    // constants
    final private static int BOARD_SIZE_DEFAULT = 5;
    final private static int TILE_SIZE_DEFAULT = 5;

    // top-level menus
    final private JMenu fileMenu = new JMenu("File");
    final private JMenu helpMenu = new JMenu("Help");
    final private JMenu playMenu = new JMenu("Play");
    final private JMenu thinkingMenu = new JMenu("Thinking");
    final private JMenu viewMenu = new JMenu("View");
    
    // submenus
    final private SizeMenu boardSize 
            = new SizeMenu("Board Size", BOARD_SIZE_DEFAULT);
    final private SizeMenu tileSize 
            = new SizeMenu("Tile Size", TILE_SIZE_DEFAULT);
    
    // menu items
    final private JCheckBoxMenuItem showClocks 
            = new JCheckBoxMenuItem("Show Clocks");
    final private JCheckBoxMenuItem showGrid 
            = new JCheckBoxMenuItem("Show Grid");
    final private JCheckBoxMenuItem showScores 
            = new JCheckBoxMenuItem("Show Scores");
    final private JCheckBoxMenuItem peek 
            = new JCheckBoxMenuItem("Peek");
    
    public MenuBar() {
        // flesh out the File menu
        final JMenuItem exit = new JMenuItem("Exit");
        fileMenu.add(exit);

        // flesh out the View menu
        boardSize.accelerate();
        boardSize.menuBar = this;
        boardSize.setMnemonic(KeyEvent.VK_B);
        viewMenu.add(boardSize);
        
        tileSize.menuBar = this;
        tileSize.setMnemonic(KeyEvent.VK_T);
        viewMenu.add(tileSize);
        
        final JMenuItem recenter = new JMenuItem("Re-center");
        recenter.addActionListener(this);
        recenter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, 0));
        recenter.setActionCommand("recenterBoard");
        recenter.setMnemonic(KeyEvent.VK_R);
        viewMenu.add(recenter);
        
        viewMenu.addSeparator();

        showClocks.addActionListener(this);        
        showClocks.setActionCommand("repaintBoard");
        showClocks.setMnemonic(KeyEvent.VK_C);
        viewMenu.add(showClocks);
        
        showGrid.addActionListener(this);        
        showGrid.setActionCommand("repaintBoard");
        showGrid.setMnemonic(KeyEvent.VK_G);
        viewMenu.add(showGrid);
        
        showScores.addActionListener(this);      
        showScores.setActionCommand("repaintBoard");
        showScores.setMnemonic(KeyEvent.VK_O);
        viewMenu.add(showScores);
        
        peek.setMnemonic(KeyEvent.VK_P);
        viewMenu.add(peek);
        
        // add menus to the bar
        add(fileMenu);
        add(playMenu);
        add(viewMenu);
        add(thinkingMenu);
        add(helpMenu);
    }
    
    @Override
    public void actionPerformed(ActionEvent event) {
        final String command = event.getActionCommand();
        
        switch (command) {
            case "recenterBoard":
                recenterBoard();
                break;
            case "repaintBoard": 
                repaintBoard();
                break;
            default:
                throw new AssertionError(command);
        }   
    }

    public boolean areClocksVisible() {
        return showClocks.isSelected();
    }
    
    public boolean areScoresVisible() {
        return showScores.isSelected();
    }
    
    public int getTileSize(Place place) {
        switch (place) {
            case BOARD:
                return boardSize.getValue();
            case HAND:
                return tileSize.getValue();
            default:
                throw new AssertionError(place);
        }
    }

    public boolean isGridVisible() {
        return showGrid.isSelected();
    }
    
    public boolean isPeeking() {
        return peek.isSelected();
    }

    public void recenterBoard() {
        if (clientArea != null) {
            clientArea.recenter();
        }
    }
    
    public void repaintBoard() {
        if (clientArea != null) {
            clientArea.repaint();
        }
    }
}
