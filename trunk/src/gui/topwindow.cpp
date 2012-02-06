// File:    topwindow.cpp
// Purpose: TopWindow class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "cell.hpp"
#include "cells.hpp"
#include "game.hpp"
#include "gui/canvas.hpp"
#include "gui/color.hpp"
#include "gui/dialog.hpp"
#include "gui/resource.hpp"
#include "gui/topwindow.hpp"
#include "gui/windowclass.hpp"
#include "move.hpp"
#include "player.hpp"

// static data of the class

WindowClass *TopWindow::mspClass = NULL;

// message handler (callback) for top window
static TopWindow *spNewlyCreatedTopWindow = NULL;
static LRESULT CALLBACK topMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
    TopWindow *window;
	if (message == WM_CREATE && spNewlyCreatedTopWindow != NULL) {
		window = spNewlyCreatedTopWindow;
		spNewlyCreatedTopWindow = NULL;
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

// lifecycle

TopWindow::TopWindow(HINSTANCE applicationInstance, Game *pGame):
    mHandRect(0, 0, 0, 0),
    mSwapRect(0, 0, 0, 0)
{
	ASSERT(Handle() == 0);

	char const *className = "TOPWINDOW";
    if (mspClass == NULL) {
		// constructing first instance:  create a Microsoft Windows window class
		WNDPROC messageHandler = &topMessageHandler;
		mspClass = new WindowClass(applicationInstance, messageHandler, className);
		mspClass->RegisterClass();
	}
	ASSERT(mspClass != NULL);

	// Make this TopWindow object accessable to its message handler before WM_CREATE.
    ASSERT(spNewlyCreatedTopWindow == NULL);
	spNewlyCreatedTopWindow = this;

    ASSERT(pGame != NULL);
	mpGame = pGame;

    mActiveTileId = 0;
	mApplication = applicationInstance;
    mAutopauseFlag = false;
    mBoard = Board(*pGame);
    mColorAttributeCnt = 1;
    mDragBoardFlag = false;
    mFileName = "Game1";
	mpGame = pGame;
    mHandTiles = Tiles(pGame->ActivePlayer());
    mMouseLastX = 0;
    mMouseLastY = 0;
	mMouseUpCnt = 0;
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

// misc

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

void TopWindow::DrawActivePlayer(Canvas &rCanvas) {
    // draw header
    int top_y = mPadPixels;
    int left_x = mPadPixels;
    Player active_player = mpGame->ActivePlayer();
    ColorType area_color = COLOR_BLACK;
    bool left = true;
    Rect header_rect = DrawPlayerHeader(rCanvas, top_y, left_x, active_player, area_color, left);
    left_x = header_rect.LeftX();
    unsigned width = header_rect.Width();
    
    // calculate height of hand area (mHandRect)
    unsigned tile_cnt = mHandTiles.Count() - mSwapTiles.Count() - mPlayedTileCnt;
    unsigned cell_height = CellHeight();
    unsigned height = tile_cnt*cell_height + 2*mPadPixels;
    if (tile_cnt < mHandTiles.Count()) {
        height += cell_height/2;
	}

	bool from_board = false;
	bool from_swap = false;
	bool from_hand = false;
	if (mActiveTileId != 0) {
		if (mSwapTiles.ContainsId(mActiveTileId)) {
			from_swap = true;
		} else if (mBoard.ContainsId(mActiveTileId)) {
			from_board = true;
		} else {
			from_hand = true;
		}
	}

	// choose colors for hand area (mHandRect)
	if (from_hand) {
		ASSERT(tile_cnt > 0);
		--tile_cnt;
	}
    if (tile_cnt < mHandTiles.Count()) {
        area_color = COLOR_DARK_GREEN;
    } else { // hand is full
        area_color = COLOR_BROWN;
    }
    ColorType edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);

    // draw hand area (mHandRect)
    top_y = header_rect.BottomY() - 1;
    mHandRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    left_x = mHandRect.LeftX();
    width = mHandRect.Width();
    

    // calculate height of swap area (mSwapRect)
    tile_cnt = mSwapTiles.Count();
    unsigned bagHeight = rCanvas.TextHeight();
    height = tile_cnt*cell_height + bagHeight + 3*mPadPixels;
    if (mPlayedTileCnt == 0 && tile_cnt < mHandTiles.Count()) {
        height += cell_height/2;
	}

    // choose color for swap area (mSwapRect)
	if (from_swap) {
		ASSERT(tile_cnt > 0);
		--tile_cnt;
	}
	unsigned played_tile_cnt = mPlayedTileCnt;
	if (from_board) {
		ASSERT(played_tile_cnt > 0);
		--played_tile_cnt;
	}
    if (played_tile_cnt == 0 && tile_cnt < mHandTiles.Count()) {
        area_color = COLOR_DARK_GREEN;
    } else { // can't add tiles to swap area
        area_color = COLOR_BROWN;
    }
    rCanvas.UseColors(area_color, edge_color);

    // draw swap area (mSwapRect)
    top_y = mHandRect.BottomY() - 1;
    mSwapRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    
    unsigned stock = mpGame->CountStock();
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

	unsigned swap_cnt = mSwapTiles.Count();

	if (mActiveTileId != 0 && mSwapTiles.ContainsId(mActiveTileId)) {
	    ASSERT(swap_cnt > 0);
		--swap_cnt;
	}

    for (int row = top_row; row >= bottom_row; row--) {
        if (CellY(row) > (int)ClientAreaHeight()) {
            break;
        }
        for (int column = left_column; column <= right_column; column++) {
            if (CellX(column) > (int)ClientAreaWidth()) {
                break;
            }
            Cell cell(row, column);
            DrawCell(rCanvas, cell, swap_cnt);
        }
    }
}

void TopWindow::DrawCell(Canvas &rCanvas, Cell const &rCell, unsigned swapCnt) {
    int row = rCell.Row();
    int column = rCell.Column();
    int ulc_x = CellX(column);
    int ulc_y = CellY(row);

    Cells done;
    bool connected = mBoard.ConnectsToOrigin(rCell);

    ColorType cellColor = COLOR_DARK_GREEN;
    if (!connected) {
        cellColor = COLOR_BLACK;
    } else if (swapCnt > 0) {
        cellColor = COLOR_BROWN;
    }
    ColorType gridColor = cellColor;
    if (mShowGridFlag) {
        if (swapCnt == 0) {
            gridColor = COLOR_GREEN;
        } else {
            gridColor = COLOR_YELLOW;
        }
    }

    unsigned cellWidth = CellWidth();
    rCanvas.DrawCell(ulc_y, ulc_x, cellWidth, cellColor, gridColor);
    
    Tile const *tile = mBoard.GetCell(rCell);
    if (tile != NULL && tile->Id() != mActiveTileId) {
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
    for (unsigned i = 0; i < mHandTiles.Count(); i++) {
        Tile tile = mHandTiles[i];
        int x, y;
        Cell cell;
        if (mBoard.LocateTile(tile, cell)) {
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

    Players other_players = mpGame->InactivePlayers();
    Players::ConstIteratorType i_player;
    int right_x = ClientAreaWidth() - mPadPixels;
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

        for (unsigned i = 0; i < hand_tiles.Count(); i++) {
            Tile tile = hand_tiles[i];
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

void TopWindow::DrawPaused(Canvas &rCanvas) {
	ASSERT(IsPaused());
    ColorType bg_color = COLOR_BLACK;
    ColorType text_color = COLOR_WHITE;
    rCanvas.UseColors(bg_color, text_color);

    int x = 0;
    int y = 0;
    Rect clientArea(y, x, ClientAreaWidth(), ClientAreaHeight());
    rCanvas.DrawText(clientArea, "The game is paused.  Click here to proceed.");
        
    int top_y = mPadPixels;
    int left_x = mPadPixels;
    Player active_player = mpGame->ActivePlayer();
    bool left = true;
    DrawPlayerHeader(rCanvas, top_y, left_x, active_player, bg_color, left);
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
    Player active_player = mpGame->ActivePlayer();
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
    if (rTile.Id() == mActiveTileId) {
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

TileIdType TopWindow::GetTileId(int x, int y) const {
	TileIdType result = 0;

	TileMapType::const_iterator i_tile;
    for (i_tile = mTileMap.begin(); i_tile != mTileMap.end(); i_tile++) {
        Rect rect = i_tile->second;
        if (rect.Contains(x, y)) {
            result = i_tile->first;
			break;
        }
    }

    return result;
}

unsigned TopWindow::GridUnit(void) const {
    unsigned result = mTileWidth - 1;
    if (mShowGridFlag) {
        result = mTileWidth;
    }

    return result;
}

void TopWindow::HandleButtonDown(int x, int y) {
	ASSERT(!IsPaused());
    mMouseLastX = x;
    mMouseLastY = y;

	TileIdType id = GetTileId(x, y);
    if (id != 0) {
        if (!IsMouseCaptured()) {
            // Capture mouse to drag the tile
            CaptureMouse();
            mActiveTileId = id;
            mDragTileDeltaX = 0;
            mDragTileDeltaY = 0;
			mMouseUpCnt = 0;
		}
        
    } else if (!IsInHandArea(x, y) && !IsInSwapArea(x, y)) {
        // Capture mouse to drag the board
    	CaptureMouse();
        mDragBoardFlag = true;
    }
}

void TopWindow::HandleButtonUp(int x, int y) {	
    int drag_x = x - mMouseLastX;
    int drag_y = y - mMouseLastY;
    mMouseLastX = x;
    mMouseLastY = y;

	if (mDragBoardFlag) {
        ASSERT(mActiveTileId == 0);
        mOriginX += drag_x;
        mOriginY += drag_y;
        StopDragging();
        
    } else {
        ASSERT(mActiveTileId != 0);        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
		ReleaseActiveTile(x, y);
    }
    
	ForceRepaint();
}


void TopWindow::HandleDragMouse(int x, int y) {
    int dragX = x - mMouseLastX;
    int dragY = y - mMouseLastY;
    mMouseLastX = x;
    mMouseLastY = y;

    if (mDragBoardFlag) {
        mOriginX += dragX;
        mOriginY += dragY;
    } else {
        ASSERT(mActiveTileId != 0);        
        mDragTileDeltaX += dragX;
        mDragTileDeltaY += dragY;
    }
    ForceRepaint();
}

void TopWindow::HandleMenuCommand(int command) {
	ASSERT(mpPlayMenu != NULL);
	ASSERT(mpViewMenu != NULL);

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
			HWND this_window = Handle();
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
            Resize(ClientAreaWidth(), ClientAreaHeight());
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
        case IDM_RULES: {
	   	    Dialog("RULES", *this);
            break;
		}
        case IDM_ABOUT: {
	   	    Dialog("ABOUT", *this);
            break;
		}
        case IDM_WARRANTY: {
	   	    Dialog("WARRANTY", *this);
            break;
		}

		default:
			ASSERT(false);
    }
}

LRESULT TopWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
        case WM_COMMAND: { // menu command
	        int command = LOWORD(wParam);
            HandleMenuCommand(command);
            break;
  	    }

        case WM_CREATE: { // initialize window
		    CREATESTRUCT *p_create_struct = (CREATESTRUCT *)lParam;
            Initialize(p_create_struct);
            break;
        }

        case WM_LBUTTONDOWN: // left-click
			if (mPauseFlag) {
                mPauseFlag = false;
		        ForceRepaint();
            } else if (!IsDragging()) {
			    POINTS point = MAKEPOINTS(lParam);
                HandleButtonDown(point.x, point.y);
			}
            break;

        case WM_LBUTTONUP:
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS point = MAKEPOINTS(lParam);
                    HandleButtonUp(point.x, point.y);
				} else {
					StopDragging();
				}
			}
  		    break;

        case WM_MOUSEMOVE:
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS point = MAKEPOINTS(lParam);
                    HandleDragMouse(point.x, point.y);
				} else {
					StopDragging();
				}
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
    // Object initialization which happens after the Microsoft Windows window
    // has been created.

	Window::Initialize(pCreateStruct);

    HMENU menu_bar = pCreateStruct->hMenu;

    mpPlayMenu = new PlayMenu(menu_bar, 1);
	ASSERT(mpPlayMenu != NULL);

	mpViewMenu = new ViewMenu(menu_bar, 2);
	ASSERT(mpViewMenu != NULL);

	UpdateMenus();
}

char const *TopWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

void TopWindow::Play(bool passFlag) {
    Move pl;
    
    for (unsigned i = 0; i < mHandTiles.Count(); i++) {
        Tile tile = mHandTiles[i];
        Cell cell;
        if (mBoard.LocateTile(tile, cell)) {
           pl.Add(tile, cell);
        } else if (mSwapTiles.Contains(tile)) {
           pl.Add(tile);
        }
    }
    
    if (pl.IsPass() == passFlag && mpGame->IsLegalMove(pl)) {
        mpGame->FinishTurn(pl);
        if (mpGame->IsOver()) {
            mShowClocksFlag = true;
            mShowScoresFlag = true;
            mShowTilesFlag = true;
            mpGame->GoingOutBonus();
        } else {
            mpGame->ActivateNextPlayer();
            if (mAutopauseFlag) {
                mPauseFlag = true;
            }
        }

        mBoard = Board(*mpGame);
        mHandTiles = Tiles(mpGame->ActivePlayer());
        mPlayedTileCnt = 0;
        mSwapTiles.MakeEmpty();
       
        // center the origin
        mOriginIsCentered = false;
        Resize(ClientAreaWidth(), ClientAreaHeight());
    }
}

void TopWindow::PlayOnCell(Cell const &cell, Tile const &tile) {
    mBoard.PlayOnCell(cell, tile);
    ++mPlayedTileCnt;
}


void TopWindow::Recenter(unsigned oldHeight, unsigned oldWidth) {
    if (mOriginIsCentered) {
        //_originX += (_clientAreaWidth - oldWidth)/2;
        //_originY += (_clientAreaHeight - oldHeight)/2;
    } else if (ClientAreaWidth() > 250 && ClientAreaHeight() > 100) {
        mOriginX = ClientAreaWidth()/2; // TODO
        mOriginY = ClientAreaHeight()/2;
        mOriginIsCentered = true;
    }
}

void TopWindow::ReleaseActiveTile(int x, int y) {
	// Determine where the active tile got picked up.
	Cell from_cell;
    bool from_board = mBoard.LocateTileId(mActiveTileId, from_cell);
    bool from_swap = mSwapTiles.ContainsId(mActiveTileId);
	ASSERT(!(from_swap && from_board)); 
	bool from_hand = !(from_board || from_swap);

	// Determine where the active tile got released.
	Cell to_cell;
	bool to_hand = IsInHandArea(x, y);
	bool to_swap = IsInSwapArea(x, y);
	ASSERT(!(to_hand && to_swap)); 
	bool to_board = !(to_hand || to_swap);
	if (to_board) {
		to_cell = GetCell(x, y);
	}

	if (from_hand && to_hand ||
	    from_swap && to_swap ||
	    from_board && to_board && from_cell == to_cell)
	{
		// trivial cases
		++mMouseUpCnt;

		if (mMouseUpCnt > 1) {
			StopDragging();
 		}
		return;
	}

	if (from_board) {
        UnplayOnCell(from_cell, mActiveTileId);
    } else if (from_swap) {
        mSwapTiles.RemoveTileId(mActiveTileId);
    }

    Tile drag_tile = mHandTiles.FindTile(mActiveTileId);

    if (to_swap) {
        if (mPlayedTileCnt == 0) {
	        // legal addition to swap
            mSwapTiles.Add(drag_tile);
		} else {
		    ASSERT(!from_swap);
		    if (from_board) { // return tile to board
                PlayOnCell(from_cell, drag_tile);
		    }

			// explain the issue
			Dialog("MIXED", *this);
		}

	} else if (to_board) { // dragged to board
        Move move;
        move.Add(drag_tile, to_cell);
        if (mSwapTiles.Count() == 0 && mBoard.IsLegalMove(move)) {
		    // a legal move (by itself)
            PlayOnCell(to_cell, drag_tile);

		} else {
		    if (from_board) { // return tile to board
                PlayOnCell(from_cell, drag_tile);
		    } else if (from_swap) { // return tile to swap
                mSwapTiles.Add(drag_tile);
		    }

			// explain the issue
			if (mSwapTiles.Count() > 0) {
			    Dialog("MIXED", *this);
			} else if (!mBoard.ConnectsToOrigin(to_cell)) {
			    Dialog("CONNECTED", *this);
			} else {
			    Dialog("COMPATIBLE", *this);
			}
		}
	}
	StopDragging();
}

void TopWindow::Repaint(void) {
    HWND this_window = Handle();
    PAINTSTRUCT paint_struct;    
    HDC context = ::BeginPaint(this_window, &paint_struct);
    ASSERT(context != NULL);
    
    bool release_me = false;
    Canvas canvas(context, this_window, release_me, ClientAreaWidth(), ClientAreaHeight());
    
    if (mPauseFlag) {
		DrawPaused(canvas);

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
    unsigned old_height = ClientAreaHeight();
    unsigned old_width = ClientAreaWidth();
    SetClientArea(clientAreaWidth, clientAreaHeight);
    Recenter(old_height, old_width);
    ForceRepaint();
}

void TopWindow::StopDragging(void) {
	mDragBoardFlag = false;
	mActiveTileId = 0;
	::ReleaseCapture();

	ASSERT(!IsDragging());
	ASSERT(!IsMouseCaptured());
}

void TopWindow::UnplayOnCell(Cell const &cell, TileIdType id) {
	ASSERT(!mBoard.HasEmptyCell(cell));

    mBoard.MakeEmpty(cell);
    --mPlayedTileCnt;

	ASSERT(mBoard.HasEmptyCell(cell));
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


// inquiry

bool TopWindow::IsDragging(void) const {
	bool result = (mDragBoardFlag || mActiveTileId != 0);

	return result;
}

bool TopWindow::IsInHandArea(int x, int y) const {
	bool result = mHandRect.Contains(x, y);

	return result;
}

bool TopWindow::IsInSwapArea(int x, int y) const {
	bool result = mSwapRect.Contains(x, y);

	return result;
}

bool TopWindow::IsInTile(int x, int y) const {
	TileIdType id = GetTileId(x, y);
	bool result = (id != 0);

	return result;
}

bool TopWindow::IsPaused(void) const {
	bool result = mPauseFlag;

	return result;
}

#endif
