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
Drawing is performed using a temporary Canvas owned by the caller.
*/

#include "partial.hpp"  // ISA Partial

class GameView: public Partial {
public:
	// public types

	// public constants

	// public lifecycle
	GameView::GameView(Game const &rGame);
	// no default constructor
    // ~GameView(void);  compiler-generated destructor is OK
	void SetWindow(TopWindow *, MenuBar *);

	// public operators
    // GameView &operator=(GameView const &);  compiler-generated assignment operator is OK

	// misc public methods
    PCntType     CellHeight(void) const;
    LogicalXType CellX(IndexType column) const;
    LogicalYType CellY(IndexType row) const;
    PCntType     CellWidth(void) const;
    void         DrawActiveHand(Canvas &);
	void         DrawBlankTile(Canvas &, Point const &, bool odd);
    void         DrawBoard(Canvas &, unsigned layer);
    void         DrawCell(Canvas &, Cell const &, unsigned swapCnt);
    Rect         DrawHandHeader(Canvas &, LogicalYType top, LogicalXType leftRight, Hand &, 
                                ColorType, bool leftFlag);
    void         DrawHandTile(Canvas &, Point const &, Tile const &, bool odd);
    void         DrawHandTiles(Canvas &);
    void         DrawInactiveHands(Canvas &);
	void         DrawPaused(Canvas &);
	Rect         DrawTile(Canvas &, Point, Tile const &, bool odd);
	Cell         GetPointCell(Point const &) const;
    TileIdType   GetTileId(Point const &) const;
    PCntType     GridUnitX(void) const;
    PCntType     GridUnitY(void) const;
	void         Recenter(PCntType oldWidth, PCntType oldHeight);
    void         Repaint(Canvas &rCanvas);
	void         ResetTargetCell(void);
	void         SetGame(Game *);
	void         SetStartCellPosition(Point const &);
	void         SetTileWidth(IdType command);
	void         StartCellOffset(long dx, long dy);
	Point        StartCellPosition(void) const;
	Cell         TargetCell(void) const;
	Point        TileCenter(TileIdType) const;
	PCntType     TileHeight(void) const;
	void         ToggleTargetCell(Point const &);

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

	// private constants

	// private data
	ACountType             mColorAttributeCnt;
	Rect                   mHandRect;
	MenuBar *             mpMenuBar;
	PCntType               mPadPixels;
	Point                  mStartCell;
	Rect                   mSwapRect;
	Cell                   mTargetCell;
	bool                   mTargetCellFlag;
	TileMap                mTileMap;
	PCntType               mTileWidth;
	TopWindow *           mpWindow;

	// private lifecycle
	GameView(GameView const &);  // not copyable

	// private operators
    GameView &operator=(GameView const &);  // not assignable

	// misc private methods

	// private inquiry methods
	bool IsGridVisible(void) const;

};

// global utility functions

#endif
