// File:     User.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  User class for the Gold Tile Game
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

public class User
    implements ReadUser
{
    // per-instance fields

    private boolean autopause = false;
    private boolean peek = false;
    private boolean showClocks = false;
    private boolean showGrid = true;
    private boolean showScores = true;
    private DisplayModes displayModes = new DisplayModes(Display.GUI);
    private GameStyle style = GameStyle.NONE;
    private HintStrength strength = HintStrength.getStrongDefault();
    private int boardTileSize = MenuBar.BOARD_TILE_SIZE_DEFAULT;
    private int handTileSize = MenuBar.HAND_TILE_SIZE_DEFAULT;

    // logical coordinates of the Start cell
    private Point startCellPosition = new Point();

    // static fields
    private static java.util.Map<String,User> users =
            new java.util.TreeMap<>();

    // methods, sorted by name

    @Override
    public boolean getAutopause() {
        return autopause;
    }

    @Override
    public int getBoardTileSize() {
        return boardTileSize;
    }

    @Override
    public DisplayModes getDisplayModes() {
        return new DisplayModes(displayModes);
    }

    @Override
    public int getHandTileSize() {
        return handTileSize;
    }

    @Override
    public HintStrength getHintStrength() {
        return strength;
    }

    @Override
    public boolean getPeek() {
        return peek;
    }

    @Override
    public boolean getShowClocks() {
        return showClocks;
    }

    @Override
    public boolean getShowGrid() {
        return showGrid;
    }

    @Override
    public boolean getShowScores() {
        return showScores;
    }

    @Override
    public Point getStartCellPosition() {
        return startCellPosition;
    }

    public static User lookup(String name) {
        assert name != null;
        assert !name.equals("");
        assert users.containsKey(name);

        return users.get(name);
    }

    public static void newGame() {
        assert Game.hasInstance();

        final GameStyle newStyle = Game.getStyle();
        final Strings names = Game.getInstance().getUserNames();
        for (String name : names) {
            assert name != null;
            assert !name.equals("");

            User user;
            if (!users.containsKey(name)) {
                // Create a new instance and add it to the map.
                user = new User();
                users.put(name, user);
            } else {
                user = users.get(name);
            }

            // Center the view and reset board size.
            Point start = GameView.getInstance().getStartCellPosition();
            user.startCellPosition = start;
            user.boardTileSize = MenuBar.BOARD_TILE_SIZE_DEFAULT;

            if (user.style != newStyle) {
                // Override the style-sensitive settings.
                user.autopause = newStyle.hasTimeLimit();
                user.peek = newStyle.isDebug();
                user.showClocks = newStyle.showsClocks();
                user.showScores = true;
                user.strength = HintStrength.getStrongDefault();

                user.style = newStyle;
            }

            // Override any illegal settings.
            if (!newStyle.allowsStrongHints() &&
                    user.strength.isStrong())
            {
                user.strength = HintStrength.getWeakDefault();
            }
            if (!newStyle.allowsPeeking()) {
                user.peek = false;
            }
            user.displayModes.cleanup();
        }
    }

    public void setAutopause(boolean flag) {
        autopause = flag;
    }

    public void setBoardTileSize(int size) {
        assert size >= SizeMenu.MIN;
        assert size <= SizeMenu.MAX;

        boardTileSize = size;
    }

    public void setDisplayModes(DisplayModes displayModes) {
        this.displayModes = new DisplayModes(displayModes);
    }

    public void setHandTileSize(int size) {
        assert size >= SizeMenu.MIN;
        assert size <= SizeMenu.MAX;

        handTileSize = size;
    }

    public void setHintStrength(HintStrength strength) {
        assert strength != null;

        this.strength = strength;
    }

    public void setPeek(boolean flag) {
        peek = flag;
    }

    public void setShowClocks(boolean flag) {
        showClocks = flag;
    }

    public void setShowGrid(boolean flag) {
        showGrid = flag;
    }

    public void setShowScores(boolean flag) {
        showScores = flag;
    }

    public void setStartCellPosition(Point position) {
        assert position != null;

        startCellPosition = position;
    }
}
