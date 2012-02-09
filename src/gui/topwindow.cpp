// File:    topwindow.cpp
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
    mSwapRect(0, 0, 0, 0),
    mPartial(pGame, 3)
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

	mActiveCellFlag = true;
	mApplication = applicationInstance;
    mAutopauseFlag = false;
    mAutocenterFlag = false;
    mColorAttributeCnt = 1;
    mDragBoardFlag = false;
    mFileName = "Game1";
	mpGame = pGame;
    mIsStartCentered = false;
    mMouseLastX = 0;
    mMouseLastY = 0;
	mMouseUpCnt = 0;
    mPadPixels = 6;
	mPauseFlag = mAutopauseFlag;
    mpPlayMenu = NULL;
	mShowClocksFlag = false;
	mShowGridFlag = true;
	mShowScoresFlag = true;
	mShowTilesFlag = false;
	mSquareGrid = true;
	mStartX = 0;
    mStartY = 0;
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
    int result = mStartX + grid_unit*column;

    return result;
}

int TopWindow::CellY(int row) const {
    unsigned grid_unit = GridUnit();
    int result = mStartY - grid_unit*row;

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
    unsigned tile_cnt = mPartial.CountHand();
    unsigned cell_height = CellHeight();
    unsigned height = tile_cnt*cell_height + 2*mPadPixels;
    if (tile_cnt < mPartial.CountTiles()) {
        // there's room for more
        height += cell_height/2;
	}

	// choose colors for hand area (mHandRect)
	TileIdType active_tile = mPartial.GetActive();
	if (mPartial.IsInHand(active_tile)) {
        // The active tile came from this hand.
		ASSERT(tile_cnt > 0);
		--tile_cnt;
	}
    if (tile_cnt < mPartial.CountTiles()) {
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
    tile_cnt = mPartial.CountSwap();
    unsigned bagHeight = rCanvas.TextHeight();
    height = tile_cnt*cell_height + bagHeight + 3*mPadPixels;
	unsigned played_tile_cnt = mPartial.CountPlayed();
    unsigned stock_cnt = mpGame->CountStock();
    if (tile_cnt < mPartial.CountTiles() 
     && tile_cnt < stock_cnt) {
        // there's room for more
        height += cell_height/2;
	}

    // choose color for swap area (mSwapRect)
	if (mPartial.IsInSwap(active_tile)) {
		ASSERT(tile_cnt > 0);
		--tile_cnt;
	}
	if (mPartial.IsOnBoard(active_tile)) {
		ASSERT(played_tile_cnt > 0);
		--played_tile_cnt;
	}
    if (played_tile_cnt == 0
        && tile_cnt < mPartial.CountTiles()
        && tile_cnt < stock_cnt)
    {
        area_color = COLOR_DARK_GREEN;
    } else { // can't add more tiles to swap area
        area_color = COLOR_BROWN;
    }
    rCanvas.UseColors(area_color, edge_color);

    // draw swap area (mSwapRect)
    top_y = mHandRect.BottomY() - 1;
    mSwapRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    
    String stock_text = plural(stock_cnt, "tile");
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
    Board board = Board(mPartial);
    
    int top_row = 1 + board.NorthMax();
    int bottom_row = -1 - board.SouthMax();
    int right_column = 1 + board.EastMax();
    int left_column = -1 - board.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	unsigned swap_cnt = mPartial.CountSwap();
	TileIdType active_tile = mPartial.GetActive();
	if (mPartial.IsInSwap(active_tile)) {
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

    bool hinted = mPartial.IsHinted(rCell);

    ColorType cellColor = COLOR_DARK_GREEN;
    if (!hinted) {
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
    Rect rect = rCanvas.DrawCell(ulc_y, ulc_x, cellWidth, cellColor, gridColor);
    
	if (rCell.IsStart()) {
	    rCanvas.DrawText(rect, "START");
	}
	
	if (mActiveCellFlag && rCell == mActiveCell) {
	    rCanvas.DrawTarget(rect);
	}

    TileIdType id = mPartial.GetCell(rCell);
    if (id != 0 && !mPartial.IsActive(id)) {
        Tile tile = mPartial.GetTileById(id);
        DrawTile(rCanvas, ulc_y + 1, ulc_x + 1, tile);
    }
    // Draw the active tile later so it won't get obscured.
}

void TopWindow::DrawHandTile(Canvas &rCanvas, int y, int x, Tile const &rTile) {
    Rect rect = DrawTile(rCanvas, y, x, rTile);
        
    TileIdType id = rTile.Id();
    TilePairType pair(id, rect);
    std::pair<TileMapType::iterator, bool> ins;
    ins = mTileMap.insert(pair);
    bool success = ins.second;
    ASSERT(success);
}

void TopWindow::DrawHandTiles(Canvas &rCanvas) {   
    int hand_y = mHandRect.TopY() + mPadPixels;
    int swap_y = mSwapRect.TopY() + mPadPixels;

    unsigned cell_height = CellHeight();
    unsigned cell_width = CellWidth();

    unsigned tile_cnt = mPartial.CountSwap();
    unsigned stock_cnt = mpGame->CountStock();
    if (tile_cnt < mPartial.CountTiles() 
     && tile_cnt < stock_cnt) {
        swap_y += cell_height/2;
    }

    mTileMap.clear();
    
    int active_base_x, active_base_y;
    for (unsigned i = 0; i < mPartial.CountTiles(); i++) {
        Tile tile = mPartial.GetTileByIndex(i);
        TileIdType id = tile.Id();
        int x, y;
        if (mPartial.IsOnBoard(id)) {
            Cell cell = mPartial.LocateTile(id);
            x = CellX(cell.Column());
            y = CellY(cell.Row());
        } else if (mPartial.IsInSwap(id)) {
            int left = mSwapRect.LeftX();
            int width = mSwapRect.Width();
            int pad = (width - cell_width)/2;
            x = left + pad;
            y = swap_y;
            swap_y += cell_height;
        } else {
            ASSERT(mPartial.IsInHand(id)); 
            int left = mHandRect.LeftX();
            int width = mHandRect.Width();
            int pad = (width - cell_width)/2;
            x = left + pad;
            y = hand_y;
            hand_y += cell_height;
        }
        
        if (!mPartial.IsActive(id)) {
            DrawHandTile(rCanvas, y + 1, x + 1, tile);
        } else {
            active_base_y = y + 1;
            active_base_x = x + 1;
        }
    }
    
    TileIdType id = mPartial.GetActive();
    if (id != 0) {
        Tile active_tile = mPartial.GetTileById(id);
        DrawHandTile(rCanvas, active_base_y, active_base_x, active_tile);
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
    TileIdType id = rTile.Id();
    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (mPartial.Contains(id)) {
        tile_color = COLOR_WHITE;
    }
    if (mPartial.IsActive(id)) {
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
    unsigned long grid_unit = GridUnit();
    int column = -400 + (x - mStartX + 400*grid_unit)/grid_unit;
    int row = 400 - (y - mStartY + 400*grid_unit)/grid_unit;
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
            mPartial.Activate(id);
            mDragTileDeltaX = 0;
            mDragTileDeltaY = 0;
			mMouseUpCnt = 0;
		}
        
    } else if (!IsInHandArea(x, y) && !IsInSwapArea(x, y)) {
        // Capture mouse to drag the board
    	CaptureMouse();
        mDragBoardFlag = true;
		mDragBoardPixelCnt = 0;
    }
}

void TopWindow::HandleButtonUp(int x, int y) {	
	HandleMouseMove(x, y);

	if (mDragBoardFlag) {
		if (mDragBoardPixelCnt < 5) {
		    // Trivial drags of less than five pixels 
			// are treated as normal mouse-clicks 
			// which activate/deactivate the cell.
			Cell cell = GetCell(x, y);
			if (mActiveCellFlag && cell == mActiveCell) {
     			mActiveCellFlag = false;
			} else if (mPartial.IsHinted(cell)) {
				mActiveCell = cell;
				mActiveCellFlag = true;
			}
		}
   		StopDragging();
    } else {
        ASSERT(mPartial.GetActive() != 0);        
		ReleaseActiveTile(x, y);
    }
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
	    case IDM_PLAY_PLAY: {
            bool passFlag = false;
			Play(passFlag);
		    break;
        }
	    case IDM_TAKE_BACK:
			TakeBack();
		    break;
	    case IDM_PAUSE:
		    mPauseFlag = !mPauseFlag;
		    ForceRepaint();
		    break;
		case IDM_HINT:
			break;
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
            mIsStartCentered = false;
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
		case IDM_AUTOCENTER:
            mAutocenterFlag = !mAutocenterFlag;
	        UpdateMenus();
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

        case WM_LBUTTONDOWN: // begin left-click
			if (mPauseFlag) {
                mPauseFlag = false;
		        ForceRepaint();
            } else if (!IsDragging()) {
			    POINTS point = MAKEPOINTS(lParam);
                HandleButtonDown(point.x, point.y);
			}
            break;

        case WM_LBUTTONUP: // end left-click
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS point = MAKEPOINTS(lParam);
                    HandleButtonUp(point.x, point.y);
				} else {
					StopDragging();
				}
    		    ForceRepaint();
        	}
  		    break;

        case WM_MOUSEMOVE:
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS point = MAKEPOINTS(lParam);
	                HandleMouseMove(point.x, point.y);
				} else {
					StopDragging();
				}
                ForceRepaint();
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

void TopWindow::HandleMouseMove(int x, int y) {
    int drag_x = x - mMouseLastX;
    int drag_y = y - mMouseLastY;
    mMouseLastX = x;
    mMouseLastY = y;

    if (mDragBoardFlag) {
        mStartX += drag_x;
        mStartY += drag_y;
		mDragBoardPixelCnt += ::abs(drag_x) + ::abs(drag_y);

	} else {
        ASSERT(mPartial.GetActive() != 0);        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
    }
}

void TopWindow::Initialize(CREATESTRUCT const *pCreateStruct) {
    // Object initialization which occurs after the Microsoft Windows window
    // has been created.

	Window::Initialize(pCreateStruct);

    HMENU menu_bar = pCreateStruct->hMenu;

    mpPlayMenu = new PlayMenu(menu_bar, 1);
	ASSERT(mpPlayMenu != NULL);

	mpViewMenu = new ViewMenu(menu_bar, 2);
	ASSERT(mpViewMenu != NULL);

	UpdateMenus();
}

int TopWindow::MessageDispatchLoop(void) {
    int exitCode;

	HACCEL table = GetAcceleratorTable("HOTKEYS");

	while (true) {
        MSG message;
	    HWND anyWindow = NULL;
        BOOL success = ::GetMessage(&message, anyWindow, 0, 0);
        if (success == 0) {   // retrieved a WM_QUIT message
			exitCode = message.wParam;
			break;
		} else if (success == -1) { // error in GetMessage()
            exitCode = -1;
			break;
		}

		int translated = ::TranslateAccelerator(Handle(), table, &message);
		if (!translated) {
            ::TranslateMessage(&message); 
            ::DispatchMessage(&message); 
        } 
    }

	return exitCode;
}

char const *TopWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

void TopWindow::Play(bool passFlag) {
    Move move = Move(mPartial);
    
	char const *reason;
	bool is_legal = mpGame->IsLegalMove(move, reason);
    if (move.IsPass() == passFlag && is_legal) {
        mpGame->FinishTurn(move);
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
            if (mAutocenterFlag) {
                // center the start cell
                mIsStartCentered = false;
                Resize(ClientAreaWidth(), ClientAreaHeight());
            }
        }

        mPartial.Reset();
               
    } else if (!is_legal) { // explain the issue
        Dialog(reason, *this);
	}
	ForceRepaint();
}

void TopWindow::Recenter(unsigned oldHeight, unsigned oldWidth) {
    if (mIsStartCentered) {
        //_startX += (_clientAreaWidth - oldWidth)/2;
        //_startY += (_clientAreaHeight - oldHeight)/2;
    } else if (ClientAreaWidth() > 250 && ClientAreaHeight() > 100) {
        mStartX = ClientAreaWidth()/2; // TODO
        mStartY = ClientAreaHeight()/2;
        mIsStartCentered = true;
    }
}

void TopWindow::ReleaseActiveTile(int x, int y) {
    TileIdType id = mPartial.GetActive(); 
     
	// Determine where the active tile came from.
    bool from_hand = mPartial.IsInHand(id);
    bool from_swap = mPartial.IsInSwap(id);
	ASSERT(!(from_hand && from_swap)); 
	bool from_board = !(from_hand || from_swap);
	Cell from_cell;
    if (from_board) {
        from_cell = mPartial.LocateTile(id);
    }

	// Determine where the active tile was released to.
	bool to_hand = IsInHandArea(x, y);
	bool to_swap = (!to_hand) && IsInSwapArea(x, y); // overlap is possible
	bool to_board = !(to_hand || to_swap);
	Cell to_cell;
	if (to_board) {
		to_cell = GetCell(x, y);
	}

	if (from_hand && to_hand ||
	    from_swap && to_swap ||
	    from_board && to_board && from_cell == to_cell)
	{
		// Trivial drags which don't actually move the tile
		// are treated as normal mouse-clicks which
		// activate/deactivate the tile or play it to the
		// active cell.
		if (mMouseUpCnt == 1) {
			StopDragging(); // deactivates the tile
    		return;
		} else {
		    ASSERT(mMouseUpCnt == 0);
			if (mActiveCellFlag) {
			    to_board = true;
				to_cell = mActiveCell;
			} else {
        		++mMouseUpCnt;
				return;
			}
		}
	}

	if (to_board && mPartial.GetCell(to_cell) != 0) {
		// cell conflict - can't construct move in the normal way
	    Dialog("EMPTY", *this);
    	StopDragging();
		return;
	}

	// move the active tile back to the active hand
	if (from_board) {
        mPartial.BoardToHand();
    } else if (from_swap) {
        mPartial.SwapToHand();
    }

	// move tile from the active hand to its destination
    if (to_board) {
        mPartial.HandToCell(to_cell);
    } else if (to_swap) {
        ASSERT(!from_swap);
        mPartial.HandToSwap();
    }

	// Check whether the move so far is legal.
    Move move_so_far = Move(mPartial);
    char const *reason;
	bool legal = mpGame->IsLegalMove(move_so_far, reason);

	if (!legal && (to_swap || ::strcmp(reason, "FIRST") != 0)) {  
		// It's illegal, even as a partial move:  reverse it.
        if (to_board) {
            mPartial.BoardToHand();
        } else if (to_swap) {
            mPartial.SwapToHand();
        }
     	if (from_board) {
            mPartial.HandToCell(from_cell);
        } else if (from_swap) {
            ASSERT(!from_swap);
            mPartial.HandToSwap();
        }

		// Tell the use why it was illegal.
		if (::strcmp(reason, "START") == 0 && !from_board) {
			reason = "STARTSIMPLE";
	    }
	    Dialog(reason, *this);
	}

    if (mActiveCellFlag && mPartial.GetCell(mActiveCell) != 0) {
        // active cell got filled
        mActiveCellFlag = false;
    }

	StopDragging();
}

void TopWindow::Repaint(void) {
    HWND this_window = Handle();
    PAINTSTRUCT paint_struct;    
    HDC context = ::BeginPaint(this_window, &paint_struct);
    ASSERT(context != NULL);
    
    bool release_me = false;
    unsigned width = ClientAreaWidth();
    unsigned height = ClientAreaHeight();
    Canvas canvas(context, this_window, release_me, width, height);
    
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
	ASSERT(IsDragging());

	mDragBoardFlag = false;
	mPartial.Deactivate();
	::ReleaseCapture();

    ASSERT(mPartial.GetActive() == 0);
	ASSERT(!IsDragging());
	ASSERT(!IsMouseCaptured());
}

// take back an incomplete move
void TopWindow::TakeBack(void) {
    mPartial.Reset();
    ForceRepaint();
}

void TopWindow::UpdateMenus(void) {
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
    bool pass = mPartial.IsPass();
	mpPlayMenu->EnableItems(mPauseFlag, !pass);

	mpPlayMenu->Enable(true);

	// "View" menu
    mpViewMenu->TileSize(tile_size);
    mpViewMenu->ShowClocks(mShowClocksFlag);
    mpViewMenu->ShowGrid(mShowGridFlag);
    mpViewMenu->ShowHints(mShowHintsFlag);
    mpViewMenu->ShowScores(mShowScoresFlag);
    mpViewMenu->ShowTiles(mShowTilesFlag);
    mpViewMenu->Autocenter(mAutocenterFlag);

	mpViewMenu->Enable(!mPauseFlag);
	
	// redraw all menus
	HWND this_window = Handle(); 
	BOOL success = ::DrawMenuBar(this_window);
	ASSERT(success);
}


// inquiry methods

bool TopWindow::IsDragging(void) const {
	bool result = (mDragBoardFlag || mPartial.GetActive() != 0);

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
