// File:     ReadMove.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ReadMove interface for the Gold Tile Game
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

public interface ReadMove {
    public Cells copyCells();
    public java.util.TreeSet<TileCell> copySet();
    public Tiles copyTiles();
    public int countCellsUsed();
    public String describe();
    public boolean doesPlace();
    public boolean equals(Move other);
    public void implement(Partial partial);
    public boolean involvesSwap();
    public boolean isPass();
    public boolean isPureSwap();
    public boolean isResignation();
    public void place(Board board);
    public boolean repeatsCell();
    public boolean repeatsTile();
    public int size();
    @Override
    public String toString();
}
