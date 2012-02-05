// File:    topwindow.cpp
// Purpose: TopWindow class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
//#include "about.h"
#include "cell.hpp"
#include "cells.hpp"
#include "game.hpp"
#include "gui/canvas.hpp"
#include "gui/color.hpp"
#include "gui/resource.hpp"
#include "gui/topwindow.hpp"
#include "gui/windowclass.hpp"
#include "move.hpp"
#include "player.hpp"

// static member of TopWindow

WindowClass *TopWindow::mspClass = NULL;

// message handler (callback) for top window
static TopWindow *newlyCreatedTopWindow = NULL;
static LRESULT CALLBACK topMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
    TopWindow *window;
	if (message == WM_CREATE && newlyCreatedTopWindow != NULL) {
		window = newlyCreatedTopWindow;
		newlyCreatedTopWindow = NULL;
		window->SetHandle(windowHandle);
	} else {
       window = (TopWindow *)Window::Lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(window->Handle() == windowHandle);
        result = window->HandleMessage(message, wParam, lParam);
	}
	return result;
}

// constructors, assignment operator, destructor, and cast operators

TopWindow::TopWindow(HINSTANCE applicationInstance):
    mHandRect(0, 0, 0, 0),
    mSwapRect(0, 0, 0, 0)
{
	ASSERT(Handle() == 0);

	char const *className = ClassName();
    if (mspClass == NULL) {
		// constructing first instance:  create a Microsoft Windows window class
		WNDPROC messageHandler = MessageHandler();
		mspClass = new WindowClass(applicationInstance, messageHandler, className);
		mspClass->RegisterClass();
	}
	ASSERT(mspClass != NULL);

	// Make this TopWindow object accessable to its message handler before WM_CREATE.
    assert(newlyCreatedTopWindow == NULL);
	newlyCreatedTopWindow = this;

    ASSERT(game != NULL);
    Player activePlayer = game->ActivePlayer();
    
	mApplication = applicationInstance;
    mAutopauseFlag = false;
    mBoard = Board(*game);
    mColorAttributeCnt = 1;
    mDragBoardFlag = false;
    mDragTileFlag = false;
    mFileName = "Game1";
    mHandTiles = Tiles(activePlayer);
    mMouseLastX = 0;
    mMouseLastY = 0;
    mOriginIsCentered = false;
    mOriginX = 0;
    mOriginY = 0;
    mPadPixels = 6;
	mPauseFlag = mAutopauseFlag;
    mpPlayMenu = NULL;
    mPlayedTileCnt = 0;    
	mShowClocksFlag = false;
	mShowGridFlag = true;
	mShowScoresFlag = true;
	mShowTilesFlag = false;    
	mTileWidth = TILE_WIDTH_LARGE;
    mpViewMenu = NULL;

	// create Microsoft Windows window
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	int x = CW_USEDEFAULT;
	int y = CW_USEDEFAULT;
	int width = CW_USEDEFAULT;
	int height = CW_USEDEFAULT;
	HWND parent = NULL;
	HMENU menu = NULL;
	LPVOID parameters = NULL;
    HWND handle = ::CreateWindow(className, Name(), windowStyle, x, y, 
                             width, height, parent, menu, applicationInstance, 
                             parameters);
    ASSERT(handle != 0);
    SetHandle(handle);

}

TopWindow::~TopWindow(void) {
	delete mpPlayMenu;
	delete mpViewMenu;
}

// TopWindow methods

void TopWindow::ButtonDown(int x, int y) {
    if (mPauseFlag) {
        MenuCommand(IDM_PAUSE);
		return;
    }
    HWND this_window = Handle();
    POINT pt; pt.x = x; pt.y = y;
    
    TileMapType::const_iterator it;
    for (it = mTileMap.begin(); it != mTileMap.end(); it++) {
        Rect rect = it->second;
        if (rect.Contains(pt)) {
            break;
        }
    }
    if (it != mTileMap.end()) {
        // Capture mouse to drag a tile
        ::SetCapture(this_window);
        mMouseLastX = x;
        mMouseLastY = y;
        mDragTileDeltaX = 0;
        mDragTileDeltaY = 0;
        mDragTileFlag = true;
        mDragTileId = it->first;
        
    } else if (!mHandRect.Contains(pt) && !mSwapRect.Contains(pt)) {
        // Capture mouse to drag the board
    	::SetCapture(this_window);

        mMouseLastX = x;
        mMouseLastY = y;
        mDragBoardFlag = true;
    }
}

void TopWindow::ButtonUp(int x, int y) {
    HWND this_window = Handle();
	HWND captor = ::GetCapture();
    if (captor != this_window) {
		return;
    } else {
        ::ReleaseCapture();
    }
		
    int drag_x = x - mMouseLastX;
    int drag_y = y - mMouseLastY;
    if (mDragBoardFlag) {
        mOriginX += drag_x;
        mOriginY += drag_y;
        
    } else {
        ASSERT(mDragTileFlag);
        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
        
        Tile drag_tile = mHandTiles.FindTile(mDragTileId);

		// Determine where the tile was dragged from.
        bool from_board = false;
        bool from_swap = false;
		Cell from_cell;
        if (mBoard.FindTile(drag_tile, from_cell)) {
            from_board = true;
            mBoard.MakeEmpty(from_cell);
            --mPlayedTileCnt;
        } else if (mSwapTiles.Contains(drag_tile)) {
            from_swap = true;
            mSwapTiles.RemoveTile(drag_tile);
        }

        // Determine where the tile got dragged to.
        POINT point;
		point.x = x;
		point.y = y;
        
        if (mSwapRect.Contains(point)) { // to swap
            if (mPlayedTileCnt == 0) {
                mSwapTiles.AddTile(drag_tile);
            } else if (from_board) {
                mBoard.PlayOnCell(from_cell, drag_tile);
                ++mPlayedTileCnt;
            }
            
        } else if (!mHandRect.Contains(point)) { // to board
            Cell to_cell = GetCell(x, y);
            Move move;
            move.Add(drag_tile, to_cell);
            if (mSwapTiles.Count() == 0 && mBoard.IsLegalMove(move)) {
                mBoard.PlayMove(move);
                ++mPlayedTileCnt;
            } else if (from_board) {
                mBoard.PlayOnCell(from_cell, drag_tile);
                ++mPlayedTileCnt;
            } else if (from_swap) {
                mSwapTiles.AddTile(drag_tile);
            }
        }
    }
    
    mDragBoardFlag = false;
    mDragTileFlag = false;
    mMouseLastX = x;
    mMouseLastY = y;

	ForceRepaint();
}

unsigned TopWindow::CellHeight(void) const {
    unsigned result = CellWidth();

	return result;
}

unsigned TopWindow::CellWidth(void) const {
    unsigned result = mTileWidth;
    if (mShowGridFlag) {
        result = mTileWidth + 1;
    }

    return result;
}

int TopWindow::CellX(int column) const {
    unsigned grid_unit = GridUnit();
    int result = mOriginX + grid_unit*column;

    return result;
}

int TopWindow::CellY(int row) const {
    unsigned grid_unit = GridUnit();
    int result = mOriginY - grid_unit*row;

    return result;
}

char const *TopWindow::ClassName(void) const {
	return "GOLDTILETOP";
}


void TopWindow::DragMouse(int x, int y) {
    HWND this_window = Handle();
	HWND captor = ::GetCapture();
    if (captor != this_window) {
		return;
    }

    int dragX = x - mMouseLastX;
    int dragY = y - mMouseLastY;
    if (mDragBoardFlag) {
        mOriginX += dragX;
        mOriginY += dragY;
        ForceRepaint();
    } else if (mDragTileFlag) {
        mDragTileDeltaX += dragX;
        mDragTileDeltaY += dragY;
        ForceRepaint();
    }

    mMouseLastX = x;
    mMouseLastY = y;
}


void TopWindow::DrawActivePlayer(Canvas &rCanvas) {
    // draw header
    int top_y = mPadPixels;
    int left_x = mPadPixels;
    Player active_player = game->ActivePlayer();
    ColorType area_color = COLOR_BLACK;
    bool left = true;
    Rect header_rect = DrawPlayerHeader(rCanvas, top_y, left_x, active_player, area_color, left);
    left_x = header_rect.LeftX();
    unsigned width = header_rect.Width();
    
    // draw hand area (mHandRect)
    unsigned cell_height = CellHeight();
    unsigned tile_count = mHandTiles.Count() - mSwapTiles.Count() - mPlayedTileCnt;
    unsigned height = tile_count*cell_height + 2*mPadPixels;
    if (tile_count < mHandTiles.Count()) {
        height += cell_height/2;
        area_color = COLOR_DARK_GREEN;
    } else { // hand is full
        area_color = COLOR_BROWN;
    }
    ColorType edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);
    top_y = header_rect.BottomY() - 1;
    mHandRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    left_x = mHandRect.LeftX();
    width = mHandRect.Width();
    
    // draw swap area (_swapRect)
    tile_count = mSwapTiles.Count();
    unsigned bagHeight = rCanvas.TextHeight();
    height = tile_count*cell_height + bagHeight + 3*mPadPixels;
    if (tile_count < mHandTiles.Count() && mPlayedTileCnt == 0) {
        height += cell_height/2;
        area_color = COLOR_DARK_GREEN;
    } else { // can't add tiles to swap area
        area_color = COLOR_BROWN;
    }
    rCanvas.UseColors(area_color, edge_color);
    top_y = mHandRect.BottomY() - 1;
    mSwapRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    
    unsigned stock = game->CountStock();
    String stock_text = plural(stock, "tile");
    int y = mSwapRect.BottomY() - mPadPixels - bagHeight;
    Rect bounds(y, left_x, width, bagHeight);
    rCanvas.DrawText(bounds, stock_text);
}

Rect TopWindow::DrawBlankTile(Canvas &rCanvas, int topY, int leftX) {
    ColorType tile_color = COLOR_LIGHT_GRAY;
	ColorType glyph_color = tile_color;
    unsigned width = CellWidth() - 2;
    AValueType glyphs[4];
    Rect result = rCanvas.DrawTile(topY, leftX, width, 0, glyphs, tile_color, glyph_color);
    
    return result;
}

void TopWindow::DrawBoard(Canvas &rCanvas) {
    int top_row = 1 + (int)mBoard.NorthMax();
    int bottom_row = -1 - (int)mBoard.SouthMax();
    int right_column = 1 + (int)mBoard.EastMax();
    int left_column = -1 - (int)mBoard.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);
    
    for (int row = top_row; row >= bottom_row; row--) {
        if (CellY(row) > (int)mClientAreaHeight) {
            break;
        }
        for (int column = left_column; column <= right_column; column++) {
            if (CellX(column) > (int)mClientAreaWidth) {
                break;
            }
            Cell cell(row, column);
            DrawCell(rCanvas, cell);
        }
    }
}

void TopWindow::DrawCell(Canvas &rCanvas, Cell const &rCell) {
    int row = rCell.Row();
    int column = rCell.Column();
    int ulc_x = CellX(column);
    int ulc_y = CellY(row);

    Cells done;
    bool connected = mBoard.ConnectsToOrigin(rCell, done);

    ColorType cellColor = COLOR_DARK_GREEN;
    if (!connected) {
        cellColor = COLOR_BLACK;
    } else if (!mSwapTiles.IsEmpty()) {
        cellColor = COLOR_BROWN;
    }
    ColorType gridColor = cellColor;
    if (mShowGridFlag) {
        if (mSwapTiles.IsEmpty()) {
            gridColor = COLOR_GREEN;
        } else {
            gridColor = COLOR_YELLOW;
        }
    }

    unsigned cellWidth = CellWidth();
    rCanvas.DrawCell(ulc_y, ulc_x, cellWidth, cellColor, gridColor);
    
    Tile const *tile = mBoard.GetCell(rCell);
    if (tile != NULL && (!mDragTileFlag || tile->Id() != mDragTileId)) {
        DrawTile(rCanvas, ulc_y + 1, ulc_x + 1, *tile);
    }
}

void TopWindow::DrawHandTiles(Canvas &canvas) {   
    int hand_y = mHandRect.TopY() + mPadPixels;
    int swap_y = mSwapRect.TopY() + mPadPixels;
    
    unsigned cell_height = CellHeight();
    if (mSwapTiles.Count() < mHandTiles.Count() && mPlayedTileCnt == 0) {
        swap_y += cell_height/2;
    }

    mTileMap.clear();
    unsigned cell_width = CellWidth();
    Tiles::ConstIteratorType it;
    for (it = mHandTiles.begin(); it != mHandTiles.end(); it++) {
        Tile tile = *it;
        int x, y;
        Cell cell;
        if (mBoard.FindTile(tile, cell)) {
            int row = cell.Row();
            int column = cell.Column();
            x = CellX(column);
            y = CellY(row);
        } else {
            if (mSwapTiles.Contains(tile)) {
                int left = mSwapRect.LeftX();
                int width = mSwapRect.Width();
                int pad = (width - cell_width)/2;
                x = left + pad;
                y = swap_y;
                swap_y += cell_height;
            } else { 
                int left = mHandRect.LeftX();
                int width = mHandRect.Width();
                int pad = (width - cell_width)/2;
                x = left + pad;
                y = hand_y;
                hand_y += cell_height;
            }
        }
        Rect rect = DrawTile(canvas, y + 1, x + 1, tile);
        
        TileIdType id = tile.Id();
        TilePairType pair(id, rect);
        std::pair<TileMapType::iterator, bool> ins;
        ins = mTileMap.insert(pair);
        bool success = ins.second;
        ASSERT(success);
    }
}

void TopWindow::DrawInactivePlayers(Canvas &rCanvas) {     
    unsigned cell_width = CellWidth();
    unsigned cell_height = CellHeight();
    unsigned width = cell_width + 2*mPadPixels;
    ColorType area_color = COLOR_DARK_BLUE;
    ColorType edge_color = COLOR_LIGHT_GRAY;

    Players other_players = game->InactivePlayers();
    Players::ConstIteratorType i_player;
    int right_x = mClientAreaWidth - mPadPixels;
    for (i_player = other_players.begin(); i_player < other_players.end(); i_player++) {
        // draw header
        int top_y = mPadPixels;
        bool rightFlag = false;
        ColorType header_color = COLOR_BLACK;
        Rect header_rect = DrawPlayerHeader(rCanvas, top_y, right_x, *i_player, header_color, rightFlag);

        // draw hand area below the header
        top_y = header_rect.BottomY() - 1;
        int left_x = header_rect.LeftX();
        int width = header_rect.Width();
        Tiles hand_tiles = Tiles(*i_player);
        unsigned tile_count = hand_tiles.Count();
        unsigned height = tile_count*cell_height + 2*mPadPixels;
        area_color = COLOR_DARK_BLUE;
        rCanvas.UseColors(area_color, edge_color);
        Rect hand_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

        // draw tiles
        int pad = (width - cell_width)/2;
        int tile_x = left_x + pad + 1;
        int tile_y = hand_rect.TopY() + mPadPixels + 1;

        Tiles::ConstIteratorType it;
        for (it = hand_tiles.begin(); it != hand_tiles.end(); it++) {
            Tile tile = *it;
            if (mShowTilesFlag) {
                DrawTile(rCanvas, tile_y, tile_x, tile);
            } else {
                DrawBlankTile(rCanvas, tile_y, tile_x);
            } 
            tile_y += cell_height;
        }

		// pad between players
        right_x = header_rect.LeftX() - mPadPixels;
    }
}

Rect TopWindow::DrawPlayerHeader(
    Canvas &rCanvas, 
    int topY, 
    int leftRight, 
    Player const &rPlayer, 
    ColorType areaColor, 
    bool leftFlag)
{
    unsigned cell_width = CellWidth();

    String name_text = rPlayer.Name();
    Player active_player = game->ActivePlayer();
    if (name_text == active_player.Name()) {
        name_text += "'s turn";
    }
    unsigned w = rCanvas.TextWidth(name_text);
    unsigned width = (cell_width > w) ? cell_width : w;

    String scoreText;
    if (mShowScoresFlag) {
        unsigned score = rPlayer.Score();
        scoreText = plural(score, "point");
        w = rCanvas.TextWidth(scoreText);
        if (w > width) {
            width = w;
        }
    }
    
    char const *clock_text = "";
    if (mShowClocksFlag) {
        clock_text = "0:00";
        w = rCanvas.TextWidth(clock_text);
        if (w > width) {
            width = w;
        }
    }
                                 
    width += 2*mPadPixels;
    int left_x, right_x;
    if (leftFlag) {
        left_x = leftRight;
        right_x = left_x + width;
    } else {
        right_x = leftRight;
        left_x = right_x - width;
    }
    unsigned text_height = rCanvas.TextHeight();

    unsigned score_height = 0;
    if (mShowScoresFlag) {
        score_height = text_height;
    }
    unsigned clock_height = 0;
    if (mShowClocksFlag) {
        clock_height = text_height;
    }
    unsigned height = text_height + score_height + clock_height + 2*mPadPixels;
    rCanvas.UseColors(areaColor, areaColor);
    Rect result = rCanvas.DrawRectangle(topY, left_x, width, height);

    ColorType text_color = COLOR_WHITE;
    rCanvas.UseColors(areaColor, text_color);

    int y = topY + mPadPixels;
    Rect bounds(y, left_x, width, text_height);
    rCanvas.DrawText(bounds, name_text);
    y += text_height;
    
    if (mShowScoresFlag) {
        Rect score_box(y, left_x, width, score_height);
        rCanvas.DrawText(score_box, scoreText);
        y += score_height;
    }

    if (mShowClocksFlag) {
        Rect clock_box(y, left_x, width, text_height);
        rCanvas.DrawText(clock_box, clock_text);
    }
    
    return result;
}

Rect TopWindow::DrawTile(Canvas &rCanvas, int topY, int leftX, Tile const &rTile) {
    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (mHandTiles.Contains(rTile)) {
        tile_color = COLOR_WHITE;
    }
    if (mDragTileFlag && rTile.Id() == mDragTileId) {
       topY += mDragTileDeltaY;
       leftX += mDragTileDeltaX;
    }
	
	AIndexType colorInd;
	if (mColorAttributeCnt == 1) {
        colorInd = rTile.Attribute(0);
	} else {
		ASSERT(mColorAttributeCnt == 0);
        colorInd = 0;
	}
	assert(colorInd < 9);
	static ColorType GlyphColors[9] = {
		COLOR_BLACK,      COLOR_RED,       COLOR_DARK_BLUE, 
        COLOR_DARK_GREEN, COLOR_PURPLE,    COLOR_BROWN, 
        COLOR_DARK_GRAY,  COLOR_PINK,      COLOR_LIGHT_BLUE
	};
    ColorType glyph_color = GlyphColors[colorInd];

    ACountType numberOfGlyphAttributes = Tile::AttributeCnt() - mColorAttributeCnt;
    AValueType glyphs[4];
    for (AIndexType gi = 0; gi < 4; gi++) {
		AIndexType ind = gi + mColorAttributeCnt;
         if (gi < numberOfGlyphAttributes) {
             glyphs[gi] = rTile.Attribute(ind);
         } else {
             glyphs[gi] = 0;
         }
    }

    unsigned width = CellWidth() - 2;
    Rect result = rCanvas.DrawTile(topY, leftX, width, 
                    numberOfGlyphAttributes, glyphs, tile_color, glyph_color);
    
    return result;
}

Cell TopWindow::GetCell(int x, int y) const {
    unsigned grid_unit = GridUnit();
    int column = -400 + (x - mOriginX + 400*grid_unit)/grid_unit;
    int row = 400 - (y - mOriginY + 400*grid_unit)/grid_unit;
    Cell result(row, column);
    
    return result;
}

unsigned TopWindow::GridUnit(void) const {
    unsigned result = mTileWidth - 1;
    if (mShowGridFlag) {
        result = mTileWidth;
    }

    return result;
}

WNDPROC TopWindow::MessageHandler(void) const {
    return &topMessageHandler;
}

char const *TopWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

LRESULT TopWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
        case WM_COMMAND: { // menu command
	        int command = LOWORD(wParam);
            MenuCommand(command);
            break;
  	    }

        case WM_CREATE: { // initialize window
		    CREATESTRUCT *p_create_struct = (CREATESTRUCT *)lParam;
            Initialize(p_create_struct);
            break;
        }

        case WM_LBUTTONDOWN: { // left-click
			POINTS point = MAKEPOINTS(lParam);
            ButtonDown(point.x, point.y);
            break;
	    }

        case WM_LBUTTONUP: {
			POINTS point = MAKEPOINTS(lParam);
            ButtonUp(point.x, point.y);
  		    break;
		}

        case WM_MOUSEMOVE:
            if (wParam & MK_LBUTTON) { // dragging with left button
	  	        POINTS point = MAKEPOINTS(lParam);
                DragMouse(point.x, point.y);
		    }
            break;

	    case WM_PAINT: // repaint
	        UpdateMenus();
            Repaint();
            break;
           
        case WM_SIZE: { // resize
            unsigned long clientAreaWidth = LOWORD(lParam);
            unsigned long clientAreaHeight = HIWORD(lParam);
            Resize(clientAreaWidth, clientAreaHeight);
            break;
        }

	    default:  // invoke message handler of the base class
		    Window *base = (Window *)this;
		    result = base->HandleMessage(message, wParam, lParam);
		    break;
    }

    return result;
}

void TopWindow::Initialize(CREATESTRUCT const *pCreateStruct) {
    // Initialization which happens after the Microsoft Windows window
    // has been created.

	Window::Initialize(pCreateStruct);

    HMENU menu_bar = pCreateStruct->hMenu;

    mpPlayMenu = new PlayMenu(menu_bar, 1);
	ASSERT(mpPlayMenu != NULL);

	mpViewMenu = new ViewMenu(menu_bar, 2);
	ASSERT(mpViewMenu != NULL);

	UpdateMenus();
}

void TopWindow::MenuCommand(int command) {
	ASSERT(mpPlayMenu != NULL);
	ASSERT(mpViewMenu != NULL);

	HWND this_window = Handle();

    switch (command) {
    // File menu options
        case IDM_NEW: {
	        // TODO
		    mFileName = "Game1";
            break;
	    }
		case IDM_OPEN:
		    break;
		case IDM_REOPEN:
		    break;
		case IDM_REVERT:
		    break;
		case IDM_SAVE:
		    break;
		case IDM_SAVE_AS:
		    break;
		case IDM_CLOSE:
		    break;
		case IDM_PRINT:
		    break;
        case IDM_EXIT: { // terminate the application
		    // TODO prompt for save
	    	UINT message = WM_CLOSE;
            ::SendMessage(this_window, message, 0, 0L);
            break;
	    }

	    // Play menu options
	    case IDM_PAUSE:
		    mPauseFlag = !mPauseFlag;
		    ForceRepaint();
		    break;
		case IDM_ACCEPT:
			break;
	    case IDM_PLAY_PLAY: {
            bool passFlag = false;
			Play(passFlag);
		    break;
        }
	    case IDM_PASS: {
            bool passFlag = true;
            Play(passFlag);
            break;
        }
		case IDM_RESIGN:
			break;
		case IDM_RESTART:
			break;
		case IDM_UNDO:
			break;
		case IDM_REDO:
			break;
        case IDM_AUTOPAUSE:
            mAutopauseFlag = !mAutopauseFlag;
	        UpdateMenus();
            break;

	    // View menu options
        case IDM_SMALL_TILES:
            mTileWidth = TILE_WIDTH_SMALL;
	        ForceRepaint();
            break;
        case IDM_MEDIUM_TILES:
            mTileWidth = TILE_WIDTH_MEDIUM;
		    ForceRepaint();
            break;
        case IDM_LARGE_TILES:
            mTileWidth = TILE_WIDTH_LARGE;
		    ForceRepaint();
            break;
        case IDM_RECENTER:
            mOriginIsCentered = false;
            Resize(mClientAreaWidth, mClientAreaHeight);
            break;
		case IDM_ATTRIBUTES:
			break;
	    case IDM_SHOW_CLOCKS:
            mShowClocksFlag = !mShowClocksFlag;
	        ForceRepaint();
            break;
        case IDM_SHOW_GRID:
            mShowGridFlag = !mShowGridFlag;
	        ForceRepaint();
            break;
        case IDM_SHOW_HINTS:
            mShowHintsFlag = !mShowHintsFlag;
	        ForceRepaint();
            break;
        case IDM_SHOW_SCORES:
            mShowScoresFlag = !mShowScoresFlag;
	        ForceRepaint();
            break;
        case IDM_SHOW_TILES:
            mShowTilesFlag = !mShowTilesFlag;
	        ForceRepaint();
            break;
		case IDM_ANIMATION:
			break;

        // Help menu options
        case IDM_RULES:
            ::MessageBox(this_window, "The Rules of Gold Tile", 
		       "Gold Tile", MB_ICONINFORMATION | MB_OK);
            break;
        case IDM_ABOUT: {
	   	    AboutDialog about(mModule, "About", this_window);
            break;
		}

		default:
			ASSERT(false);
    }
}

void TopWindow::Play(bool passFlag) {
    Move pl;
    
    Tiles::ConstIteratorType it;
    for (it = mHandTiles.begin(); it != mHandTiles.end(); it++) {
        Tile tile = *it;
        Cell cell;
        if (mBoard.FindTile(tile, cell)) {
           pl.Add(tile, cell);
        } else if (mSwapTiles.Contains(tile)) {
           pl.Add(tile);
        }
    }
    
    if (pl.IsPass() == passFlag && game->IsLegalMove(pl)) {
        game->FinishTurn(pl);
        if (game->IsOver()) {
            mShowClocksFlag = true;
            mShowScoresFlag = true;
            mShowTilesFlag = true;
            game->GoingOutBonus();
        } else {
            game->ActivateNextPlayer();
            if (mAutopauseFlag) {
                mPauseFlag = true;
            }
        }

        mBoard = Board(*game);
        Player activePlayer = game->ActivePlayer();
        mHandTiles = Tiles(activePlayer);
        mPlayedTileCnt = 0;
        mSwapTiles.MakeEmpty();
       
        // center the origin
        mOriginIsCentered = false;
        Resize(mClientAreaWidth, mClientAreaHeight);
    }
}

void TopWindow::Recenter(unsigned oldHeight, unsigned oldWidth) {
    if (mOriginIsCentered) {
        //_originX += (_clientAreaWidth - oldWidth)/2;
        //_originY += (_clientAreaHeight - oldHeight)/2;
    } else if (mClientAreaWidth > 250 && mClientAreaHeight > 100) {
        mOriginX = mClientAreaWidth/2; // TODO
        mOriginY = mClientAreaHeight/2;
        mOriginIsCentered = true;
    }
}

void TopWindow::Repaint(void) {
    HWND this_window = Handle();
    PAINTSTRUCT paint_struct;    
    HDC context = ::BeginPaint(this_window, &paint_struct);
    ASSERT(context != NULL);
    
    bool release_me = false;
    Canvas canvas(context, this_window, release_me, mClientAreaWidth, mClientAreaHeight);
    
    if (mPauseFlag) {
        ColorType bgColor = COLOR_BLACK;
        ColorType textColor = COLOR_WHITE;
        canvas.UseColors(bgColor, textColor);

        int x = 0;
        int y = 0;
        Rect clientArea(y, x, mClientAreaWidth, mClientAreaHeight);
        canvas.DrawText(clientArea, "The game is paused.  Click here to proceed.");
        
        int top_y = mPadPixels;
        int left_x = mPadPixels;
        Player activePlayer = game->ActivePlayer();
        bool left = true;
        DrawPlayerHeader(canvas, top_y, left_x, activePlayer, bgColor, left);

    } else {
        DrawBoard(canvas);
        DrawInactivePlayers(canvas);
        DrawActivePlayer(canvas);
        DrawHandTiles(canvas);
    }

    canvas.Close();
    ::EndPaint(this_window, &paint_struct);
}

void TopWindow::Resize(unsigned clientAreaWidth, unsigned clientAreaHeight) {
    unsigned old_height = mClientAreaHeight;
    unsigned old_width = mClientAreaWidth;
    SetClientArea(clientAreaWidth, clientAreaHeight);
    Recenter(old_height, old_width);
    ForceRepaint();
}

void TopWindow::UpdateMenus(void) {
    unsigned count = mSwapTiles.Count() + mPlayedTileCnt;
    bool playable = (count > 0);
	UINT tile_size;
	switch (mTileWidth) {
 	    case TILE_WIDTH_SMALL:
	        tile_size = IDM_SMALL_TILES;
			break;
 	    case TILE_WIDTH_MEDIUM:
	        tile_size = IDM_MEDIUM_TILES;
			break;
 	    case TILE_WIDTH_LARGE:
	        tile_size = IDM_LARGE_TILES;
			break;
		default:
			ASSERT(false);
	}

	// "Play" menu
    mpPlayMenu->Autopause(mAutopauseFlag);
    mpPlayMenu->Pause(mPauseFlag);
	mpPlayMenu->EnableItems(mPauseFlag, playable);

	mpPlayMenu->Enable(true);

	// "View" menu
    mpViewMenu->TileSize(tile_size);
    mpViewMenu->ShowClocks(mShowClocksFlag);
    mpViewMenu->ShowGrid(mShowGridFlag);
    mpViewMenu->ShowHints(mShowHintsFlag);
    mpViewMenu->ShowScores(mShowScoresFlag);
    mpViewMenu->ShowTiles(mShowTilesFlag);

	mpViewMenu->Enable(!mPauseFlag);
	
	// redraw all menus
	HWND this_window = Handle(); 
	BOOL success = ::DrawMenuBar(this_window);
	ASSERT(success);
}
#endif
