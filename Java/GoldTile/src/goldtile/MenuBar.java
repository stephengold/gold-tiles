// File:     MenuBar.java
// Location: Java/GoldTile/src/goldtile
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
    // constants
    final public static int BOARD_SIZE_DEFAULT = 5;
    final public static int TILE_SIZE_DEFAULT = 5; // TODO based on hand size

    // per-instance data
    
    public GamePanel clientArea = null;
    private SwingWorker worker = null;

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
    
    public MenuBar() {
        // Flesh out the "File" menu.
        
        nu = new JMenuItem("New");
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
        close.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_W, InputEvent.CTRL_DOWN_MASK));
        close.setMnemonic(KeyEvent.VK_C);
        fileMenu.add(close);
        
        exit = new JMenuItem("Exit");
        exit.setAccelerator(KeyStroke.getKeyStroke(
                KeyEvent.VK_F4, InputEvent.ALT_DOWN_MASK));
        exit.setMnemonic(KeyEvent.VK_E);
        fileMenu.add(exit);

        // Flesh out the "Play" menu.
        
        play = new JMenuItem("Play");
        play.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0));
        play.setMnemonic(KeyEvent.VK_P);
        playMenu.add(play);

        takeBack = new JMenuItem("Take Back");
        takeBack.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0));
        takeBack.setMnemonic(KeyEvent.VK_B);
        playMenu.add(takeBack);
        
        suggest = new JMenuItem("Suggest");
        suggest.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, 0));
        suggest.setMnemonic(KeyEvent.VK_G);
        playMenu.add(suggest);
        
        pause = new JMenuItem("error!");
        pause.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_PAUSE, 0));
        pause.setActionCommand("togglePause");
        playMenu.add(pause);
        
        swapAll = new JMenuItem("Swap All");
        swapAll.setMnemonic(KeyEvent.VK_W);
        playMenu.add(swapAll);

        pass = new JMenuItem("Pass");
        pass.setMnemonic(KeyEvent.VK_S);
        playMenu.add(pass);

        resign = new JMenuItem("Resign");
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
        boardSize.setMnemonic(KeyEvent.VK_B);
        viewMenu.add(boardSize);
        
        tileSize = new SizeMenu("Tile Size", TILE_SIZE_DEFAULT);
        tileSize.setMnemonic(KeyEvent.VK_T);
        viewMenu.add(tileSize);
        
        recenter = new JMenuItem("Re-center");
        recenter.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, 0));
        recenter.setMnemonic(KeyEvent.VK_R);
        viewMenu.add(recenter);
        
        attrs = new JMenuItem("Tile Attributes...");
        attrs.setMnemonic(KeyEvent.VK_A);
        viewMenu.add(attrs);
        
        hints = new JMenuItem("Hints...");
        hints.setMnemonic(KeyEvent.VK_H);
        viewMenu.add(hints);

        viewMenu.addSeparator();

        showClocks = new JCheckBoxMenuItem("Show Clocks");
        showClocks.setActionCommand("repaint");
        showClocks.setMnemonic(KeyEvent.VK_C);
        viewMenu.add(showClocks);
        
        showGrid = new JCheckBoxMenuItem("Show Grid");
        showGrid.setActionCommand("repaint");
        showGrid.setMnemonic(KeyEvent.VK_G);
        viewMenu.add(showGrid);
        
        showScores = new JCheckBoxMenuItem("Show Scores");
        showScores.setActionCommand("repaint");
        showScores.setMnemonic(KeyEvent.VK_O);
        viewMenu.add(showScores);
        
        peek = new JCheckBoxMenuItem("Peek");
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
                final GameStyle oldStyle = Game.getStyle();
                Game.closeCurrentInstance();
                clientArea.view.changeGame(oldStyle);
                break;
                
            case "Exit":
                System.exit(0);              

            case "Pass":
            case "Play":
            case "Resign":
                clientArea.view.playMenuCommand(command);
                break;
                
            case "Take Back":
                clientArea.view.takeBack();
                break;
   
            case "Suggest":
                startSuggest();
                break;                

            case "Swap All":
                clientArea.view.swapAll();
                break;

            case "Re-center":
                clientArea.view.recenter();
                break;

            case "togglePause":
                clientArea.view.togglePause();
                break;

            case "Hints...":
                clientArea.view.hintBox();
                break;
                
            case "Cancel":
                assert worker != null;
                worker.interrupt();
                break;

            case "repaint": 
                break;

            default:
                throw new AssertionError(command);
        }
        
        clientArea.repaint();
        update();
    }

    public void addListeners() {
        // "File" menu
        nu.addActionListener(this);     
        close.addActionListener(this);     
        exit.addActionListener(this);     

        // "Play" menu
        play.addActionListener(this);        
        takeBack.addActionListener(this);        
        suggest.addActionListener(this);        
        pause.addActionListener(this);        
        swapAll.addActionListener(this);        
        pass.addActionListener(this);        
        resign.addActionListener(this);

        // "View" menu
        boardSize.menuBar = this;
        tileSize.menuBar = this;
        recenter.addActionListener(this);
        hints.addActionListener(this);
        showClocks.addActionListener(this);        
        showGrid.addActionListener(this);        
        showScores.addActionListener(this);     
        peek.addActionListener(this);
        
        // "File" menu        
        cancel.addActionListener(this);        
    }
    
    public boolean areClocksVisible() {
        return showClocks.isSelected();
    }
    
    public boolean areScoresVisible() {
        return showScores.isSelected();
    }
    
    public void gameOver() {
        peek.setSelected(true);
        showClocks.setSelected(true);
        showScores.setSelected(true);
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

    public void loadUser(ReadUser user) {
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
    
    public void startAutoPlay() {
        assert worker == null : worker;
        
        worker = new SwingWorker() {
            @Override
            public Object construct() {
                return clientArea.view.autoPlay();
            }
            @Override
            public void finished() {
                worker = null;
                final ReadMove move = (ReadMove)get();
                clientArea.view.implement(move);
                clientArea.view.finishTurn(move);
                clientArea.repaint();
                update();
           }
       };
       worker.start();       
    }
    
    public void startSuggest() {
        assert worker == null : worker;
        
        worker = new SwingWorker() {
            @Override
            public Object construct() {
                return clientArea.view.suggest();
            }
            @Override
            public void finished() {
                final Partial suggestion = (Partial)get();
                if (suggestion != null) {
                    clientArea.view.copy(suggestion);
                    clientArea.repaint();
                }
                worker = null;
                update();
           }
       };
       worker.start();       
    }
    
    public void update() {
        boolean control = GoldTile.control == Display.GUI;
        boolean thinking = worker != null;
        
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
            haveGame = Game.haveInstance();
            if (haveGame) {
                local = Game.getInstance().getPlayable().getOpt().isLocalUser();
                
                final ReadGame game = Game.getInstance();
                paused = game.isPaused();
                over = game.isOver();
                canRedo = game.canRedo();
                canUndo = game.canUndo();
            }
            passFlag = partial.isPass();
                        
            final GameStyle style = Game.getStyle();
            suggestStyle = style.allowsHints(); 
            timedStyle = style.hasTimeLimit(); 
            undoStyle = style.allowsUndo();
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
        final boolean cgl = control && haveGame && local && !thinking;
        play.setEnabled(cgl && !over && !paused && !passFlag);
        takeBack.setEnabled(cgl && !over && !paused && !passFlag);
        suggest.setEnabled(cgl && !over && !paused && suggestStyle);
        pause.setEnabled(cgl && !over);
        swapAll.setEnabled(cgl && !over && !paused && passFlag);
        pass.setEnabled(cgl && !over && !paused && passFlag);
        resign.setEnabled(cgl && !over && !paused);
        restart.setEnabled(cgl && !paused && undoStyle);
        undo.setEnabled(cgl && !paused && undoStyle && canUndo);
        redo.setEnabled(cgl && !paused && undoStyle && canRedo);
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
        cancel.setEnabled(thinking);
        updateMenu(thinkingMenu);

        // "Help" menu
        updateMenu(helpMenu);
    }

    private static void updateMenu(JMenu menu) {
        assert menu != null;
        
        for (java.awt.Component component : menu.getMenuComponents()) {
            if (component.isEnabled()) {
                menu.setEnabled(true);
                return;
            }
        }
        
        menu.setEnabled(false);        
    }
}
