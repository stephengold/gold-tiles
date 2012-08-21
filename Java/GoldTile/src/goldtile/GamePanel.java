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
    final public GameView view;
    final private MenuBar menuBar;
    
    // per-instance fields, sorted by type
    private boolean dragBoardFlag = false;
    private int dragBoardPixelCnt = 0;
    private int dragTileDeltaX = 0;
    private int dragTileDeltaY = 0;
    private int mouseUpCount = 0;
    private Point mouseLast = null; // null means LMB is not pressed
    
    // constructors
    
    public GamePanel(MenuBar menuBar) {
        this.menuBar = menuBar;
        view = new GameView(this, menuBar);
        
        setBackground(Color.BLACK);
        //setPreferredSize(area);
       
        addComponentListener(new java.awt.event.ComponentAdapter() {
            @Override
            public void componentResized(java.awt.event.ComponentEvent event) {
                repaint();
            }
        });
        
        addMouseListener(new MouseAdapter() {
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

    private void handleClickView(Point point) {
        assert point != null;
        assert view.hasGame();
        assert !view.getGame().isPaused();
        assert !dragBoardFlag;
        
        final Tile tile = view.findPlayableTile(point);
        if (tile != null) {
            // Activate the tile and start dragging it around.
            view.activate(tile);
            dragTileDeltaX = 0;
            dragTileDeltaY = 0;
            mouseUpCount = 0;
            repaint();
            
        } else if (!view.isInHandRect(point) && !view.isInSwapRect(point)) {
            // Start dragging the board around.
            dragBoardFlag = true;
            view.deactivate();
            dragBoardPixelCnt = 0;
            setCursor(DRAG_CURSOR);
            repaint();
        }
    }
    
    private void handleDragView(Point point) {
        assert point != null;
        assert view.hasGame();
        assert !view.getGame().isPaused();
        
        final int dx = point.x - mouseLast.x;
        final int dy = point.y - mouseLast.y;
        if (view.hasActiveTile()) {
            assert !dragBoardFlag;
            dragTileDeltaX += dx;
            dragTileDeltaY += dy;
            repaint();

        } else if (dragBoardFlag) {
            view.translate(dx, dy);
            dragBoardPixelCnt += Math.abs(dx) + Math.abs(dy);
            repaint();
        }
    }
    
    private void handleReleaseView(Point point) {
        assert point != null;
        assert mouseLast != null;
        assert view.hasGame();
        assert !view.getGame().isPaused();
        
        final int dx = point.x - mouseLast.x;
        final int dy = point.y - mouseLast.y;
        if (view.hasActiveTile()) {
            assert !dragBoardFlag;
            if (view.dropActiveTile(point)) {
                if (mouseUpCount == 0) {
                    // TODO
                }
            }
            repaint();

        } else if (dragBoardFlag) {
            view.translate(dx, dy);
            dragBoardPixelCnt += Math.abs(dx) + Math.abs(dy);
  
            if (dragBoardPixelCnt < DRAG_THRESHOLD) {
                /*
                 * Drags shorter than six pixels (clicks, basically)
                 * also serve to alter or un-target the target cell.
                 */
                view.toggleTargetCell(point);
            }
            dragBoardFlag = false;
            setCursor(Cursor.getDefaultCursor());
            repaint();
        }
    }
    
    private void leftButtonPress(Point point) {
        assert mouseLast == null : mouseLast;
        
        final ReadGame game = view.getGame();
        if (game != null && !game.isPaused()) {
            handleClickView(point);
        }
        mouseLast = point;
    }

    private void leftButtonRelease(Point point) {
        assert mouseLast != null;
        
        final ReadGame game = view.getGame();
        if (game != null) {
            if (game.isPaused()) {
                view.startClock();
                menuBar.update();
                repaint();
            } else {
                handleReleaseView(point);
            }
        }
        mouseLast = null;
    }
    
    private void mouseDrag(Point point) {
        if (mouseLast != null) {

            final ReadGame game = view.getGame();
            if (game != null && !game.isPaused()) {
                handleDragView(point);            
            }
            mouseLast = point;
        }
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
    
    public void ruleBox(UserMessage userMessage) {
        assert userMessage != null;
        
        JOptionPane.showMessageDialog(this, 
                userMessage.message, 
                userMessage.title + " - Gold Tile Game", 
                JOptionPane.ERROR_MESSAGE);
    }
    
    public void translateTile(Point center) {
        center.translate(dragTileDeltaX, dragTileDeltaY);
    }
}
