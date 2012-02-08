#ifndef TOPWINDOW_HPP_INCLUDED
#define TOPWINDOW_HPP_INCLUDED

// File:    topwindow.hpp
// Purpose: TopWindow class
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
A TopWindow object represents the primary window for the game.

The TopWindow class is an extension of the Window class.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "board.hpp"
#include "cells.hpp"
#include "gui/canvas.hpp"
#include "gui/rect.hpp"
#include "gui/playmenu.hpp"
#include "gui/viewmenu.hpp"
#include "gui/window.hpp"
#include "partial.hpp"
#include "tile.hpp"

typedef std::map<TileIdType,Rect>  TileMapType;
typedef std::pair<TileIdType,Rect> TilePairType;

enum TileWidthType {
    TILE_WIDTH_SMALL = 21,
	TILE_WIDTH_MEDIUM = 41,
	TILE_WIDTH_LARGE = 61
};

class TopWindow: public Window {
public:
	// lifecycle
	TopWindow(void) { ASSERT(false); };
	TopWindow(HINSTANCE, Game *pGame);
    TopWindow(TopWindow const &) { ASSERT(false); };
    ~TopWindow(void);

	// operator
    TopWindow &operator=(TopWindow const &) { ASSERT(false); };

	// misc
	LRESULT HandleMessage(UINT message, WPARAM, LPARAM);
	int     MessageDispatchLoop(void);

private:
	static WindowClass *mspClass;

	Cell          mActiveCell;
	bool          mActiveCellFlag;
	HINSTANCE     mApplication;	
	bool          mAutopauseFlag;
	bool          mAutocenterFlag;
	ACountType    mColorAttributeCnt;
	bool          mDragBoardFlag;
	unsigned      mDragBoardPixelCnt;
	int           mDragTileDeltaX, mDragTileDeltaY;
	Game *       mpGame;
	char *        mFileName;
	Rect          mHandRect;
	bool          mIsStartCentered;
	int           mMouseLastX, mMouseLastY; // coordinates of last mouse down
	int           mMouseUpCnt;
	unsigned      mPadPixels;
	Partial       mPartial;
	bool          mPauseFlag;
    PlayMenu *   mpPlayMenu;
	unsigned      mRunLength;
	bool          mShowClocksFlag, mShowGridFlag, mShowHintsFlag;
	bool          mShowScoresFlag, mShowTilesFlag;
	bool          mSquareGrid;      // false = hex grid
	long          mStartX, mStartY; // logical coordinates of center of start cell
	Rect          mSwapRect;
	TileMapType   mTileMap;
	TileWidthType mTileWidth;
    ViewMenu *   mpViewMenu;

	// lifecycle
    void Initialize(CREATESTRUCT const *);

	// misc
    unsigned     CellHeight(void) const;
    int          CellX(int column) const;
    int          CellY(int row) const;
    unsigned     CellWidth(void) const;
    char const * ClassName(void) const;
    void         DrawActivePlayer(Canvas &);
	Rect         DrawBlankTile(Canvas &, int centerY, int centerX);
    void         DrawBoard(Canvas &);
    void         DrawCell(Canvas &, Cell const &, unsigned swapCnt);
    void         DrawHandTile(Canvas &, int y, int x, Tile const &);
    void         DrawHandTiles(Canvas &);
    void         DrawInactivePlayers(Canvas &);
	void         DrawPaused(Canvas &);
    Rect         DrawPlayerHeader(Canvas &, int top, int leftRight, Player const &, 
                          ColorType, bool leftFlag);
	Rect         DrawTile(Canvas &, int centerY, int centerX, Tile const &);
	Cell         GetCell(int x, int y) const;
    TileIdType   GetTileId(int x, int y) const;
    unsigned     GridUnit(void) const;
    void         HandleButtonDown(int x, int y);
	void         HandleButtonUp(int x, int y);
    void         HandleMenuCommand(int);
	void         HandleMouseMove(int x, int y);
	char const * Name(void) const;
    void         Play(bool passFlag);
	void         Recenter(unsigned oldWidth, unsigned oldHeight);
	void         ReleaseActiveTile(int x, int y);
    void         Resize(unsigned width, unsigned height);
    void         Repaint(void);
	void         StopDragging(void);
	void         TakeBack(void);
	void         UpdateMenus(void);

	// inquiry
	bool IsDragging(void) const;
    bool IsInBounds(Cell const &) const;
	bool IsInHandArea(int x, int y) const;
	bool IsInSwapArea(int x, int y) const;
	bool IsInTile(int x, int y) const;
	bool IsPaused(void) const;
};

#endif
#endif
