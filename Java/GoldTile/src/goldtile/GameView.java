// File:     GameView.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  GameView class for the Gold Tile Game
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
import java.awt.Point;
import java.util.Map;
import java.util.TreeMap;

public class GameView extends Partial {
    // constants
    final private static int PAD_PIXELS_DEFAULT = 6;
    
    //
    private Canvas canvas = null;
    private Cell target = null;
    private DisplayModes displayModes = new DisplayModes(Display.GUI);
    final private GamePanel panel;
    final private MenuBar menuBar;
    private int padPixels = PAD_PIXELS_DEFAULT;
    private int recenterLeftX = 0;
    private int recenterRightX = 0;
    private static Map<Integer,Rect> tileFinder = new TreeMap<>();
    private Point start = new Point();
    private Rect handRect = new Rect(0,0,0,0);
    private Rect swapRect = new Rect(0,0,0,0);
    private Tiles warmTiles = new Tiles(); // tiles played on the previous turn
    
    // constructors
    
    public GameView(GamePanel panel, MenuBar menuBar) {
        super(null, Hint.getDefault(null), HandOpt.SKIP_PROBABILITY_DEFAULT);
        assert panel != null;
        assert menuBar != null;
        
        this.menuBar = menuBar;
        this.panel = panel;
    }

    // methods
    
    public Cell getCell(Point point) {
        final Area gridUnit = getGridUnitArea();

        final int unitX = gridUnit.width;
        final int dx = point.x - start.x + unitX/2;
        int column;
        if (dx >= 0) {
            column = dx / unitX;
        } else {
            final int absNum = unitX - dx - 1;
            assert absNum > 1 : absNum;
            column = -(absNum / unitX);  
        }
        
        final int unitY = gridUnit.height;
        final int dy = point.y - start.y + unitY/2;
        int row;
        if (dy >= 0) {
            row = -(dy / unitY);
        } else {
            final int absNum = unitY - dy - 1;
            assert absNum > 1 : absNum;
            row = absNum / unitY;  
        }

        return new Cell(row, column);
    }

    private Area getCellArea(Place place) {
        Area result = getTileArea(place);
        
        if (menuBar.isGridVisible()) {
            // add room for two grid lines
            result = result.expand(2);
        }
        
        return result;
    }
    
    private Point getCenter(Cell cell) {
        final Area unitArea = getGridUnitArea();
        
        final int column = cell.getColumn();
        final int unitX = unitArea.width;
        final int x = start.x + unitX*column;    
        
        final int row = cell.getRow();
        final int unitY = unitArea.height;
        final int y = start.y - unitY*row;
        
        return new Point(x, y);
    }
    
    private Area getGridUnitArea() {
        final Area cell = getCellArea(Place.BOARD);
        final int cellHeight = cell.height;
        final int cellWidth = cell.width;

        int height;
        int width;
        switch (Cell.getShape()) {
            case HEXAGON:
                height = cellHeight/2;
                width = (int)(0.5 + 0.75*cellWidth);
                break;
            case SQUARE:
                height = cellHeight;
                width = cellWidth;
                break;
            case TRIANGLE:
                height = cellHeight;
                width = cellWidth/2;
                break;
            default:
                throw new AssertionError(Cell.getShape());
        }

        Area result = new Area(height, width);
        if (menuBar.isGridVisible()) {
            result = result.expand(-1);
        }

        return result;
    }
    
    private Area getTileArea(Place place) {
        final int width = getTileWidth(place);
        
        int height = 0;
        switch(Cell.getShape()) {
            case HEXAGON:
            case TRIANGLE:
                height = (int)((1 + Global.SQRT_3*width)/2);
                break;
            case SQUARE:
                height = width;
                break;
            default:
                throw new AssertionError(Cell.getShape());
        }
        
        return new Area(width, height);
    }
    
    private Area getTileArea(Point point) {
        if (handRect.contains(point) || swapRect.contains(point)) {
            return getTileArea(Place.HAND);
        } else {
            return getTileArea(Place.BOARD);
        }
    }
    
    private int getTileWidth(Place place) {
        final int size = menuBar.getTileSize(place);
        final Shape shape = Cell.getShape();
        final int result = shape.tinyWidth * (size + 1);
        
        assert Global.isEven(result) : result;
        assert result > 0 : result;
        
        return result;
    }
    
    private boolean isInCellArea(Point point, Cell cell) {
        final Point center = getCenter(cell);
        final Area area = getCellArea(Place.BOARD);
        final boolean oddFlag = cell.isOdd();
        final Rect interior 
                = Canvas.interiorGridShape(center, area, oddFlag);

        return interior.contains(point);
    }
    
    public boolean isWarmTile(Tile tile) {
        return warmTiles.contains(tile); 
    }

    public void paintAll(Canvas c) {
        canvas = c;
        if (game == null) {
            // TODO paintIdle();
        } else if (game.isPaused()) {
            paintPaused();
        } else {
            final Area clientArea = panel.getClientArea();
            recenterLeftX = 0;
            recenterRightX = clientArea.width;
            tileFinder.clear();

            paintBoard(Layer.VALID);
            paintBoard(Layer.HINTED_OR_EMPTY);
            // TODO paintUnplayableHands();
            // TODO paintPlayableHand();
            // TODO paintPlayableTiles();
            paintBoard(Layer.ACTIVE_FROM_BOARD);
        }
        canvas = null;
    }
    
    public void paintBoard(Layer showLayer) {
        final Grid grid = Cell.getGrid();
        final int extraColumns = grid.columnFringe - 1;
        final int extraRows = grid.rowFringe - 1;
        
        // Determine which cells are visible in the client area.
        final Point ulc = new Point(0, 0);
        final Cell ulcCell = getCell(ulc);
        int topRow = ulcCell.getRow() + extraRows;
        int leftColumn = ulcCell.getColumn() + extraColumns;
        
        final Point brc = panel.getBrc();
        final Cell brcCell = getCell(brc);
        int bottomRow = brcCell.getRow() - extraRows;
        int rightColumn = brcCell.getColumn() - extraColumns;
        
        if (!Cell.doesBoardWrap()) {
            assert bottomRow <= topRow;
            assert leftColumn <= rightColumn;

            // Get the range of "might use" cells.
            final int bottomUseRow = board.getBottomUseRow();
            final int leftUseColumn = board.getLeftUseColumn();
            final int rightUseColumn = board.getRightUseColumn();
            final int topUseRow = board.getTopUseRow();

            assert bottomUseRow <= topUseRow;
            assert leftUseColumn <= rightUseColumn;

            // Reduce the visible range to only include "might use" cells.
            if (bottomUseRow > bottomRow) {
                bottomRow = bottomUseRow;
            }
            if (leftUseColumn > leftColumn) {
                leftColumn = leftUseColumn;
            }
            if (rightUseColumn < rightColumn) {
                rightColumn = rightUseColumn;
            }
            if (topUseRow < topRow) {
                topRow = topUseRow;
            }
        }
        
        int swapCnt = countSwap();
        Tile activeTile = getActiveTile();
        if (isInSwap(activeTile)) {
            assert swapCnt > 0;
            --swapCnt;
        }

        for (int row = topRow; row >= bottomRow; row--) {
            paintRow(showLayer, leftColumn, rightColumn, swapCnt, row);
        }
    }

    private void paintCell(Cell cell, Point center, int swapCnt) {
        assert cell.isValid();
        
        Color cellColor = Color.BLACK;
        Color featureColor = Color.LIGHT_GRAY;
        if (!isEmpty(cell)) {
            cellColor = Color.DARK_BLUE;
            featureColor = Color.LIGHT_BLUE;
        } else if (isHinted(cell)) {
            if (swapCnt == 0) {
                cellColor = Color.DARK_GREEN;
                featureColor = Color.LIGHT_GREEN;
            } else {
                cellColor = Color.BROWN;
                featureColor = Color.YELLOW;
            }
        }
        
        Color gridColor = cellColor;
        if (menuBar.isGridVisible()) {
           gridColor = featureColor;
        }
        
        final Area area = getCellArea(Place.BOARD);
        final boolean oddFlag = cell.isOdd();        
        final Rect bounds = canvas.drawCell(center, area, 
                cellColor, gridColor, oddFlag);
        
        // Draw cell features.
        if (target != null && cell.equals(target)) {
            // It's the target cell.
            final Color targetColor = Color.MEDIUM_GRAY;
            canvas.backgroundColor = targetColor;
            canvas.foregroundColor = targetColor;
            canvas.drawTargetArrow(bounds);
        }
        if (cell.isStart()) {
            // It's the start cell.
            canvas.foregroundColor = featureColor;
            canvas.drawTextLine(bounds, "START", "S");
        }
     
        /*  
         * If the active tile came from the board, draw it later 
         * (not now) so it won't get obscured.
         */
        final Tile tile = getCellTile(cell);
        if (tile != null && !isActive(tile)) {
            // inactive tile -- draw it now
            paintTile(center, tile, oddFlag);
        }
    }

    private void paintRow(Layer showLayer, int leftSeeColumn, 
            int rightSeeColumn, int swapCnt, int row)
    {
        for (int column = leftSeeColumn; column <= rightSeeColumn; column++) {
            final Cell cell = new Cell(row, column);
            Cell wrapCell = cell.wrap();
                
            if ((board.mightUse(cell) || board.mightUse(wrapCell)) 
                    && wrapCell.isValid())
            {
                final Point center = getCenter(cell);
                paintCell(cell, center, swapCnt);                   
            }
        }       
    }

    private void paintPaused() {
        assert game != null;
        assert game.isPaused();
        assert !game.isOver();
        
        canvas.backgroundColor = Color.BLACK;
        canvas.foregroundColor = Color.GREEN;
        final Rect clientRect = panel.getClientRect();
        canvas.drawTextMultiline(clientRect, 
                "The game is paused.\n\nClick here to resume.");

        final int topY = padPixels;
        final int leftX = padPixels;
        // TODO
    }
    
    void paintTile(Point center, Tile tile, boolean oddFlag) {
        final int markingCnt = displayModes.getMarkingCount();
        assert markingCnt <= Markings.MARKING_COUNT_MAX;
        
        // Determine the tile's background color.
        Color tileColor = Color.LIGHT_GRAY;
        if (tile.hasBonus()) {
            tileColor = Color.DULL_GOLD;
        }
        if (contains(tile)) {
            tileColor = Color.WHITE;
            if (tile.hasBonus()) {
                tileColor = Color.GOLD;
            }
        }

        final Markings markings = new Markings(tile, displayModes);
        final Area tileArea = getTileArea(center);
        final boolean warmFlag = isWarmTile(tile);
        canvas.drawTile(markings, tileColor, center, tileArea, 
                warmFlag, oddFlag);
    }
    
    public void recenter() {
        final Area clientArea = panel.getClientArea();
        
        if (recenterLeftX >= recenterRightX) {
            recenterLeftX = 0;
            recenterRightX = clientArea.width;
        }
        int x = (recenterLeftX + recenterRightX)/2;
        int y = clientArea.height/2;
        
        final Area gridUnitArea = getGridUnitArea();

        x -= gridUnitArea.width * (board.getEastMax() - board.getWestMax())/2;
        y += gridUnitArea.height * (board.getNorthMax() - board.getSouthMax())/2;

        start = new Point(x, y);
    }
    
    public void scroll(int dx, int dy) {
        start.x += dx;
        start.y += dy;
    }
    
    final public void setGame(Game g) {
        game = g;
        
        reset(game, Hint.getDefault(game), HandOpt.SKIP_PROBABILITY_DEFAULT);
        recenter();
        target = null;
        displayModes.cleanup();
    }
    
    public void toggleTargetCell(Point point) {
        final Cell rawCell = getCell(point);
        final Cell wrapCell = rawCell.wrap();
        
        if (wrapCell.isValid() && isInCellArea(point, rawCell)) {
            if (target != null && wrapCell.equals(target)) {
                target = null;
            } else if (board.mightUse(wrapCell) && isEmpty(wrapCell)) {
                // new target cell
                target = wrapCell;
            }
        }
    }
}
