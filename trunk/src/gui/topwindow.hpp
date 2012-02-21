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

#include <map>
#include "gui/color.hpp"
#include "gui/parmbox1.hpp"
#include "gui/rect.hpp"
#include "gui/window.hpp"
#include "partial.hpp"


class TopWindow: public Window {
public:
	// lifecycle
	TopWindow(HINSTANCE, Game *pGame);
	// no default constructor
    // no copy constructor
    ~TopWindow(void);

	// operator
    TopWindow &operator=(TopWindow const &) { FAIL(); };

	// misc public methods
	LRESULT HandleMessage(UINT message, WPARAM, LPARAM);
	int     MessageDispatchLoop(void);

private:
    typedef std::map<TileIdType,Rect>    TileMapType;
    typedef std::pair<TileIdType,Rect>   TilePairType;
    typedef TileMapType::iterator        TileIterType;
    typedef std::pair<TileIterType,bool> TileInsResultType;

	static WindowClass *mspClass;

	HINSTANCE     mApplication;	
	bool          mAutopauseFlag;
	bool          mAutocenterFlag;
	ACountType    mColorAttributeCnt;
	bool          mDragBoardFlag;
	PCntType      mDragBoardPixelCnt;
	long          mDragTileDeltaX, mDragTileDeltaY;
    FileMenu *   mpFileMenu;
	Game *       mpGame;
	GameStyleType mGameStyle;
	Rect          mHandRect;
	bool          mInitialNewGame;
	bool          mIsStartCentered;
	Point         mMouseLast; // coordinates of last mouse down
	unsigned      mMouseUpCnt;
	PCntType      mPadPixels;
	Partial       mPartial;
    PlayMenu *   mpPlayMenu;
	unsigned      mRunLength;
	bool          mShowClocksFlag, mShowGridFlag;
	bool          mShowScoresFlag, mShowTilesFlag;
	Point         mStartCell; // logical coordinates of center of start cell
	Rect          mSwapRect;
	Cell          mTargetCell;
	bool          mTargetCellFlag;
	TileMapType   mTileMap;
	int           mTileSizeCmd;
	PCntType      mTileWidth;
    ViewMenu *   mpViewMenu;

	// lifecycle
    void Initialize(CREATESTRUCT const *);

	// misc private methods
    PCntType     CellHeight(void) const;
    LogicalXType CellX(IndexType column) const;
    LogicalYType CellY(IndexType row) const;
    PCntType     CellWidth(void) const;
    char const * ClassName(void) const;
	String       ClockText(Hand &) const;
	int          CreateNewGame(void);
	void         DiscardGame(void);
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
	Cell         GetCell(Point const &) const;
    TileIdType   GetTileId(Point const &) const;
    PCntType     GridUnitX(void) const;
    PCntType     GridUnitY(void) const;
    void         HandleButtonDown(Point const &);
	void         HandleButtonUp(Point const &);
    void         HandleMenuCommand(int);
	void         HandleMouseMove(Point const &);
	char const * Name(void) const;
	void		 OfferNewGame(void);
	void		 OfferSaveGame(void);
    void         Play(bool passFlag);
	void         Recenter(PCntType oldWidth, PCntType oldHeight);
	void         ReleaseActiveTile(Point const &);
    void         Resize(PCntType width, PCntType height);
    void         Repaint(void);
	void         SetGame(Game *);
	void         SetTileWidth(int command);
	void         StopDragging(void);
	PCntType     TileHeight(void) const;
	void         TogglePause(void);
	void         UpdateMenus(void);

	// private inquiry methods
	bool IsDragging(void) const;
	bool IsGameOver(void) const;
	bool IsGamePaused(void) const;
    bool IsInBounds(Cell const &) const;
	bool IsInCellArea(Point const &, Cell const &) const;
	bool IsInHandArea(Point const &) const;
	bool IsInSwapArea(Point const &) const;
	bool IsInTile(Point const &) const;

	//constants
	static const unsigned HAND_CNT_DEFAULT = 2;
    static const unsigned ID_CLOCK_TIMER = 1;
	static const unsigned TIMEOUT_MSEC = 1000;
};

#endif
