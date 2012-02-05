#ifndef TOPWINDOW_HPP_INCLUDED
#define TOPWINDOW_HPP_INCLUDED

// File:    topwindow.hpp
// Purpose: top window for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A TopWindow object represents the main window
 for the game.

 The TopWindow class is implemented by extending Window.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "board.hpp"
#include "gui/canvas.hpp"
#include "gui/rect.hpp"
#include "gui/playmenu.hpp"
#include "gui/viewmenu.hpp"
#include "gui/window.hpp"
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

private:
	static WindowClass *mspClass;

	TileIdType    mActiveTileId;
	HINSTANCE     mApplication;	
	bool          mAutopauseFlag;
	Board         mBoard;
	ACountType    mColorAttributeCnt;
	bool          mDragBoardFlag, mDragTileFlag;
	int           mDragTileDeltaX, mDragTileDeltaY;
	Game *       mpGame;
	char *        mFileName;
	Rect          mHandRect;
	Tiles         mHandTiles;
	int           mMouseLastX, mMouseLastY; // coordinates of last mouse down
	bool          mOriginIsCentered;
	long          mOriginX, mOriginY; // logical coordinates of ulc of origin cell
	unsigned      mPadPixels;
	bool          mPauseFlag;
    PlayMenu *    mpPlayMenu;
    unsigned      mPlayedTileCnt;
	unsigned      mRunLength;
	bool          mShowClocksFlag, mShowGridFlag, mShowHintsFlag;
	bool          mShowScoresFlag, mShowTilesFlag;
	Tiles         mSwapTiles;
	Rect          mSwapRect;
	TileMapType   mTileMap;
	TileWidthType mTileWidth;
    ViewMenu *    mpViewMenu;

	// misc
    void DrawActivePlayer(Canvas &);
	Rect DrawBlankTile(Canvas &, int topY, int leftX);
    void DrawBoard(Canvas &);
    void DrawCell(Canvas &, Cell const &);
    void DrawHandTiles(Canvas &);
    void DrawInactivePlayers(Canvas &);
	void DrawPaused(Canvas &);
    Rect DrawPlayerHeader(Canvas &, int top, int leftRight, Player const &, 
                          ColorType, bool leftFlag);
	Rect DrawTile(Canvas &, int topY, int leftX, Tile const &);
    void HandleButtonDown(int x, int y);
	void HandleButtonUp(int x, int y);
	void HandleDragMouse(int x, int y);
    void HandleMenuCommand(int);
    void Initialize(CREATESTRUCT const *);
    void Play(bool passFlag);
	void Recenter(unsigned oldWidth, unsigned oldHeight);
    void Resize(unsigned width, unsigned height);
    void Repaint(void);
	void UpdateMenus(void);

	// access
    unsigned     CellHeight(void) const;
    int          CellX(int column) const;
    int          CellY(int row) const;
    unsigned     CellWidth(void) const;
    char const * ClassName(void) const;
	Cell         GetCell(int x, int y) const;
    unsigned     GridUnit(void) const;
	char const * Name(void) const;
};

#endif
#endif