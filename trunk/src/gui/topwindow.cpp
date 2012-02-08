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

	mActiveCellFlag = true;
    mActiveTileId = 0;
	mApplication = applicationInstance;
    mAutopauseFlag = false;
    mAutocenterFlag = false;
    mBoard = Board(*pGame);
    mColorAttributeCnt = 1;
    mDragBoardFlag = false;
    mFileName = "Game1";
	mpGame = pGame;
    mHandTiles = Tiles(pGame->ActivePlayer());
	mHintStrength = 3;
    mIsStartCentered = false;
    mMouseLastX = 0;
    mMouseLastY = 0;
	mMouseUpCnt = 0;
    mPadPixels = 6;
	mPauseFlag = mAutopauseFlag;
    mpPlayMenu = NULL;
    mPlayedTileCnt = 0;    
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

void TopWindow::AddValidNextUses(
    Move const &rMove, 
    Tile const &rTile, 
    Cells const &rBase)
{
    Cells::ConstIteratorType i_cell;
    for (i_cell = rBase.begin(); i_cell != rBase.end(); i_cell++) {
        Cell cell = *i_cell;
        if (!mHintedCells.Contains(cell) 
         && IsValidNextStep(rMove, cell, rTile))
        {
      		mHintedCells.Add(cell);
        }
    }
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
    if (played_tile_cnt == 0
        && tile_cnt < mHandTiles.Count()
        && tile_cnt < mpGame->CountStock())
    {
        area_color = COLOR_DARK_GREEN;
    } else { // can't add more tiles to swap area
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
    // TODO - reuse hinted cells if situation is unchanged
	SetHintedCells();

    int top_row = 1 + mBoard.NorthMax();
    int bottom_row = -1 - mBoard.SouthMax();
    int right_column = 1 + mBoard.EastMax();
    int left_column = -1 - mBoard.WestMax();
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

    bool hinted = IsHinted(rCell);

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

    // Draw the active tile last so it isn't obscured.
    Tile const *tile = mBoard.GetCell(rCell);
    if (tile != NULL && tile->Id() != mActiveTileId) {
        DrawTile(rCanvas, ulc_y + 1, ulc_x + 1, *tile);
    }
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
    if (mSwapTiles.Count() < mHandTiles.Count() && mPlayedTileCnt == 0) {
        swap_y += cell_height/2;
    }

    mTileMap.clear();
    unsigned cell_width = CellWidth();
    
    int active_x, active_y;
    Tile active_tile;

    for (unsigned i = 0; i < mHandTiles.Count(); i++) {
        Tile tile = mHandTiles[i];
        int x, y;
        Cell cell;
        if (mBoard.LocateTile(tile, cell)) {
            int row = cell.Row();
            int column = cell.Column();
            x = CellX(column);
            y = CellY(row);
        } else if (mSwapTiles.Contains(tile)) {
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
        
        TileIdType id = tile.Id();
        if (id != mActiveTileId) {
            DrawHandTile(rCanvas, y + 1, x + 1, tile);
        } else {
            active_y = y + 1;
            active_x = x + 1;
            active_tile = tile;
        }
    }
    
    if (mActiveTileId != 0) {
        DrawHandTile(rCanvas, active_y, active_x, active_tile);
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
    unsigned long grid_unit = GridUnit();
    int column = -400 + (x - mStartX + 400*grid_unit)/grid_unit;
    int row = 400 - (y - mStartY + 400*grid_unit)/grid_unit;
    Cell result(row, column);
    
    return result;
}

Move TopWindow::GetMove(void) const {
    Move result;
    
    for (unsigned i = 0; i < mHandTiles.Count(); i++) {
        Tile tile = mHandTiles[i];
        Cell cell;
        if (mBoard.LocateTile(tile, cell)) {
           result.Add(tile, cell);
        } else if (mSwapTiles.Contains(tile)) {
           result.Add(tile);
        }
    }

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
			} else if (IsHinted(cell)) {
				mActiveCell = cell;
				mActiveCellFlag = true;
			}
		}
   		StopDragging();
    } else {
        ASSERT(mActiveTileId != 0);        
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
        ASSERT(mActiveTileId != 0);        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
    }
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
    Move move = GetMove();
    
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
        }

        mBoard = Board(*mpGame);
        mHandTiles = Tiles(mpGame->ActivePlayer());
        mPlayedTileCnt = 0;
        mSwapTiles.MakeEmpty();
       
        // center the start cell
        mIsStartCentered = false;
        Resize(ClientAreaWidth(), ClientAreaHeight());
    }

	if (!is_legal) {
		// explain the issue
        Dialog(reason, *this);
	}
}

void TopWindow::PlayOnCell(Cell const &cell, Tile const &tile) {
    mBoard.PlayOnCell(cell, tile);
    ++mPlayedTileCnt;
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
	// Determine where the active tile came from.
	Cell from_cell;
    bool from_board = mBoard.LocateTileId(mActiveTileId, from_cell);
    bool from_swap = mSwapTiles.ContainsId(mActiveTileId);
	ASSERT(!(from_swap && from_board)); 
	bool from_hand = !(from_board || from_swap);

	// Determine where the active tile was released to.
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

	// move the tile
	if (from_board) {
        UnplayOnCell(from_cell, mActiveTileId);
    } else if (from_swap) {
        mSwapTiles.RemoveTileId(mActiveTileId);
    }

    Tile drag_tile = mHandTiles.FindTile(mActiveTileId);
    if (to_swap) {
        ASSERT(!from_swap);
        mSwapTiles.Add(drag_tile);
	} else if (to_board) {
        PlayOnCell(to_cell, drag_tile);
	}

	// Check whether the move so far is legal.
    Move move_so_far = GetMove();
    char const *reason;
	bool legal = mpGame->IsLegalMove(move_so_far, reason);

	if (legal || (!to_swap && ::strcmp(reason, "FIRST") == 0)) {  
		// It's legal, at least as a partial move.
	    mActiveCellFlag = false;

	} else {
		// It's illegal, even as a partial move:  undo it.
		if (to_swap) {
            mSwapTiles.RemoveTileId(mActiveTileId);
		} else if (to_board) {
            UnplayOnCell(to_cell, mActiveTileId);
		}

        if (from_swap) { // return tile to swap
            mSwapTiles.Add(drag_tile);
		} else if (from_board) { // return tile to board
            PlayOnCell(from_cell, drag_tile);
		}

		// Explain the issue to the user.
		if (::strcmp(reason, "START") == 0 && !from_board) {
			reason = "STARTSIMPLE";
	    }
	    Dialog(reason, *this);
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

void TopWindow::SetHintedCells(void) {
    mHintedCells.MakeEmpty();

    // for mHintStrength == 0, empty cells (from start of turn) are hinted
    int top_row = 1 + mBoard.NorthMax();
    int bottom_row = -1 - mBoard.SouthMax();
    int right_column = 1 + mBoard.EastMax();
    int left_column = -1 - mBoard.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);
    for (int row = top_row; row >= bottom_row; row--) {
        for (int column = left_column; column <= right_column; column++) {
            Cell cell(row, column);
            if (mpGame->HasEmptyCell(cell)) {
                mHintedCells.Add(cell);
            }
        }
    }
    
    if (mHintStrength > 0) {
        // for mHintStrength == 1, only cells connected to the start are hinted
        
        Cells::IteratorType i_cell;
        for (i_cell = mHintedCells.begin(); i_cell != mHintedCells.end(); i_cell++) {
            Cell cell = *i_cell;
            if (!mBoard.ConnectsToStart(cell)) {
                mHintedCells.erase(i_cell);
            }
        }
    }

    if (mHintStrength > 1) {
        // for mHintStrength == 2, only cells usable with available tiles are hinted
        // for mHintStrength == 3, only cells usable with active tile are hinted
        Cells base = mHintedCells;
        mHintedCells.MakeEmpty();
	    Move move = GetMove();

	    for (unsigned i = 0; i < mHandTiles.Count(); i++) {
            Tile tile = mHandTiles[i];
	        TileIdType id = tile.Id();

            bool include_tile = (!mBoard.ContainsId(id) || id == mActiveTileId);
            if (mHintStrength == 3 && mActiveTileId != 0) {
                include_tile = (mActiveTileId == id);
            }
            
		    if (include_tile) {
                 AddValidNextUses(move, tile, base);
            }
		}
    }
}

void TopWindow::StopDragging(void) {
	mDragBoardFlag = false;
	mActiveTileId = 0;
	::ReleaseCapture();

	ASSERT(!IsDragging());
	ASSERT(!IsMouseCaptured());
}

// take back an incomplete move
void TopWindow::TakeBack(void) {
    mBoard = Board(*mpGame);
    mPlayedTileCnt = 0;
    mSwapTiles.MakeEmpty();

    ForceRepaint();
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
    mpViewMenu->Autocenter(mAutocenterFlag);

	mpViewMenu->Enable(!mPauseFlag);
	
	// redraw all menus
	HWND this_window = Handle(); 
	BOOL success = ::DrawMenuBar(this_window);
	ASSERT(success);
}


// inquiry methods

bool TopWindow::IsDragging(void) const {
	bool result = (mDragBoardFlag || mActiveTileId != 0);

	return result;
}

bool TopWindow::IsHinted(Cell const &rCell) const {
	bool result = mHintedCells.Contains(rCell);
	
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

#if 0
bool TopWindow::IsInBounds(Cell const &rCell) const {
	bool result = true;

	int n = rCell.Row();
    if (n > 1 + mBoard.NorthMax()) {
        result = false;
    }
    if (n < -1 - mBoard.SouthMax()) {
        result = false;
    }

    int e = rCell.Column();
    if (e > 1 + mBoard.EastMax()) {
        result = false;
    }
    if (e < -1 - mBoard.WestMax()) {
        result = false;
    }

	return result;
}
#endif

bool TopWindow::IsInTile(int x, int y) const {
	TileIdType id = GetTileId(x, y);
	bool result = (id != 0);

	return result;
}

bool TopWindow::IsPaused(void) const {
	bool result = mPauseFlag;

	return result;
}

bool TopWindow::IsValidNextStep(Move const &base, Cell const &rCell, Tile const &rTile) const {
	// Check whether a hypothetical next step would be legal.
	Move move = base;
	move.Add(rTile, rCell);
    char const *reason;
	bool result = mpGame->IsLegalMove(move, reason);

	if (!result && ::strcmp(reason, "FIRST") == 0) {  
		// legal as a partial move.
	    result = true;
	}

	return result;
}

#endif
