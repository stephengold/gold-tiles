#ifndef TOPWINDOW_HPP_INCLUDED
#define TOPWINDOW_HPP_INCLUDED

// File:    topwindow.hpp
// Purpose: top window for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A TopWindow object represents the main window
 for the game.

 The TopWindow class is implemented by extending Window.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "board.hpp"
#include "gui/window.hpp"
#include "gui/canvas.hpp"
#include "gui/menu.hpp"
#include "tile.hpp"

typedef std::map<TileId, RECT> TileMap;
typedef std::pair<TileId, RECT> TilePair;

class TopWindow: public Window {
	static WindowClass *_class;

	HINSTANCE _application;
	char *_fileName;
	
	bool _autopauseFlag;
	Board _board;
	bool _dragBoardFlag, _dragTileFlag;
	int _dragTileDeltaX, _dragTileDeltaY;
	TileId _dragTileId;
	Tiles _handTiles;
	RECT _handRect;
	int _mouseLastX, _mouseLastY; // coordinates of last mouse down
	ACount _numberOfColorAttributes;
	bool _originHasBeenCentered;
	long _originX, _originY; // logical coordinates of ulc of origin cell
	bool _pauseFlag;
    PlayMenu *_playMenu;
    unsigned _playedTileCount;
	unsigned _runLength;
	bool _showClocksFlag;
    bool _showGridFlag;
	bool _showHintsFlag;
	bool _showScoresFlag;
	bool _showTilesFlag;
	Tiles _swapTiles;
	RECT _swapRect;
	TileMap _tileMap;
	unsigned _tileWidth;
    ViewMenu *_viewMenu;

    void buttonDown(int x, int y);
	void buttonUp(int x, int y);
    void createdWindow(CREATESTRUCT const *);
	void dragMouse(int x, int y);

	RECT drawBlankTile(Canvas &, int topY, int leftX);
    void drawBoard(Canvas &);
    void drawCell(Canvas &, GridRef const &);
    void drawHandTiles(Canvas &, unsigned pad, unsigned bagHeight);
    void drawInactivePlayers(Canvas &);
    RECT drawPlayerHeader(Canvas &, int top, int left, Player const &, COLORREF);
	RECT drawTile(Canvas &, int topY, int leftX, Tile const &);

	GridRef getCellRef(int x, int y);
    unsigned getCellWidth(void) const;
    int getCellX(int column) const;
    int getCellY(int row) const;
    char const *getClassName(void) const;
    unsigned getGridUnit(void) const;
    WNDPROC getMessageHandler(void) const;
	char const *getName(void) const;

    void menuCommand(int);
    void play(bool passFlag);
	void recenter(unsigned oldWidth, unsigned oldHeight);
    void resize(unsigned width, unsigned height);
    void repaint(void);
	void updateMenus(void);

    public:
		TopWindow(void) { assert(false); };
        TopWindow(TopWindow const &) { assert(false); };
        TopWindow &operator=(TopWindow const &) { assert(false); };
        // ~WindowClass(void);  implicitly declared destructor is fine

		TopWindow(HINSTANCE);

        LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
#endif
