// File:     GamePanel.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  GamePanel class for the Gold Tile Game
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

import java.awt.Cursor;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.JOptionPane;

public class GamePanel 
    extends javax.swing.JPanel
{ 
    // constants, sorted by type
    final private static Cursor DRAG_CURSOR 
            = Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR);
    final private static int DRAG_THRESHOLD = 6;
    
    // links
    final public GameFrame frame;
    final public GameView view;
    final private MenuBar menuBar;
    
    // per-instance fields, sorted by type
    private boolean deactivateOnRelease = false;
    private boolean dragBoard = false;
    private boolean leftButtonDown = false;
    private int dragBoardPixelCount = 0;
    private Point mouseLast = null;
    
    // constructors
    
    public GamePanel(MenuBar menuBar, GameFrame frame) {
        assert menuBar != null;
        assert frame != null;
        
        this.menuBar = menuBar;
        this.frame = frame;
        view = new GameView(this, menuBar);
        
        setBackground(Color.BLACK);
       
        // add event listeners
        
        addComponentListener(new java.awt.event.ComponentAdapter() {
            @Override
            public void componentResized(java.awt.event.ComponentEvent event) {
                repaint();
            }
        });
        
        addKeyListener(new java.awt.event.KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent event) {
                keyPress(event.getKeyCode());
            }
        });
        
        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseEntered(MouseEvent event) {
                requestFocusInWindow(); // so KeyListener will receive events
            }
            @Override
            public void mousePressed(MouseEvent event) {
                if (event.getButton() == MouseEvent.BUTTON1) {
                    leftButtonPress(event.getPoint());
                }
            }
            @Override
            public void mouseReleased(MouseEvent event) {
                if (event.getButton() == MouseEvent.BUTTON1) {
                    leftButtonRelease(event.getPoint());
                }
            }
        });
        
        addMouseMotionListener(new MouseAdapter() {
            @Override
            public void mouseDragged(MouseEvent event) {
                mouseDrag(event.getPoint());
            }
            @Override
            public void mouseMoved(MouseEvent event) {
                mouseMove(event.getPoint());
            }
        });
    }
    
    // methods, sorted by name
    
    public Point getBottomRightCorner() {
        final int x = getWidth() - 1;
        final int y = getHeight() - 1;
        
        return new Point(x, y);
    }
    
    public Area getClientArea() {
        return new Area(getWidth(), getHeight());
    }

    public Rect getClientRect() {
        return new Rect(0, 0, getWidth(), getHeight());
    }

    public Point getMouseLast() {
        return new Point(mouseLast);
    }

    private void handleClickView(Point point) {
        assert point != null;
        assert Game.haveInstance();
        assert !Game.getInstance().isPaused();
        assert !dragBoard;

        if (view.hasActiveTile()) {
            // Continue moving the active tile.
            assert mouseLast != null;
            assert deactivateOnRelease;
            repaint();
            return;
        }
        
        final Tile tile = view.findPlayableTile(point);
        if (tile != null) {
            /*
             * The local user clicked on a playable tile.
             * Activate the tile and start dragging it around.
             */
            view.activate(tile);
            deactivateOnRelease = false;
            repaint();
            
        } else if (!view.isInHandRect(point) && !view.isInSwapRect(point)) {
            // Start dragging the board around.
            dragBoard = true;
            view.deactivate();
            dragBoardPixelCount = 0;
            setCursor(DRAG_CURSOR);
            repaint();
        }
    }
    
    private void handleDragView(Point point) {
        assert point != null;
        assert Game.haveInstance();
        assert !Game.getInstance().isPaused();
        
        if (view.hasActiveTile()) {
            assert !dragBoard;
            repaint();

        } else if (dragBoard) {
            final int dx = point.x - mouseLast.x;
            final int dy = point.y - mouseLast.y;
            view.translate(dx, dy);
            dragBoardPixelCount += Math.abs(dx) + Math.abs(dy);
            repaint();
        }
    }
    
    private void handleReleaseView(Point point) {
        assert point != null;
        assert mouseLast != null;
        assert Game.haveInstance();
        assert !Game.getInstance().isPaused();
        
        final int dx = point.x - mouseLast.x;
        final int dy = point.y - mouseLast.y;
        if (view.hasActiveTile()) {
            assert !dragBoard;
            deactivateOnRelease = view.dropActiveTile(point, 
                    deactivateOnRelease);
            repaint();

        } else if (dragBoard) {
            view.translate(dx, dy);
            dragBoardPixelCount += Math.abs(dx) + Math.abs(dy);
  
            if (dragBoardPixelCount < DRAG_THRESHOLD) {
                /*
                 * Board drags shorter than six pixels (clicks, basically)
                 * also serve to alter or un-target the target cell.
                 */
                view.toggleTargetCell(point);
            }
            
            // done dragging the board
            dragBoard = false;
            setCursor(Cursor.getDefaultCursor());
            repaint();
        }
    }
    
    private void keyPress(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_DOWN:
                view.moveTarget(Direction.SOUTH);
                break;
            case KeyEvent.VK_LEFT:
                view.moveTarget(Direction.WEST);
                break;
            case KeyEvent.VK_RIGHT:
                view.moveTarget(Direction.EAST);
                break;
            case KeyEvent.VK_UP:
                view.moveTarget(Direction.NORTH);
                break;
        }
    }
    
    private void leftButtonPress(Point point) {
        assert point != null;
        
        final ReadGame game = Game.getInstance();
        if (game != null && !game.isPaused()) {
            handleClickView(point);
        }
        leftButtonDown = true;
        mouseLast = point;
    }

    private void leftButtonRelease(Point point) {
        assert point != null;
        assert mouseLast != null;
        
        final Game game = Game.getInstance();
        if (game != null) {
            if (game.isPaused()) {
                view.startClock();
            } else {
                handleReleaseView(point);
            }
        }
        leftButtonDown = false;
        mouseLast = point;
    }
    
    private void mouseDrag(Point point) {
        assert point != null;
        
        if (leftButtonDown) {
            assert mouseLast != null;

            final ReadGame game = Game.getInstance();
            if (game != null && !game.isPaused()) {
                handleDragView(point);            
            }
            mouseLast = point;
        }
    }
    
    private void mouseMove(Point point) {
        assert point != null;
        assert !dragBoard;
        assert !leftButtonDown;
        
        if (view.hasActiveTile()) {
            assert mouseLast != null;
            handleDragView(point);    
        }
        mouseLast = point;
    }
    
    public void offerNewGame() {
        GameOpt gameOpt;
        if (Game.haveInstance()) {
            // Copy the game options of the active instance.
            gameOpt = new GameOpt(Game.getInstance().getOpt());
            gameOpt.setRules(Rules.REPLAY);
        } else {
            // Start with the default game options.
            gameOpt = new GameOpt();
        }

        final int maxHands = ParmBox3.HANDS_DEALT_MAX;
        final HandOpt handOpts[] = new HandOpt[maxHands];
        for (int iHand = 0; iHand < maxHands; iHand++) {
            HandOpt handOpt;
            if (Game.haveInstance() && iHand < gameOpt.getHandsDealt()) {
                ReadHand hand = Game.getInstance().getHand(iHand);
                handOpt = new HandOpt(hand.getOpt());
            } else {
                handOpt = new HandOpt("User");
            }
            handOpts[iHand] = handOpt;
        }
                
        final Wizard wizard = new Wizard(frame);
        
        final ParmBox1 parmBox1 = new ParmBox1(wizard);
        wizard.addCard(parmBox1);

        final WizardCard parmBox2 = new ParmBox2(wizard);
        wizard.addCard(parmBox2);
        
        final WizardCard parmBox3 = new ParmBox3(wizard);
        wizard.addCard(parmBox3);

        final WizardCard handBox = new HandBox(wizard);
        wizard.addCard(handBox);

        // pack and go
        final boolean completed = wizard.run(parmBox1, gameOpt, handOpts);
        
        if (!completed) {
            Console.print("New game aborted.\n");
            return;
        }
        
        final GameStyle oldStyle = Game.getStyle();
        new Game(gameOpt, handOpts);
        view.changeGame(oldStyle);
    }
    
    @Override
    public void paintComponent(java.awt.Graphics context) {
        super.paintComponent(context);
        
        final Canvas canvas = new Canvas(context);
        view.paintAll(canvas);
    }
    
    public void recenter() {
        view.recenter();
        repaint();
    }
    
    public void showInformationBox(String message, String title) {
        assert message != null;
        assert title != null;
        
        JOptionPane.showMessageDialog(this, message, 
                title + " - Gold Tile Game",
                JOptionPane.INFORMATION_MESSAGE);
    }

    public void showRuleBox(UserMessage userMessage) {
        assert userMessage != null;
        
        JOptionPane.showMessageDialog(this, 
                userMessage.message, 
                userMessage.title + " - Gold Tile Game", 
                JOptionPane.ERROR_MESSAGE);
    }
}
