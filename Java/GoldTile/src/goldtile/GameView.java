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

public class GameView
    extends Partial
{
    // enums, sorted by name
    public static enum Layer { VALID, HINTED_OR_EMPTY; }
    public static enum Side { LEFT, RIGHT; }

    // constants
    final private static int PAD_PIXELS_DEFAULT = 6;

    // links
    final private GamePanel panel;
    final private MenuBar menuBar;

    // per-instance fields, sorted by type
    private Canvas canvas = null;
    private Cell target = null;
    private DisplayModes displayModes = new DisplayModes(Display.GUI);
    private int padPixels = PAD_PIXELS_DEFAULT;
    private int recenterLeftX = 0;
    private int recenterRightX = 0;
    private Point startCellPosition = new Point();
    private Rect handRect = new Rect(0,0,0,0);
    private Rect swapRect = new Rect(0,0,0,0);
    private Tiles warmTiles = new Tiles(); // tiles played on the previous turn

    // static fields, sorted by type
    private static GameView currentInstance = null;
    final private static java.util.Map<Integer,Rect> tileFinder =
            new java.util.TreeMap<>();

    // constructors

    public GameView(GamePanel panel, MenuBar menuBar) {
        super();
        assert panel != null;
        assert menuBar != null;

        currentInstance = this;

        // Initialize links to related objects.
        this.menuBar = menuBar;
        this.panel = panel;
    }

    // methods, sorted by name

    private void changeHand(String previousPlayerName) {
        assert previousPlayerName != null;
        assert Game.hasInstance();

        final ReadGame game = Game.getInstance();
        final ReadHandOpt handOpt = game.getPlayable().getOpt();
        final String playerName = handOpt.getPlayerName();

        if (handOpt.isLocalUser()) {
            final ReadUser user = User.lookup(playerName);
            loadUser(user);
        }
        target = null;
        if (!game.isOver()) {
            if (!handOpt.isLocalUser()) {
                startClock();
            } else if (playerName.equals(previousPlayerName)) {
                startClock();
            } else if (!menuBar.isAutoPauseEnabled()) {
                startClock();
            }

            if (handOpt.isAutomatic()) {
                menuBar.startAutoPlay();
            }
        }
    }

    public boolean dropActiveTile(Point mouse, boolean deactivateOnRelease) {
        assert mouse != null;
        assert Game.hasInstance();

        final ReadBoard board = getBoard();
        final Tile tile = getActiveTile();

        // Determine where the tile came from.
        final Place from = findActiveTile();
        Cell fromCell;
        if (from != Place.BOARD) {
            fromCell = null;
        } else {
            fromCell = board.find(tile);
        }

        // Determine where the tile was released.
        Place to = findPlace(mouse);
        Cell toCell;
        if (to != Place.BOARD) {
            toCell = null;
        } else {
            toCell = getCell(mouse);
            if (!isInCellInterior(mouse, toCell)) {
                // ambiguous release point
                return false;
             }
             if (getGameOpt().doesBoardWrap()) {
                 toCell = toCell.wrap();
             }
        }

        if (from == to && (fromCell == toCell || fromCell.equals(toCell))) {
            /*
             * Trivial drags (which don't actually move the tile)
             * are treated like mouse-clicks.
             */
            if (deactivateOnRelease) {
                // Stop dragging the active tile.
                deactivate();
                return false;
            } else {
                if (target != null && from != Place.BOARD) {
                    // Play the active tile to the target cell.
                    to = Place.BOARD;
                    toCell = target;
                } else {
                    // Continue dragging the active tile.
                    target = null;
                    return true;
                }
            }
        }

        // Try performing the move.

        if (to == Place.BOARD &&
               (!toCell.isValid() || !board.isEmpty(toCell)) )
        {
            /*
             * The destination cell is invalid or already used, so we
             * can't perform the move in the usual manner.
             */
            panel.showRuleBox(UserMessage.EMPTY_CELL);
            deactivate();
            return false;
        }

        moveActiveTile(from, to, toCell);

        // Check whether the new partial move is legal.
        final ReadMove moveSoFar = getMove(Active.INCLUDED);
        UserMessage reason = Game.getInstance().checkMove(moveSoFar);

        if (reason != null &&
                (to == Place.SWAP_AREA || reason != UserMessage.FIRST_TURN))
        {
            // It's illegal, even as a partial turn:  reverse the move.
            moveActiveTile(to, from, fromCell);

            if (reason == UserMessage.START && from != Place.BOARD) {
                reason = UserMessage.START_SIMPLE;
            }

            // Explain to the user why it was illegal.
            panel.showRuleBox(reason);
        }

        target = null;
        deactivate();
        menuBar.update();
        panel.repaint();

        return false;
    }

    private Place findPlace(Point point) {
        assert point != null;

        if (handRect.contains(point)) {
            return Place.HAND;
        } else if (swapRect.contains(point)) {
            assert !handRect.contains(point);
            return Place.SWAP_AREA;
        } else {
            return Place.BOARD;
        }
    }

    public Tile findPlayableTile(Point point) {
        assert point != null;

        for (Integer tileId : tileFinder.keySet()) {
            final Rect rect = tileFinder.get(tileId);
            if (rect.contains(point)) {
                final Tile tile = new Tile(tileId);
                // Make sure the tile is visible.
                if (isActive(tile)) {
                    return tile;
                } else if (isInSwapRect(point) && !isInSwap(tile)) {
                    continue;
                } else if (isInHandRect(point) && !isInHand(tile)) {
                    continue;
                } else {
                    return tile;
                }
            }
        }

        return null;
    }

    public void finishTurn(ReadMove move) {
        assert move != null;
        assert Game.hasInstance();

        Game.getInstance().finishTurn(move);

        if (Game.getInstance().isOver()) {
            gameOver();

        } else {
            // The game isn't over yet, so proceed to the next hand.
            final String oldPlayerName = saveHand();
            nextHand();
            changeHand(oldPlayerName);
        }
    }

    private String formatClock(int iHand) {
        assert Game.hasInstance();
        assert iHand >= 0;

        int seconds = Game.getInstance().getSeconds(iHand);

        // convert to minutes and seconds
        String sign;
        if (seconds >= 0) {
            sign = "";
        } else {
            sign = "-";
            seconds = -seconds;
        }

        final int minutes = seconds / Global.SECONDS_PER_MINUTE;
        seconds -= minutes * Global.SECONDS_PER_MINUTE;
        final int tensOfSeconds = seconds / 10;
        seconds -= tensOfSeconds*10;

        assert seconds >= 0;
        assert seconds <= 9;
        assert tensOfSeconds >= 0;
        assert tensOfSeconds <= 5;

        return String.format("%s%d:%d%d",
                sign,
                minutes,
                tensOfSeconds,
                seconds);
    }

    private String formatScore(ReadHand hand, boolean playableFlag) {
        assert hand != null;
        assert Game.hasInstance();

        final int score = hand.getScore();

        int pointCountThisTurn = 0;
        if (playableFlag) {
            pointCountThisTurn = getPointCount();
        }

        if (pointCountThisTurn == 0) {
            return StringExt.plural(score, "point");
        } else {
            return String.format("%d+%s",
                    score,
                    StringExt.plural(pointCountThisTurn, "point"));
        }
    }

    private void gameOver() {
        assert Game.hasInstance();
        assert Game.getInstance().isOver();

        menuBar.gameOver();
        final String endBonus = Game.getInstance().reportEndBonus();
        panel.showInformationBox(endBonus, "Game Over");
        takeBack();
    }


    public Cell getCell(Point point) {
        assert point != null;

        final Area gridUnit = getGridUnitArea();

        final int unitX = gridUnit.width;
        final int dx = point.x - startCellPosition.x + unitX/2;
        int column;
        if (dx >= 0) {
            column = dx / unitX;
        } else {
            final int absNum = unitX - dx - 1;
            assert absNum > 1 : absNum;
            column = -(absNum / unitX);
        }

        final int unitY = gridUnit.height;
        final int dy = point.y - startCellPosition.y + unitY/2;
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

    /**
     * Compute the width and height of cells in the specified Place.
     * @param place a region of the game view (never null)
     * @return a new Area object (never null)
     */
    private Area getCellArea(Place place) {
        assert place != null;

        Area result = getTileArea(place);

        if (menuBar.isGridVisible()) {
            // Add room for two grid lines.
            result = result.expand(2);
        }

        return result;
    }

    private Point getCenter(Cell cell) {
        assert cell != null;

        final Area unitArea = getGridUnitArea();

        final int column = cell.getColumn();
        final int unitX = unitArea.width;
        final int x = startCellPosition.x + column * unitX;

        final int row = cell.getRow();
        final int unitY = unitArea.height;
        final int y = startCellPosition.y - row * unitY;

        return new Point(x, y);
    }

    public Point getCenter(Tile tile) {
        assert tile != null;

        final Integer tileId = tile.getId();
        final Rect rect = tileFinder.get(tileId);

        return rect.getCenter();
    }

    public DisplayModes getDisplayModes() {
        return displayModes;
    }

    private ReadGameOpt getGameOpt() {
        return Game.getInstance().getOpt();
    }

    private Area getGridUnitArea() {
        final Area cell = getCellArea(Place.BOARD);
        final int cellHeight = cell.height;
        final int cellWidth = cell.width;

        int height;
        int width;
        switch (getGameOpt().getGrid().getCellShape()) {
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
                throw new AssertionError(getGameOpt());
        }

        Area result = new Area(width, height);
        if (menuBar.isGridVisible()) {
            result = result.expand(-1);
        }

        return result;
    }

    public static GameView getInstance() {
        return currentInstance;
    }

    public Point getStartCellPosition() {
        return startCellPosition;
    }

    private Area getTileArea(Place place) {
        assert place != null;

        final int width = getTileWidth(place);

        int height = 0;
        switch(getGameOpt().getGrid().getCellShape()) {
            case HEXAGON:
            case TRIANGLE:
                height = (int)((1 + Global.SQRT_3*width)/2);
                break;
            case SQUARE:
                height = width;
                break;
            default:
                throw new AssertionError(getGameOpt());
        }

        return new Area(width, height);
    }

    private int getTileWidth(Place place) {
        assert place != null;

        final int size = menuBar.getTileSize(place);
        final Shape shape = getGameOpt().getGrid().getCellShape();
        final int result = shape.tinyWidth * (size + 1);

        assert Global.isEven(result) : result;
        assert result > 0 : result;

        return result;
    }

    public static boolean hasInstance() {
        return currentInstance != null;
    }

    private boolean isInCellInterior(Point point, Cell cell) {
        assert point != null;
        assert cell != null;

        final Point center = getCenter(cell);
        final Area area = getCellArea(Place.BOARD);
        final boolean oddFlag = cell.isOdd();
        final Rect interior
                = Canvas.interiorGridShape(center, area, oddFlag);

        return interior.contains(point);
    }

    /**
     * Implement a move that didn't come from a local user.
     * @param move indicates the move (non-null)
     */
    public void implement(ReadMove move) {
        assert move != null;
        assert Game.hasInstance();

        move.implement(this);
        panel.repaint();

        final String handName = Game.getInstance().getPlayable().getName();
        final String description = String.format("%s %s.\n",
                handName, move.describe());
        if (!move.doesPlace()) {
            panel.showInformationBox(description, "Move Notification");
        }
        Console.print(description);
    }

    public boolean isInHandRect(Point point) {
        assert point != null;

        return handRect.contains(point);
    }

    public boolean isInSwapRect(Point point) {
        assert point != null;

        return swapRect.contains(point);
    }

    private boolean isWarmTile(Tile tile) {
        assert tile != null;

        return warmTiles.contains(tile);
    }

    private void loadUser(ReadUser user) {
        assert user != null;

        displayModes = user.getDisplayModes();
        assert displayModes.countColors() <= Markings.MARKING_COUNT_MAX;
        assert displayModes.countMarkings() <= Markings.MARKING_COUNT_MAX;

        startCellPosition = user.getStartCellPosition();
        setHintStrength(user.getHintStrength());
        menuBar.loadUser(user);
    }

    public void moveTarget(Direction direction) {
        assert direction != null;

        if (target == null) {
            target = new Cell(); // the start cell
        }

        for (;;) {
            target = new Cell(target, direction);
            if (target.isValid() && getBoard().isEmpty(target)) {
                break;
            }
        }

        if (!getBoard().mightUse(target)) {
            target = null;
        }
        panel.repaint();
    }

    @Override
    public void nextHand() {
        warmTiles = getBoardTiles();
        super.nextHand();
    }

    public void newGame(GameStyle oldStyle) {
        assert oldStyle != null;

        super.takeBack();

        warmTiles = new Tiles();
        menuBar.newGame(oldStyle);

        if (Game.hasInstance()) {
            // Re-initialize the preferences of all users involved in this game.
            displayModes.cleanup();
            recenter();
            User.newGame();
        }

        menuBar.update();
        panel.repaint();

        if (Game.hasInstance()) {
            assert !Game.getInstance().canRedo();
            assert !Game.getInstance().canUndo();

            final String report = Game.getInstance().reportBestRun();
            panel.showInformationBox(report, "Opening Bids");
            changeHand("");
        }
    }

    private void paintActiveTile() {
        if (hasActiveTile()) {
            final Tile tile = getActiveTile();
            final Point center = panel.getMouseLast();
            final boolean oddFlag = false;
            paintPlayableTile(center, tile, oddFlag);
        }
    }

    public void paintAll(Canvas canvas) {
        assert canvas != null;

        this.canvas = canvas;
        tileFinder.clear();

        if (!Game.hasInstance()) {
            paintIdle();
        } else if (Game.getInstance().isPaused()) {
            paintPaused();
        } else {
            final Area clientArea = panel.getClientArea();
            recenterLeftX = 0;
            recenterRightX = clientArea.width;

            paintBoard(Layer.VALID);
            paintBoard(Layer.HINTED_OR_EMPTY);
            paintUnplayableHands();
            paintPlayableHand();
            paintPlayableTiles();
            paintActiveTile();
        }

        this.canvas = null;
    }

    private void paintBlankTile(Point center, boolean bonusFlag,
            boolean oddFlag)
    {
        assert center != null;

        final Place place = findPlace(center);
        final Area area = getTileArea(place);
        Color color = Color.LIGHT_GRAY;
        if (bonusFlag) {
            color = Color.DULL_GOLD;
        }

        canvas.drawBlankTile(center, area, color, oddFlag);
    }

    private void paintBoard(Layer showLayer) {
        final Grid grid = getGameOpt().getGrid();
        final int extraColumns = grid.getColumnFringe() - 1;
        final int extraRows = grid.getRowFringe() - 1;

        // Determine which cells are visible in the client area.
        final Point ulc = new Point(0, 0);
        final Cell ulcCell = getCell(ulc);
        int topRow = ulcCell.getRow() + extraRows;
        int leftColumn = ulcCell.getColumn() + extraColumns;

        final Point brc = panel.getBottomRightCorner();
        final Cell brcCell = getCell(brc);
        int bottomRow = brcCell.getRow() - extraRows;
        int rightColumn = brcCell.getColumn() - extraColumns;

        if (!getGameOpt().doesBoardWrap()) {
            assert bottomRow <= topRow;
            assert leftColumn <= rightColumn;

            // Get the range of "might use" cells.
            final ReadBoard board = getBoard();
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

        int swapCount = countSwapped();
        Tile activeTile = getActiveTile();
        if (isInSwap(activeTile)) {
            assert swapCount > 0;
            --swapCount;
        }

        // Set target automatically if exactly one cell is hinted.
        if (Game.getInstance().getPlayable().getOpt().isLocalUser() &&
                target == null &&
                countHintedCells() == 1)
        {
            target = firstHintedCell();
        }

        for (int row = topRow; row >= bottomRow; row--) {
            paintRow(showLayer, leftColumn, rightColumn, swapCount, row);
        }
    }

    private void paintCell(Cell cell, Point center, int swapCnt) {
        assert cell != null;
        assert cell.isValid();

        Color cellColor = Color.BLACK;
        Color featureColor = Color.LIGHT_GRAY;
        final ReadBoard board = getBoard();
        final boolean isLocal =
                Game.getInstance().getPlayable().getOpt().isLocalUser();
        if (!board.isEmpty(cell)) {
            cellColor = Color.DARK_BLUE;
            featureColor = Color.LIGHT_BLUE;
        } else if (isLocal && isHinted(cell)) {
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
        if (cell.equals(target)) {
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
         * If tile is active, draw it later
         * (in PaintActiveTile() -- not now)
         * so it won't get obscured by other tiles.
         */
        final Tile tile = board.getContent(cell);
        if (tile != null && !isActive(tile)) {
            // inactive tile -- draw it now
            paintTile(center, tile, oddFlag);
        }
    }

    private Rect paintHandHeader(Point corner, int iHand, Side align) {
        assert corner != null;
        assert iHand >= 0;
        assert align != null;
        assert Game.hasInstance();

        // Calculate the width of the header.
        final Area cellArea = getCellArea(Place.HAND);
        int width = cellArea.width;

        final ReadHand hand = Game.getInstance().getHand(iHand);
        final String name = hand.getName();
        final Area nameArea = canvas.getTextArea(name);
        if (nameArea.width > width) {
            width = nameArea.width;
        }

        final boolean playableFlag = Game.getInstance().isPlayable(iHand);
        if (playableFlag) {
            final int stockWidth = canvas.getTextWidth("in the stock bag");
            if (stockWidth > width) {
                width = stockWidth;
            }
        }

        Area scoreArea = new Area(0, 0);
        String scoreText = "";
        if (menuBar.areScoresVisible()) {
            scoreText = formatScore(hand, playableFlag);
            scoreArea = canvas.getTextArea(scoreText);
            if (scoreArea.width > width) {
                width = scoreArea.width;
            }
        }

        Area clockArea = new Area(0, 0);
        String clockText = "";
        if (menuBar.areClocksVisible()) {
            clockText = formatClock(iHand);
            clockArea = canvas.getTextArea(clockText);
            if (clockArea.width > width) {
                width = clockArea.width;
            }
        }

        width += 2*padPixels;

        // Calculate hand header bounds.
        final int leftX = corner.x - (align == Side.RIGHT ? width : 0);
        Point ulc = new Point(leftX, corner.y);
        final int height = nameArea.height + scoreArea.height
                + clockArea.height + 2*padPixels;
        final Area area = new Area(width, height);
        final Rect result = new Rect(area, ulc);

        // Draw hand header background.
        final Color areaColor = Color.BLACK;
        canvas.backgroundColor = areaColor;
        canvas.foregroundColor = areaColor;
        canvas.drawRect(result);
        ulc.translate(0, padPixels);

        // Draw name text.
        nameArea.width = width;
        final Rect nameBox = new Rect(nameArea, ulc);
        canvas.foregroundColor = Color.WHITE;
        canvas.drawTextLine(nameBox, name, null);
        ulc.translate(0, nameArea.height);

        if (menuBar.areScoresVisible()) {
            // Draw score text.
            scoreArea.width = width;
            final Rect scoreBox = new Rect(scoreArea, ulc);
            canvas.drawTextLine(scoreBox, scoreText);
            ulc.translate(0, scoreArea.height);
        }

        if (menuBar.areClocksVisible()) {
            // Draw clock text.
            clockArea.width = width;
            final Rect clockBox = new Rect(clockArea, ulc);
            canvas.drawTextLine(clockBox, clockText);
        }

        return result;
    }

    private void paintIdle() {
        final Strings messageList = new Strings();

        if (GoldTile.isClient) {
            messageList.addLast("Type Ctrl+N to start a game.");
        }

        canvas.backgroundColor = Color.BLACK;
        canvas.foregroundColor = Color.GREEN;

        final Area client = panel.getClientArea();
        final int lineHeight = canvas.getFontHeight();
        final int startY = (client.height - messageList.size()*lineHeight)/2;
        final Rect line = new Rect(0, startY, client.width, lineHeight);
        for (int i = 0; i < messageList.size(); i++) {
            final String message = messageList.get(i);
            canvas.drawTextLine(line, message);
            line.y += lineHeight;
        }
    }

    private void paintPaused() {
        assert Game.hasInstance();
        assert Game.getInstance().isPaused();

        canvas.backgroundColor = Color.BLACK;
        canvas.foregroundColor = Color.GREEN;
        final Rect clientRect = panel.getClientRect();
        canvas.drawTextLine(clientRect, "The game is paused.  Click here to resume.");

        final Point ulc = new Point(padPixels, padPixels);
        final int iHand = Game.getInstance().getPlayableIndex();
        paintHandHeader(ulc, iHand, Side.LEFT);
    }

    private void paintPlayableHand() {
        assert Game.hasInstance();
        assert !Game.getInstance().isPaused();

        // Draw hand header.
        Point ulc = new Point(padPixels, padPixels);
        final int iHand = Game.getInstance().getPlayableIndex();
        final Rect headerRect = paintHandHeader(ulc, iHand, Side.LEFT);

        recenterLeftX = headerRect.getRightX();
        ulc = headerRect.getBlc();

        // Calculate height of playable hand area (mHandRect).
        int tileCount = countHand();
        final Area cellArea = getCellArea(Place.HAND);
        final int cellHeight = cellArea.height;
        int height = canvas.getTextHeight("My") + 2*padPixels;
        final ReadHand playableHand = Game.getInstance().getPlayable();
        if (!playableHand.hasResigned() && !playableHand.hasGoneOut()) {
            height = tileCount*cellHeight + 2*padPixels;
            if (tileCount < countPlayable()) {
                // Show that there's room for more.
                height += cellHeight/2;
            }
        }
        final Area handArea = new Area(headerRect.width, height);

        // Determine the colors of the playable hand area (handRect).
        final Tile activeTile = getActiveTile();
        if (isInHand(activeTile)) {
            // The active tile started from this hand.
            assert tileCount > 0;
            --tileCount;
        }

        final ReadHand hand = Game.getInstance().getPlayable();
        final boolean localUser = hand.getOpt().isLocalUser();
        final boolean hasGoneOut = hand.hasGoneOut();
        final boolean hasResigned = hand.hasResigned();
        Color areaColor;
        if (!localUser || hasGoneOut || hasResigned) {
            areaColor = Color.DARK_BLUE;
        } else if (tileCount < countPlayable()) {
            areaColor = Color.DARK_GREEN;
        } else { // hand is full
            areaColor = Color.BROWN;
        }
        final Color edgeColor = Color.WHITE;
        canvas.backgroundColor = areaColor;
        canvas.foregroundColor = edgeColor;

        // Draw the hand area (handRect).
        handRect = new Rect(handArea, ulc);
        canvas.drawRect(handRect);

        if (playableHand.isDisconnected()) {
            canvas.drawTextLine(handRect, "disconnected");
        } else if (playableHand.hasResigned()) {
            canvas.drawTextLine(handRect, "resigned");
        } else if (playableHand.hasGoneOut()) {
            canvas.drawTextLine(handRect, "went out");
        }
        ulc = handRect.getBlc();

        // Draw the swap area (swapRect) below the hand.
        if (playableHand.hasGoneOut() || playableHand.hasResigned()) {
            final Area swapArea = new Area(headerRect.width, 0);
            swapRect = new Rect(swapArea, ulc);
        } else {
            swapRect = paintSwapArea(ulc, headerRect.width);
        }
        ulc = swapRect.getBlc();

        // Draw stock bag below the swap area.
        paintStockArea(ulc, headerRect.width);
    }

    private void paintPlayableTile(Point center, Tile tile, boolean oddFlag) {
        assert center != null;
        assert tile != null;
        assert Game.hasInstance();

        final ReadBoard board = getBoard();
        final boolean localUser =
                Game.getInstance().getPlayable().getOpt().isLocalUser();
        if (!localUser &&
                !board.contains(tile) &&
                !menuBar.isPeeking())
        {
            // Draw the tile's backside.
            paintBlankTile(center, tile.hasBonus(), oddFlag);

        } else {  // Draw the tile's face.
            paintTile(center, tile, oddFlag);
        }
    }

    private void paintPlayableTiles() {
        assert Game.hasInstance();

        final Area cellArea = getCellArea(Place.HAND);
        final int cellHeight = cellArea.height;
        final int x = handRect.getMiddleX();
        int handY = handRect.y + padPixels + cellHeight/2;
        int swapY = swapRect.y + padPixels + cellHeight/2;

        final int tileCount = countSwapped();
        final int stockCount = Game.getInstance().countStock();
        if (tileCount < countPlayable() && tileCount < stockCount) {
            swapY += cellHeight/2;
        }

        final ReadBoard board = getBoard();
        Point center;
        final Tiles contents = Game.getInstance().getPlayable().copyContents();
        for (Tile tile : contents) {
            boolean oddFlag = false;
            final Place place = find(tile);
            assert place != null : tile;
            switch (place) {
                case BOARD:
                    final Cell cell = board.find(tile);
                    oddFlag = cell.isOdd();
                    center = getCenter(cell);
                    break;
                case HAND:
                    center = new Point(x, handY);
                    handY += cellHeight;
                    break;
                case SWAP_AREA:
                    center = new Point(x, swapY);
                    swapY += cellHeight;
                    break;
                default:
                    throw new AssertionError(place);
            }

            //  Draw the active tile later (not now) so it won't get obscured.
            if (!isActive(tile)) {
                // inactive tile -- draw it now
                paintPlayableTile(center, tile, oddFlag);
            }
        }
    }

    private void paintRow(Layer showLayer, int leftSeeColumn,
            int rightSeeColumn, int swapCnt, int row)
    {
        assert Game.hasInstance();

        final ReadBoard board = getBoard();

        for (int column = leftSeeColumn; column <= rightSeeColumn; column++) {
            final Cell cell = new Cell(row, column);
            final Cell wrapCell = cell.wrap();

            if ( (board.mightUse(cell) || board.mightUse(wrapCell))
                      && wrapCell.isValid() )
            {
                final Point center = getCenter(cell);
                final boolean isEmpty = board.isEmpty(wrapCell);
                final boolean isHinted = isHinted(wrapCell);
                final Layer layer = (isEmpty || isHinted) ?
                            Layer.HINTED_OR_EMPTY : Layer.VALID;
                if (layer == showLayer) {
                    paintCell(cell, center, swapCnt);
                }
            }
        }
    }

    private void paintStockArea(Point ulc, int width) {
        assert ulc != null;
        assert width > 0;
        assert Game.hasInstance();

        // Calculate the height of the stock area.
        final int stockCount = Game.getInstance().countStock();
        final String text1 = StringExt.plural(stockCount, "tile");
        final Area area1 = canvas.getTextArea(text1);

        final String text2 = "in the stock bag";
        final Area area2 = canvas.getTextArea(text2);

        final int height = area1.height + area2.height + 3*padPixels;
        final Area area = new Area(width, height);
        final Rect rect = new Rect(area, ulc);

        // Choose colors for the stock area.
        final Color areaColor = Color.DARK_BLUE;
        final Color edgeColor = Color.WHITE;
        canvas.backgroundColor = areaColor;
        canvas.foregroundColor = edgeColor;

        // Draw the stock area.
        canvas.drawRect(rect);

        // Draw two lines of descriptive text in the stock area.
        final Point ulc1 = new Point(ulc.x, ulc.y + padPixels);
        area1.width = width;
        final Rect bounds1 = new Rect(area1, ulc1);
        canvas.drawTextLine(bounds1, text1);

        final Point ulc2 = new Point(ulc.x, ulc1.y + area1.height + padPixels);
        area2.width = width;
        final Rect bounds2 = new Rect(area2, ulc2);
        canvas.drawTextLine(bounds2, text2);
    }

    private Rect paintSwapArea(Point ulc, int width) {
        assert ulc != null;
        assert width > 0;
        assert Game.hasInstance();

        // Calculate height of swap area (swapRect)
        final Area cellArea = getCellArea(Place.HAND);
        final int cellHeight = cellArea.height;
        final int stockCount = Game.getInstance().countStock();
        int tileCount = countSwapped();
        final String text = "swap area";
        final Area textArea = canvas.getTextArea(text);
        final int textHeight = textArea.height;
        int height = tileCount*cellHeight + textHeight + 3*padPixels;
        if (tileCount < countPlayable() && tileCount < stockCount) {
            // Show that there's room for more tiles.
            height += cellHeight/2;
        }

        final Area swapArea = new Area(width, height);
        final Rect result = new Rect(swapArea, ulc);

        // Choose colors for the swap area.
        final Tile activeTile = getActiveTile();
        if (isInSwap(activeTile)) {
            assert tileCount > 0;
            --tileCount;
        }
        ReadBoard board = getBoard();
        int placedTileCount = countPlayed();
        if (board.contains(activeTile)) {
            assert placedTileCount > 0;
            --placedTileCount;
        }
        Color areaColor;
        final boolean localUser =
                Game.getInstance().getPlayable().getOpt().isLocalUser();
        if (!localUser) {
            areaColor = Color.DARK_BLUE;
        } else if (placedTileCount == 0
            && tileCount < countPlayable()
            && tileCount < stockCount)
        {
            areaColor = Color.DARK_GREEN;
        } else { // can't add more tiles to swap area
            areaColor = Color.BROWN;
        }
        canvas.backgroundColor = areaColor;

        Color edgeColor = Color.WHITE;
        canvas.foregroundColor = edgeColor;

        // Draw the swap area.
        canvas.drawRect(result);

        // Label the swap area.
        ulc.y = result.getBottomY() - padPixels - textHeight;
        textArea.width = width;
        final Rect bounds = new Rect(textArea, ulc);
        canvas.drawTextLine(bounds, text);

        return result;
    }

    private void paintTile(Point center, Tile tile, boolean oddFlag) {
        assert center != null;
        assert tile != null;

        final int markingCount = displayModes.countMarkings();
        assert markingCount <= Markings.MARKING_COUNT_MAX;

        // Determine the tile's background color.
        Color tileColor = Color.LIGHT_GRAY;
        if (tile.hasBonus()) {
            tileColor = Color.DULL_GOLD;
        }
        if (isPlayable(tile) &&
                Game.getInstance().getPlayable().getOpt().isLocalUser())
        {
            // Highlight tiles which the local user can drag around.
            tileColor = Color.WHITE;
            if (tile.hasBonus()) {
                tileColor = Color.GOLD;
            }
        }

        final Markings markings = new Markings(tile, displayModes);
        final Place place = findPlace(center);
        final Area area = getTileArea(place);
        final boolean warmFlag = isWarmTile(tile);
        final Rect interior = canvas.drawTile(markings, tileColor, center,
                area, warmFlag, oddFlag);

        final boolean localFlag =
                Game.getInstance().getPlayable().getOpt().isLocalUser();
        if (isPlayable(tile) && localFlag) {
            tileFinder.put(tile.getId(), interior);
        }
    }

    private void paintUnplayableHands() {
        assert Game.hasInstance();

        final Area cellArea = getCellArea(Place.BOARD);
        final int cellHeight = cellArea.height;
        final Color areaColor = Color.DARK_BLUE;
        final Color edgeColor = Color.LIGHT_GRAY;
        final boolean peekFlag = menuBar.isPeeking();

        int rightX = panel.getClientArea().width - padPixels;
        int topY = padPixels;
        for (int iHand : Game.getInstance().getUnplayableIndices()) {
            // Draw the header.
            final Point urc = new Point(rightX, topY);
            final Rect headerRect = paintHandHeader(urc, iHand, Side.RIGHT);

            // Draw hand area below the header.
            final Point ulc = headerRect.getBlc();
            final ReadHand hand = Game.getInstance().getHand(iHand);
            final int tileCount = hand.countContents();
            final String text = reportStatus(hand, peekFlag);

            int height = 2*padPixels;
            if (text == null) {
                height += cellHeight * tileCount;
            } else {
                height += canvas.getTextHeight(text);
            }
            final Area area = new Area(headerRect.width, height);
            final Rect bounds = new Rect(area, ulc);
            canvas.backgroundColor = areaColor;
            canvas.foregroundColor = edgeColor;
            canvas.drawRect(bounds);

            if (text != null) {
                canvas.drawTextLine(bounds, text);
            } else {  // peeking:  draw tiles
                final int yCenter = bounds.y + padPixels + cellHeight/2;
                Point center = new Point(bounds.getMiddleX(), yCenter);

                for (Tile tile : hand.copyContents()) {
                    paintTile(center, tile, false);
                    center.translate(0, cellHeight);
                }
            } // if peeking

            recenterRightX = bounds.x;

            // Insert padding between hands.
            if (peekFlag) {
                // right to left
                rightX = bounds.x - padPixels;
            } else {
                // top to bottom
                topY += height + padPixels;
            }
        }
    }

    public void playMenuCommand(String command) {
        assert command != null;
        assert Game.hasInstance();
        assert !hasActiveTile();
        assert !Game.getInstance().isPaused();
        assert !Game.getInstance().isOver();

        // Check whether the playable hand has run out of time.
        ReadMove move;
        if (Game.getInstance().isOutOfTime()) {
            // If it has, force a resignation.
            command = "Resign";
        }

        if (command.equals("Resign")) {
            final Tiles discard =
                    Game.getInstance().getPlayable().copyContents();
            move = new Move(discard);
        } else {
            move = getMove(Active.INCLUDED);
        }

        switch (command) {
            case "Pass":
                assert move.isPass();
                break;
            case "Play":
                assert move.doesPlace() || move.isPureSwap();
                break;
            case "Resign":
                assert move.isResignation();
                break;
            default:
                throw new AssertionError();
        }

        // Check whether the move is a legal one.
        final UserMessage reason = Game.getInstance().checkMove(move);
        if (reason == null) {
            finishTurn(move);

        } else { // explain the issue
            panel.showRuleBox(reason);
            if (reason == UserMessage.FIRST_TURN) {
                takeBack();
            }
        }
    }

    public void recenter() {
        assert Game.hasInstance();

        final Area clientArea = panel.getClientArea();

        if (recenterLeftX >= recenterRightX) {
            recenterLeftX = 0;
            recenterRightX = clientArea.width;
        }
        int x = (recenterLeftX + recenterRightX)/2;
        int y = clientArea.height/2;

        final Area gridUnitArea = getGridUnitArea();
        final ReadBoard board = getBoard();

        x -= gridUnitArea.width * (board.getEastMax() - board.getWestMax())/2;
        y += gridUnitArea.height * (board.getNorthMax() - board.getSouthMax())/2;

        startCellPosition = new Point(x, y);
    }

    public void repaint() {
        panel.repaint();
    }

    private String reportStatus(ReadHand hand, boolean peekFlag) {
        assert hand != null;

        if (hand.isDisconnected()) {
            return "disconnected";
        } else if (hand.hasResigned()) {
            return "resigned";
        } else if (hand.hasGoneOut()) {
            return "went out";
        } else if (peekFlag) {
            return null;
        } else {
            final int tileCount = hand.countContents();
            return StringExt.plural(tileCount, "tile");
        }
    }

    public String saveHand() {
        if (!Game.hasInstance()) {
            return "";
        }

        final ReadHandOpt handOpt = Game.getInstance().getPlayable().getOpt();
        final String playerName = handOpt.getPlayerName();

        if (handOpt.isLocalUser()) {
            final User user = User.lookup(playerName);
            saveUser(user);
        }

        return playerName;
    }

    private void saveUser(User user) {
        assert user != null;

        menuBar.saveUser(user);
        user.setDisplayModes(displayModes);
        user.setHintStrength(getHintStrength());
        user.setStartCellPosition(startCellPosition);
    }

    public void setDisplayModes(DisplayModes displayModes) {
        if (displayModes != null) {
            this.displayModes = displayModes;
            repaint();
        }
    }

    public void startClock() {
        assert Game.hasInstance();

        Game.getInstance().startClock();
        menuBar.update();
        repaint();
    }

    public void togglePause() {
        assert Game.hasInstance();

        Game.getInstance().togglePause();
        menuBar.update();
        repaint();
    }

    public void toggleTargetCell(Point point) {
        final Cell rawCell = getCell(point);
        assert rawCell != null;

        final Cell wrapCell = rawCell.wrap();

        if (wrapCell.isValid() && isInCellInterior(point, rawCell)) {
            final ReadBoard board = getBoard();

            if (target != null && wrapCell.equals(target)) {
                target = null;
            } else if (board.mightUse(wrapCell) && board.isEmpty(wrapCell)) {
                // new target cell
                target = wrapCell;
            }
        }
    }

    public void translate(int dx, int dy) {
        startCellPosition.translate(dx, dy);
    }

    public void updateClock() {
        if (menuBar.areClocksVisible() &&
                Game.hasInstance() &&
                Game.getInstance().getPlayable().isClockRunning()) {
            // don't update menus
            repaint();
        }
    }
}
