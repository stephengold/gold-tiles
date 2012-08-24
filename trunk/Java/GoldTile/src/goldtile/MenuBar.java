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

import java.awt.event.InputEvent;
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
    final public static int BOARD_SIZE_DEFAULT = 5;
    final public static int TILE_SIZE_DEFAULT = 5;

    // top-level menus
    final private JMenu fileMenu = new JMenu("File");
    final private JMenu helpMenu = new JMenu("Help");
    final private JMenu playMenu = new JMenu("Play");
    final private JMenu thinkingMenu = new JMenu("Thinking");
    final private JMenu viewMenu = new JMenu("View");
    
    // menu items
    final private JMenuItem nu;
    final private JMenuItem open;
    final private JMenuItem openRecent;
    final private JMenuItem revert;
    final private JMenuItem save;
    final private JMenuItem saveAs;
    final private JMenuItem close;        
    final private JMenuItem exit;
    
    final private JMenuItem play;
    final private JMenuItem takeBack;
    final private JMenuItem suggest;
    final private JMenuItem pause;
    final private JMenuItem swapAll;
    final private JMenuItem pass;
    final private JMenuItem resign;
    final private JMenuItem restart;
    final private JMenuItem undo;
    final private JMenuItem redo;
    final private JCheckBoxMenuItem autoPause;

    final private SizeMenu boardSize;
    final private SizeMenu tileSize;
    final private JMenuItem recenter;    
    final private JMenuItem attrs;    
    final private JMenuItem hints;    
    final private JCheckBoxMenuItem showClocks;
    final private JCheckBoxMenuItem showGrid;
    final private JCheckBoxMenuItem showScores;
    final private JCheckBoxMenuItem peek;
 
    final private JMenuItem cancel;    
 
    // constructors
    
    @SuppressWarnings("LeakingThisInConstructor")
    public MenuBar() {
        // Flesh out the "File" menu.
        
        nu = new JMenuItem("New");
        nu.addActionListener(this);     
        nu.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_N, InputEvent.CTRL_DOWN_MASK));
        nu.setMnemonic(KeyEvent.VK_N);
        fileMenu.add(nu);
        
        open = new JMenuItem("Open...");
        open.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_O, InputEvent.CTRL_DOWN_MASK));
        open.setMnemonic(KeyEvent.VK_O);
        fileMenu.add(open);
        
        openRecent = new JMenuItem("Open Recent...");
        openRecent.setMnemonic(KeyEvent.VK_R);
        fileMenu.add(openRecent);
        
        revert = new JMenuItem("Revert");
        revert.setMnemonic(KeyEvent.VK_V);
        fileMenu.add(revert);
                    
        fileMenu.addSeparator();
                
        save = new JMenuItem("Save");
        save.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_S, InputEvent.CTRL_DOWN_MASK));
        save.setMnemonic(KeyEvent.VK_S);
        fileMenu.add(save);
        
        saveAs = new JMenuItem("Save As...");
        saveAs.setMnemonic(KeyEvent.VK_A);
        fileMenu.add(saveAs);
            
        fileMenu.addSeparator();
                
        close = new JMenuItem("Close");
        close.addActionListener(this);     
        close.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_W, InputEvent.CTRL_DOWN_MASK));
        close.setMnemonic(KeyEvent.VK_C);
        fileMenu.add(close);
        
        exit = new JMenuItem("Exit");
        exit.addActionListener(this);     
        exit.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_F4, InputEvent.ALT_DOWN_MASK));
        exit.setMnemonic(KeyEvent.VK_E);
        fileMenu.add(exit);

        // Flesh out the "Play" menu.
        
        play = new JMenuItem("Play");
        play.addActionListener(this);        
        play.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0));
        play.setMnemonic(KeyEvent.VK_P);
        playMenu.add(play);

        takeBack = new JMenuItem("Take Back");
        takeBack.addActionListener(this);        
        takeBack.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0));
        takeBack.setMnemonic(KeyEvent.VK_B);
        playMenu.add(takeBack);
        
        suggest = new JMenuItem("Suggest");
        suggest.addActionListener(this);        
        suggest.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, 0));
        suggest.setMnemonic(KeyEvent.VK_G);
        playMenu.add(suggest);
        
        pause = new JMenuItem("TODO");
        pause.addActionListener(this);        
        pause.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_PAUSE, 0));
        pause.setActionCommand("togglePause");
        playMenu.add(pause);
        
        swapAll = new JMenuItem("Swap All");
        swapAll.addActionListener(this);        
        swapAll.setMnemonic(KeyEvent.VK_W);
        playMenu.add(swapAll);

        pass = new JMenuItem("Pass");
        pass.addActionListener(this);        
        pass.setMnemonic(KeyEvent.VK_S);
        playMenu.add(pass);

        resign = new JMenuItem("Resign");
        resign.addActionListener(this);
        resign.setMnemonic(KeyEvent.VK_R);
        playMenu.add(resign);
            
        playMenu.addSeparator();
        
        restart = new JMenuItem("Restart");
        restart.setMnemonic(KeyEvent.VK_E);
        playMenu.add(restart);
        
        undo = new JMenuItem("Undo");
        undo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, 0));
        undo.setMnemonic(KeyEvent.VK_U);
        playMenu.add(undo);
        
        redo = new JMenuItem("Redo");
        redo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, 0));
        redo.setMnemonic(KeyEvent.VK_D);
        playMenu.add(redo);
                    
        playMenu.addSeparator();
        
        autoPause = new JCheckBoxMenuItem("AutoPause");
        playMenu.add(autoPause);
        
        // Flesh out the "View" menu.
        
        boardSize = new SizeMenu("Board Size", BOARD_SIZE_DEFAULT);
        boardSize.accelerate();
        boardSize.menuBar = this;
        boardSize.setMnemonic(KeyEvent.VK_B);
        viewMenu.add(boardSize);
        
        tileSize = new SizeMenu("Tile Size", TILE_SIZE_DEFAULT);
        tileSize.menuBar = this;
        tileSize.setMnemonic(KeyEvent.VK_T);
        viewMenu.add(tileSize);
        
        recenter = new JMenuItem("Re-center");
        recenter.addActionListener(this);
        recenter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, 0));
        recenter.setMnemonic(KeyEvent.VK_R);
        viewMenu.add(recenter);
        
        attrs = new JMenuItem("Tile Attributes...");
        attrs.setMnemonic(KeyEvent.VK_A);
        viewMenu.add(attrs);
        
        hints = new JMenuItem("Hints...");
        hints.addActionListener(this);
        hints.setMnemonic(KeyEvent.VK_H);
        viewMenu.add(hints);

        viewMenu.addSeparator();

        showClocks = new JCheckBoxMenuItem("Show Clocks");
        showClocks.addActionListener(this);        
        showClocks.setActionCommand("repaint");
        showClocks.setMnemonic(KeyEvent.VK_C);
        viewMenu.add(showClocks);
        
        showGrid = new JCheckBoxMenuItem("Show Grid");
        showGrid.addActionListener(this);        
        showGrid.setActionCommand("repaint");
        showGrid.setMnemonic(KeyEvent.VK_G);
        viewMenu.add(showGrid);
        
        showScores = new JCheckBoxMenuItem("Show Scores");
        showScores.addActionListener(this);     
        showScores.setActionCommand("repaint");
        showScores.setMnemonic(KeyEvent.VK_O);
        viewMenu.add(showScores);
        
        peek = new JCheckBoxMenuItem("Peek");
        peek.addActionListener(this);
        peek.setActionCommand("repaint");
        peek.setMnemonic(KeyEvent.VK_P);
        viewMenu.add(peek);
                                
        // Flesh out the "Thinking" menu.
        
        cancel = new JMenuItem("Cancel");
        cancel.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_C, InputEvent.CTRL_DOWN_MASK));
        cancel.setMnemonic(KeyEvent.VK_C);
        thinkingMenu.add(cancel);
        
        // Flesh out the "Help" menu.
        
        final JMenuItem rules = new JMenuItem("Rules...");
        rules.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F1, 0));
        rules.setMnemonic(KeyEvent.VK_R);
        helpMenu.add(rules);
        
        final JMenuItem warranty = new JMenuItem("Warranty...");
        warranty.setMnemonic(KeyEvent.VK_W);
        helpMenu.add(warranty);
        
        final JMenuItem about = new JMenuItem("About Gold Tile...");
        about.setMnemonic(KeyEvent.VK_A);
        helpMenu.add(about);

        // Add menus to the bar.
        add(fileMenu);
        add(playMenu);
        add(viewMenu);
        add(thinkingMenu);
        add(helpMenu);
    }
    
    // methods, sorted by name
    
    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        final String command = event.getActionCommand();
        
        switch (command) {
            case "New":
                clientArea.offerNewGame();
                break;
            case "Close":
                clientArea.view.changeGame(null);
                break;
            case "Pass":
            case "Play":
            case "Resign":
                clientArea.view.playMenuCommand(command);
                break;
            case "Take Back":
                clientArea.view.takeBack();
                break;
            case "Suggest":
                clientArea.view.suggest();
                break;
            case "Swap All":
                clientArea.view.swapAll();
                break;
            case "Re-center":
                clientArea.recenter();
                break;
            case "Hints...":
                clientArea.view.hintBox();
                break;
            case "togglePause":
                clientArea.view.togglePause();
                break;
            case "repaint": 
                break;
            case "Exit":
                System.exit(0);              
            default:
                throw new AssertionError(command);
        }
        
        clientArea.repaint();
        update();
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
            case SWAP_AREA:
                return tileSize.getValue();
            default:
                throw new AssertionError(place);
        }
    }

    public boolean isAutoPauseEnabled() {
        return autoPause.isSelected();
    }

    public boolean isGridVisible() {
        return showGrid.isSelected();
    }
    
    public boolean isPeeking() {
        return peek.isSelected();
    }

    public void loadUser(User user) {
        autoPause.setState(user.getAutopause());
        boardSize.setValue(user.getBoardTileSize());
        tileSize.setValue(user.getBoardTileSize());
        peek.setState(user.getPeek());
        showClocks.setState(user.getShowClocks());
        showGrid.setState(user.getShowGrid());
        showScores.setState(user.getShowScores());
    }
    
    public void newGame(GameStyle oldStyle, GameStyle newStyle) {
        assert oldStyle != null;
        assert newStyle != null;
        
        boardSize.setValue(BOARD_SIZE_DEFAULT);    
        tileSize.setValue(TILE_SIZE_DEFAULT);
        
        if (!oldStyle.equals(newStyle)) {
            autoPause.setState(newStyle.hasTimeLimit());
            peek.setState(newStyle.allowsPeeking());
            showClocks.setState(newStyle.showClocks());
            showGrid.setState(true);
            showScores.setState(true);
        }
        
        if (!newStyle.allowsPeeking()) {
            peek.setState(false);
        }
    }
    
    public void saveUser(User user) {
        user.setAutopause(isAutoPauseEnabled());
        user.setBoardTileSize(getTileSize(Place.BOARD));
        user.setHandTileSize(getTileSize(Place.HAND));
        user.setPeek(isPeeking());
        user.setShowClocks(areClocksVisible());
        user.setShowGrid(isGridVisible());
        user.setShowScores(areScoresVisible());    
    }
    
    public void update() {
        boolean control = GoldTile.control == Display.GUI;
        boolean haveGame = false;
        boolean local = false;
        boolean over = false;
        boolean paused = false;
        boolean canRedo = false;
        boolean canUndo = false;
        
        boolean passFlag = false;
        
        boolean suggestStyle = false;
        boolean timedStyle = false;
        boolean undoStyle = false;
        
        final Partial partial = clientArea.view;
        if (partial != null) {
            haveGame = partial.hasGame();
            if (haveGame) {
                local = partial.getPlayable().getOpt().isLocalUser();
                
                final ReadGame game = partial.getGame();
                paused = game.isPaused();
                over = game.isOver();
                canRedo = game.canRedo();
                canUndo = game.canUndo();
            }
            passFlag = partial.isPass();
                        
            final GameStyle style = partial.getStyle();
            if (style != null) {
                suggestStyle = style.allowsHints(); 
                timedStyle = style.hasTimeLimit(); 
                undoStyle = style.allowsUndo();
            }
        }
        
        // "File" menu
        nu.setEnabled(true);
        open.setEnabled(true);
        openRecent.setEnabled(true);
        revert.setEnabled(haveGame);
        save.setEnabled(haveGame);
        saveAs.setEnabled(haveGame);
        close.setEnabled(haveGame);
        exit.setEnabled(true);
        updateMenu(fileMenu);

        // "Play" menu
        pause.setText(paused ? "Resume" : "Pause");
        final boolean cgl = control && haveGame && local;
        play.setEnabled(cgl && !over && !paused && !passFlag);
        takeBack.setEnabled(cgl && !over && !paused && !passFlag);
        suggest.setEnabled(cgl && !over && !paused && suggestStyle);
        pause.setEnabled(cgl && !over);
        swapAll.setEnabled(cgl && !over && !paused && passFlag);
        pass.setEnabled(cgl && !over && !paused && passFlag);
        resign.setEnabled(cgl && !over && !paused);
        restart.setEnabled(control && haveGame && !paused && undoStyle);
        undo.setEnabled(control && haveGame && !paused && undoStyle && canUndo);
        redo.setEnabled(control && haveGame && !paused && undoStyle && canRedo);
        autoPause.setEnabled(control && !timedStyle);
        updateMenu(playMenu);

        // "View" menu
        boardSize.setEnabled(true);
        tileSize.setEnabled(true);
        recenter.setEnabled(haveGame && !paused);
        attrs.setEnabled(true);
        hints.setEnabled(true);
        showClocks.setEnabled(true);
        showGrid.setEnabled(true);
        showScores.setEnabled(true);
        updateMenu(viewMenu);

        // "Thinking" menu
        cancel.setEnabled(false);
        updateMenu(thinkingMenu);

        // "Help" menu
        helpMenu.setEnabled(true);
        updateMenu(helpMenu);
    }

    private static void updateMenu(JMenu menu) {
        for (java.awt.Component component : menu.getMenuComponents()) {
            if (component.isEnabled()) {
                menu.setEnabled(true);
                return;
            }
        }
        
        menu.setEnabled(false);        
    }
}
