// File:     UserMessage.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  UserMessage enum for the Gold Tile Game
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

public enum UserMessage {
    // values 
    COLUMN_COMPATIBILITY (
        "Tiles in a column (with no intervening empty cells) must all be mutually compatible.",
        "Column Compatibility Rule"),
    DIAGONAL_COMPATIBILITY (
        "Tiles on a diagonal (with no intervening empty cells) must all be mutually compatible.",
        "Diagonal Compatibility Rule"),
    EMPTY_CELL (
        "That cell has already been used.",
        "Empty Cell Rule"),
    FIRST_TURN (
        "On the first turn, you must play as many tiles as possible.  Keep looking!",
        "First Turn Rule"),
    NEIGHBOR (
        "Each cell you use must be a neighbor of a used cell.",
        "Neighbor Rule"),
    REPEATED_CELL (
        "You can't use the same cell twice.",
        "Repeated Cell Rule"),
    REPEATED_TILE (
        "You can't use the same tile twice.",
        "Repeated Tile Rule"),
    ROW_COMPATIBILITY (
        "Tiles in a row (with no intervening empty cells) must all be mutually compatible.",
        "Row Compatibility Rule"),
    RULES (
        "The rules of Gold Tile are available online at http://code.google.com/p/gold-tiles/wiki/Playing",
        "Rules"),
    SCORING_AXIS (
        "The cells you use must lie along a single scoring axis, with no empty cells between them.",
        "Scoring Axis Rule"),
    START (
        "Your first tile must use the start cell. To change this cell, take back ALL your played tiles.",
        "Start Rule"),
    START_SIMPLE (
        "Your first tile must use the start cell.",
        "Start Rule"),
    STOCK (
        "You can't swap more tiles than the number remaining in the stock bag.",
        "Stock Rule"),
    SWAP (
        "You can play tiles or swap them, but you can't do both in the same turn.",
        "Swap Rule");
    
    // per-instance fields
    final public String message;
    final public String title;
    
    // constructor
    private UserMessage(String message, String title) {
        this.message = message;
        this.title = title;
    }
}
