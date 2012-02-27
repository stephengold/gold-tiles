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
A TopWindow object represents the main window for the game.

The TopWindow class is an extension of the Window class.
*/

#include "gui/color.hpp"
#include "gui/window.hpp"  // ISA Window
#include "partial.hpp"     // HASA Partial

class TopWindow: public Window {
public:
	// public lifecycle
	TopWindow(Win::HINSTANCE, Game *pGame);
	// no default constructor
    ~TopWindow(void);

	// misc public methods
	Win::LRESULT HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
	int          MessageDispatchLoop(void);

private:
	// private types
    typedef std::map<TileIdType,Rect>  TileMap;
    typedef std::pair<TileIdType,Rect> TilePair;
    typedef TileMap::iterator          TileIter;
    typedef TileMap::const_iterator    TileConstIter;
    typedef std::pair<TileIter,bool>   TileInsResult;

	// private constants
	static const unsigned HAND_CNT_DEFAULT = 2;
    static const unsigned ID_CLOCK_TIMER = 1;
	static const unsigned TIMEOUT_MSEC = 1000;

	// private data
	static WindowClass * mspClass;
	ACountType             mColorAttributeCnt;
	bool                   mDragBoardFlag;
	PCntType               mDragBoardPixelCnt;
	long                   mDragTileDeltaX, mDragTileDeltaY;
	Game *                 mpGame;
	Rect                   mHandRect;
	bool                   mInitialNewGame;
	bool                   mIsStartCentered;
	MenuBar *             mpMenuBar;
	Point                  mMouseLast; // coordinates of last mouse down
	unsigned               mMouseUpCnt;
	PCntType               mPadPixels;
	Partial                mPartial;  // the move in progress
	Point                  mStartCell;
	Rect                   mSwapRect;
	Cell                   mTargetCell;
	bool                   mTargetCellFlag;
	TileMap                mTileMap;
	PCntType               mTileWidth;

	// private lifecycle
    TopWindow(TopWindow const &); // not copyable
    void Initialize(Win::CREATESTRUCT const &);

	// private operator
    TopWindow &operator=(TopWindow const &); // not assignable

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
    void         HandleMenuCommand(IdType);
	void         HandleMouseMove(Point const &);
	void         InfoBox(char const *message);
	void         LoadPlayerOptions(Hand const &);
	char const * Name(void) const;
	void		 OfferNewGame(void);
	void		 OfferSaveGame(void);
    void         Play(bool passFlag);
	void         Recenter(PCntType oldWidth, PCntType oldHeight);
	void         ReleaseActiveTile(Point const &);
    void         Repaint(void);
    void         Resize(PCntType width, PCntType height);
	void         RuleBox(char const *message);
	void         SavePlayerOptions(Hand const &) const;
	void         SetGame(Game *);
	void         SetTileWidth(IdType command);
	void         StopDragging(void);
	PCntType     TileHeight(void) const;
	void         TogglePause(void);
	void         UpdateMenuBar(void);
	int          WarnBox(char const *message);

	// private inquiry methods
	bool IsDragging(void) const;
	bool IsGameOver(void) const;
	bool IsGamePaused(void) const;
    bool IsInBounds(Cell const &) const;
	bool IsInCellArea(Point const &, Cell const &) const;
	bool IsInHandArea(Point const &) const;
	bool IsInSwapArea(Point const &) const;
	bool IsInTile(Point const &) const;

};

#endif
