#ifndef GAMEVIEW_HPP_INCLUDED
#define GAMEVIEW_HPP_INCLUDED

// File:    gameview.hpp
// Purpose: GameView class header file
// Author:  Stephen Gold sgold@sonic.net
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

/*
A GameView object represents a view of the game in progress in the client
area of a window.

The GameView class extends the Partial class with drawing methods.
Drawing is performed on a temporary Canvas owned by the caller.
*/

#include "gui/color.hpp"
#include "gui/displaymodes.hpp" // HASA DisplayModes
#include "gui/rect.hpp"         // HASA Rect
#include "partial.hpp"          // ISA Partial

class GameView: public Partial {
public:
	// public lifecycle
	GameView::GameView(Game const &rGame);
	// no default constructor
    // ~GameView(void);  compiler-generated destructor is OK
	void SetWindow(GameWindow *, MenuBar *);

	// public operators
    // GameView &operator=(GameView const &);  compiler-generated assignment operator is OK
	operator DisplayModes(void) const;

	// misc public methods
	Cell       GetPointCell(Point const &) const;
    TileIdType GetTileId(Point const &) const;
	void       LoadPlayerOptions(Player const &);
	void       Recenter(PixelCntType oldWidth, PixelCntType oldHeight);
    void       Repaint(Canvas &);
	void       ResetTargetCell(void);
	void       SavePlayerOptions(Player &) const;
	void       SetDisplayModes(DisplayModes const &);
	void       SetGame(Game *);
	void       SetTileWidth(IdType command);
	void       StartCellOffset(long dx, long dy);
	Cell       TargetCell(void) const;
	Point      TileCenter(TileIdType) const;
	void       ToggleTargetCell(Point const &);

	// public inquiry methods
	bool IsDragging(void) const;
    bool IsInBounds(Cell const &) const;
	bool IsInCellArea(Point const &, Cell const &) const;
	bool IsInHandArea(Point const &) const;
	bool IsInSwapArea(Point const &) const;
	bool IsInTile(Point const &) const;
	bool IsTargetSet(void) const;
	bool IsTargetUsed(void) const;

private:
	// private types
    typedef std::map<TileIdType,Rect>  TileMap;
    typedef std::pair<TileIdType,Rect> TilePair;
    typedef TileMap::iterator          TileIter;
    typedef TileMap::const_iterator    TileConstIter;
    typedef std::pair<TileIter,bool>   TileInsResult;

	// private data
	DisplayModes mDisplayModes;
	Rect         mHandRect;
	MenuBar *   mpMenuBar;
	PixelCntType mPadPixels;
	Point        mStartCell;
	Rect         mSwapRect;
	Cell         mTargetCell;
	bool         mTargetCellFlag;
	TileMap      mTileMap;
	PixelCntType mTileWidth;
	GameWindow * mpWindow;

	// private lifecycle
	GameView(GameView const &);  // not copyable

	// private operators
    GameView &operator=(GameView const &);  // not assignable

	// misc private methods
    PixelCntType  CellHeight(void) const;
    PixelCntType  CellWidth(void) const;
    LogicalXType  CellX(IndexType column) const;
    LogicalYType  CellY(IndexType row) const;
	String        ClockText(Hand &) const;
	AttrIndexType ColorAttribute(void) const;
    void          DrawActiveHand(Canvas &);
	void          DrawBlankTile(Canvas &, Point const &, bool odd);
    void          DrawBoard(Canvas &, unsigned layer);
    void          DrawCell(Canvas &, Cell const &, unsigned swapCnt);
	void          DrawPaused(Canvas &);
    Rect          DrawHandHeader(Canvas &, LogicalYType, LogicalXType leftRight, Hand &, 
                                ColorType, bool leftFlag);
    void          DrawHandTile(Canvas &, Point const &, Tile const &, bool odd);
    void          DrawHandTiles(Canvas &);
    void          DrawInactiveHands(Canvas &);
	void          DrawStockArea(Canvas &, LogicalYType, LogicalXType, PixelCntType width);
	Rect          DrawSwapArea(Canvas &, LogicalYType, LogicalXType, PixelCntType width);
	Rect          DrawTile(Canvas &, Point const &center, Tile const &, bool odd);
    PixelCntType  GridUnitX(void) const;
    PixelCntType  GridUnitY(void) const;
	PixelCntType  TileHeight(void) const;

	// private inquiry methods
	bool IsGridVisible(void) const;

};
#endif // !defined(GAMEVIEW_HPP_INCLUDED)
