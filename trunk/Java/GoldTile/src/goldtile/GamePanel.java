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

public class GamePanel extends javax.swing.JPanel {
    final public GameView view;
    private int dragBoardPixelCnt = 0;
    private Point mouseLast = null;
    
    // constants
    final private static Cursor DRAG_CURSOR 
            = Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR);
    final private static int DRAG_THRESHOLD = 6;
   
    // constructor
    
    GamePanel(MenuBar menuBar) {
        view = new GameView(this, menuBar);
        
        setBackground(Color.BLACK);
        setPreferredSize(new Area(640, 480)); // TODO
       
        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent event) {
                handleButtonDown(event.getPoint());
            }
            @Override
            public void mouseReleased(MouseEvent event) {
                handleButtonUp(event.getPoint());
            }
        });
        
        addMouseMotionListener(new MouseAdapter() {
            @Override
            public void mouseDragged(MouseEvent event) {
                handleMouseMove(event.getPoint());
            }
        });
    }
    
    // methods
    
    public Point getBrc() {
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

    private void handleButtonDown(Point point) {
        assert mouseLast == null : mouseLast;
        
        dragBoardPixelCnt = 0;
        mouseLast = point;
        setCursor(DRAG_CURSOR);
        repaint();
    }
    
    private void handleButtonUp(Point point) {
        assert mouseLast != null;
        
        final int dx = point.x - mouseLast.x;
        final int dy = point.y - mouseLast.y;
        view.scroll(dx, dy);
        dragBoardPixelCnt += Math.abs(dx) + Math.abs(dy);
  
        if (dragBoardPixelCnt < DRAG_THRESHOLD) {
            /*
             Drags shorter than six pixels 
             are treated as normal mouse-clicks 
             which change or deactivate the target cell.
             */
            view.toggleTargetCell(point);
        }
        
        mouseLast = null;
        setCursor(Cursor.getDefaultCursor());
        repaint();
    }
    
    private void handleMouseMove(Point point) {
        assert mouseLast != null;
        
        final int dx = point.x - mouseLast.x;
        final int dy = point.y - mouseLast.y;
        view.scroll(dx, dy);
        dragBoardPixelCnt += Math.abs(dx) + Math.abs(dy);
        
        mouseLast = point;
        repaint();
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
}
