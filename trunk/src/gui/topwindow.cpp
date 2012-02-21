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

#include "gui/canvas.hpp"
#include "gui/handbox.hpp"
#include "gui/hintbox.hpp"
#include "gui/menubar.hpp"
#include "gui/parmbox1.hpp"
#include "gui/parmbox2.hpp"
#include "gui/parmbox3.hpp"
#include "gui/resource.hpp"
#include "gui/tilebox.hpp"
#include "gui/topwindow.hpp"
#include "gui/win_types.hpp"
#include "gui/windowclass.hpp"
#include "gui/yesno.hpp"
#include "hand.hpp"
#include "move.hpp"
#include "strings.hpp"

// static data of the class

WindowClass *TopWindow::mspClass = NULL;

static const ColorType GlyphColors[9] = {
	COLOR_BLACK,      COLOR_RED,       COLOR_DARK_BLUE, 
    COLOR_DARK_GREEN, COLOR_PURPLE,    COLOR_BROWN, 
    COLOR_DARK_GRAY,  COLOR_PINK,      COLOR_LIGHT_BLUE
};

// message handler (callback) for top window
static LRESULT CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParam,
	LPARAM lParam)
{
	ASSERT(windowHandle != NULL);
    TopWindow *window = (TopWindow *)Window::Lookup(windowHandle);

	LRESULT result;
	if (window == NULL) { // unknown window
		// invoke default message handler
		result = Win::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(window->Handle() == windowHandle);
        result = window->HandleMessage(message, wParam, lParam);
	}

	return result;
}

// lifecycle

TopWindow::TopWindow(HINSTANCE applicationInstance, Game *pGame):
    mHandRect(0, 0, 0, 0),
    mMouseLast(0, 0),
    mPartial(pGame),
    mStartCell(0, 0),
    mSwapRect(0, 0, 0, 0)
{
	ASSERT(Handle() == 0);

	char const *className = "TOPWINDOW";
    if (mspClass == NULL) {
		// constructing first instance:  create a Microsoft Windows window class
		WNDPROC messageHandler = &message_handler;
		mspClass = new WindowClass(applicationInstance, messageHandler, className);
		mspClass->RegisterClass();
	}
	ASSERT(mspClass != NULL);

	// Make this TopWindow object accessable to its message handler before WM_CREATE.
    ASSERT(mspNewlyCreatedWindow == NULL);
	mspNewlyCreatedWindow = this;

    mColorAttributeCnt = 1;
    mDragBoardFlag = false;
	mpFileMenu = NULL;
	mpGame = pGame;
	mGameStyle = GAME_STYLE_PRACTICE;
	mInitialNewGame = (pGame == NULL);
    mIsStartCentered = false;
	mMouseUpCnt = 0;
    mPadPixels = 6;
    mpPlayMenu = NULL;
	mShowGridFlag = false;
	mShowScoresFlag = true;
	mShowTilesFlag = false;
	mTargetCellFlag = false;
    mpViewMenu = NULL;

    mAutocenterFlag = (mGameStyle == GAME_STYLE_CHALLENGE);
    mAutopauseFlag = (mGameStyle == GAME_STYLE_CHALLENGE);
	mShowClocksFlag = (mGameStyle == GAME_STYLE_CHALLENGE);

    SetTileWidth(IDM_LARGE_TILES);
	if (mpGame != NULL && !mAutopauseFlag) {
		mpGame->StartClock();
	}

	HWND desktop_handle = Win::GetDesktopWindow();
	RECT rect;
	Win::GetWindowRect(desktop_handle, &rect);
	Rect desktop_bounds(rect);

	// create Microsoft Windows window
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	int height = int(0.8*double(desktop_bounds.Height()));
	int width = int(0.8*double(desktop_bounds.Width()));
	int x = width/8;
	int y = height/8;
	HWND parent = NULL;
	HMENU menu = NULL;
	LPVOID parameters = NULL;
    HWND handle = Win::CreateWindow(className, Name(), windowStyle, x, y, 
                             width, height, parent, menu, applicationInstance, 
                             parameters);
    ASSERT(Handle() == handle);

	// wait for message_handler() to receive a message with this handle
}

TopWindow::~TopWindow(void) {
	delete mpFileMenu;
	delete mpPlayMenu;
	delete mpViewMenu;
}

void TopWindow::Initialize(CREATESTRUCT const *pCreateStruct) {
    // Object initialization which occurs after the Microsoft Windows window
    // has been created and has received WM_CREATE.

	ASSERT(mpFileMenu == NULL);
	ASSERT(mpPlayMenu == NULL);
	ASSERT(mpViewMenu == NULL);

	Window::Initialize(pCreateStruct);

	MenuBar bar(pCreateStruct);
#if 0
    HMENU menu_bar = pCreateStruct->hMenu;

	mpFileMenu = new FileMenu(menu_bar, 0);
	ASSERT(mpFileMenu != NULL);

	mpPlayMenu = new PlayMenu(menu_bar, 1);
	ASSERT(mpPlayMenu != NULL);

	mpViewMenu = new ViewMenu(menu_bar, 2);
	ASSERT(mpViewMenu != NULL);
#endif
	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);
	UpdateMenus();
}


// misc methods

PCntType TopWindow::CellHeight(void) const {
    PCntType result = TileHeight();
    if (mShowGridFlag) {
        result += 2; // add room for two grid lines
    }

    return result;
}

PCntType TopWindow::CellWidth(void) const {
    PCntType result = mTileWidth;
    if (mShowGridFlag) {
        result += 2; // add room for two grid lines
    }

	ASSERT(::is_even(result));
    return result;
}

LogicalXType TopWindow::CellX(IndexType column) const {
    PCntType grid_unit = GridUnitX();
    LogicalXType result = mStartCell.X() + grid_unit*column;

    return result;
}

LogicalYType TopWindow::CellY(IndexType row) const {
    PCntType grid_unit = GridUnitY();
    LogicalYType result = mStartCell.Y() - grid_unit*row;

    return result;
}

String TopWindow::ClockText(Hand &rHand) const {
	ASSERT(mpGame != NULL);

	int seconds = mpGame->Seconds(rHand);

	// convert to minutes and seconds
	bool minus_sign;
	if (seconds >= 0) {
		minus_sign = false;
	} else {
		minus_sign = true;
		seconds = -seconds;
	}

	unsigned minutes, tens_of_seconds;
	minutes = seconds / SECONDS_PER_MINUTE;
	seconds -= minutes*SECONDS_PER_MINUTE;
    tens_of_seconds = seconds / 10;
	seconds -= tens_of_seconds*10;

	ASSERT(seconds >= 0);
	ASSERT(seconds <= 9);
	ASSERT(tens_of_seconds >= 0);
	ASSERT(tens_of_seconds <= 5);
	ASSERT(minutes >= 0);

	String result;
	if (minus_sign) {
		result += "-";
	}
	result += String(minutes) + ":" 
		   + String(tens_of_seconds) + String(seconds);

	return result;
}

void TopWindow::DrawActiveHand(Canvas &rCanvas) {
    // draw header
	ASSERT(mpGame != NULL);

    LogicalYType top_y = mPadPixels;
    LogicalXType left_x = mPadPixels;
    Hand active_hand = Hand(*mpGame);
    ColorType area_color = COLOR_BLACK;
    bool left = true;
    Rect header_rect = DrawHandHeader(rCanvas, top_y, left_x, active_hand, area_color, left);
    left_x = header_rect.LeftX();
    PCntType width = header_rect.Width();
    
    // calculate height of hand area (mHandRect)
    unsigned tile_cnt = mPartial.CountHand();
    PCntType cell_height = CellHeight();
    PCntType height = tile_cnt*cell_height + 2*mPadPixels;
    if (tile_cnt < mPartial.CountTiles()) {
        // show that there's room for more
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
    PCntType bagHeight = rCanvas.TextHeight();
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
    LogicalYType y = mSwapRect.BottomY() - mPadPixels - bagHeight;
    Rect bounds(y, left_x, width, bagHeight);
    rCanvas.DrawText(bounds, stock_text);
}

void TopWindow::DrawBlankTile(Canvas &rCanvas, Point const &rCenter, bool oddFlag) {
	PCntType height = TileHeight();
    ColorType tile_color = COLOR_LIGHT_GRAY;
    rCanvas.DrawBlankTile(rCenter, mTileWidth, height, tile_color, oddFlag);
}

void TopWindow::DrawBoard(Canvas &rCanvas, unsigned showLayer) {
    Board board = Board(mPartial);

	int fringe = 1;
	if (Cell::Grid() == GRID_HEX) {
		fringe = 2;
	}

    IndexType top_row = fringe + board.NorthMax();
    IndexType bottom_row = -fringe - board.SouthMax();
    IndexType right_column = fringe + board.EastMax();
    IndexType left_column = -fringe - board.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	unsigned swap_cnt = mPartial.CountSwap();
	TileIdType active_tile = mPartial.GetActive();
	if (mPartial.IsInSwap(active_tile)) {
	    ASSERT(swap_cnt > 0);
		--swap_cnt;
	}

	if (!mTargetCellFlag && mPartial.CountHinted() == 1) {
		mTargetCell = mPartial.FirstHinted();
		mTargetCellFlag = true;
	}

    for (IndexType row = top_row; row >= bottom_row; row--) {
        if (CellY(row) > (LogicalYType)ClientAreaHeight()) {
            break;
        }
        for (IndexType column = left_column; column <= right_column; column++) {
            if (CellX(column) > (LogicalXType)ClientAreaWidth()) {
                break;
            } else if (Cell::IsValid(row, column)) {
                Cell cell(row, column);
				bool hinted = mPartial.IsHinted(cell);
				bool empty = mPartial.IsEmpty(cell);
				unsigned layer = 0;
				if (hinted || empty) {
					layer = 1;
				}
				if (layer == showLayer) {
                    DrawCell(rCanvas, cell, swap_cnt);
				}
			}
        }
    }
}

void TopWindow::DrawCell(Canvas &rCanvas, Cell const &rCell, unsigned swapCnt) {
	IndexType row = rCell.Row();
    IndexType column = rCell.Column();
	LogicalXType center_x = CellX(column);
    LogicalYType center_y = CellY(row);
    Point center(center_x, center_y);

    bool hinted = mPartial.IsHinted(rCell);
	bool used = !mPartial.IsEmpty(rCell);

    ColorType cell_color = COLOR_BLACK;
	ColorType feature_color = COLOR_LIGHT_GRAY;
	if (used) {
		cell_color = COLOR_DARK_BLUE;
		feature_color = COLOR_LIGHT_BLUE;
	} else if (hinted) {
        if (swapCnt == 0) {
            cell_color = COLOR_DARK_GREEN;
		    feature_color = COLOR_LIGHT_GREEN;
		} else {
            cell_color = COLOR_BROWN;
		    feature_color = COLOR_YELLOW;
		}
    }
	ColorType grid_color = cell_color;
    if (mShowGridFlag) {
		grid_color = feature_color;
	}

    PCntType cell_height = CellHeight();
    PCntType cell_width = CellWidth();
	bool odd_flag = rCell.IsOdd();
    Rect rect = rCanvas.DrawCell(center, cell_width, cell_height, cell_color, grid_color, odd_flag);
    
	// draw cell features
	rCanvas.UseColors(cell_color, feature_color);
	if (rCell.IsStart()) {
	    rCanvas.DrawText(rect, "START", "S");
	}
	
	if (mTargetCellFlag && rCell == mTargetCell) {
	    rCanvas.DrawTarget(rect);
	}

    // Draw the active tile later (not now) so it won't get obscured.
	TileIdType id = mPartial.GetCell(rCell);
    if (id != 0 && !mPartial.IsActive(id)) {
        Tile tile = mPartial.GetTileById(id);
        DrawTile(rCanvas, center, tile, odd_flag);
    }
}

Rect TopWindow::DrawHandHeader(
    Canvas &rCanvas, 
    LogicalYType topY, 
    LogicalXType leftRight, 
    Hand &rHand, 
    ColorType areaColor, 
    bool leftFlag)
{
    ASSERT(mpGame != NULL);

    unsigned cell_width = CellWidth();

    String name_text = rHand.Name();
    Hand active_hand = Hand(*mpGame);
    if (name_text == active_hand.Name()) {
        name_text += "'s turn";
    }
    unsigned w = rCanvas.TextWidth(name_text);
    unsigned width = (cell_width > w) ? cell_width : w;

    String scoreText;
    if (mShowScoresFlag) {
        unsigned score = rHand.Score();
        scoreText = plural(score, "point");
        w = rCanvas.TextWidth(scoreText);
        if (w > width) {
            width = w;
        }
    }
    
    String clock_text;
    if (mShowClocksFlag) {
        clock_text = ClockText(rHand);
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

void TopWindow::DrawHandTile(
    Canvas &rCanvas,
    Point const &rCenter,
    Tile const &rTile,
	bool oddFlag)
{
    Rect rect = DrawTile(rCanvas, rCenter, rTile, oddFlag);
        
    TileIdType id = rTile.Id();
    TilePairType pair(id, rect);
    TileInsResultType ins_result = mTileMap.insert(pair);
    bool success = ins_result.second;
    ASSERT(success);
}

void TopWindow::DrawHandTiles(Canvas &rCanvas) {
	ASSERT(mpGame != NULL);

    PCntType cell_height = CellHeight();
    LogicalYType hand_y = mHandRect.TopY() + mPadPixels + cell_height/2;
    LogicalYType swap_y = mSwapRect.TopY() + mPadPixels + cell_height/2;

    unsigned tile_cnt = mPartial.CountSwap();
    unsigned stock_cnt = mpGame->CountStock();
    if (tile_cnt < mPartial.CountTiles()  && tile_cnt < stock_cnt) {
        swap_y += cell_height/2;
    }

    mTileMap.clear();
    
    Point active_base(0, 0);
	bool active_odd = false;
    for (unsigned i = 0; i < mPartial.CountTiles(); i++) {
        Tile tile = mPartial.GetTileByIndex(i);
        TileIdType id = tile.Id();
	    bool odd_flag;
        LogicalXType x;
        LogicalYType y;
        if (mPartial.IsOnBoard(id)) {
            Cell cell = mPartial.LocateTile(id);
			IndexType row = cell.Row();
			IndexType column = cell.Column();
			odd_flag = (is_odd(column) != is_odd(row));
            x = CellX(column);
            y = CellY(row);
        } else if (mPartial.IsInSwap(id)) {
			odd_flag = false;
            x = mSwapRect.CenterX();
            y = swap_y;
            swap_y += cell_height;
        } else {
            ASSERT(mPartial.IsInHand(id)); 
			odd_flag = false;
            x = mHandRect.CenterX();
            y = hand_y;
            hand_y += cell_height;
        }
        
        Point base(x, y);
        if (!mPartial.IsActive(id)) {
            DrawHandTile(rCanvas, base, tile, odd_flag);
        } else {
            active_base = base;
			active_odd = odd_flag;
        }
    }
    
    TileIdType active_id = mPartial.GetActive();
    if (active_id != Tile::ID_NONE) {
		// there's an active tile
        Tile active_tile = mPartial.GetTileById(active_id);
        DrawHandTile(rCanvas, active_base, active_tile, active_odd);
    }

	ASSERT(mTileMap.size() == mPartial.CountTiles());
}

void TopWindow::DrawInactiveHands(Canvas &rCanvas) {
	ASSERT(mpGame != NULL);

    PCntType cell_height = CellHeight();
    ColorType area_color = COLOR_DARK_BLUE;
    ColorType edge_color = COLOR_LIGHT_GRAY;

    Hands other_hands = mpGame->InactiveHands();
    Hands::IteratorType i_hand;
    LogicalXType right_x = ClientAreaWidth() - mPadPixels;
    for (i_hand = other_hands.begin(); i_hand < other_hands.end(); i_hand++) {
        // draw header
        int top_y = mPadPixels;
        bool rightFlag = false;
        ColorType header_color = COLOR_BLACK;
        Rect header_rect = DrawHandHeader(rCanvas, top_y, right_x, *i_hand, header_color, rightFlag);

        // draw hand area below the header
        top_y = header_rect.BottomY() - 1;
        int left_x = header_rect.LeftX();
        int width = header_rect.Width();
        Tiles hand_tiles = Tiles(*i_hand);
        unsigned tile_count = hand_tiles.Count();
        unsigned height = tile_count*cell_height + 2*mPadPixels;
        area_color = COLOR_DARK_BLUE;
        rCanvas.UseColors(area_color, edge_color);
        Rect hand_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

        // draw tiles
        int tile_x = hand_rect.CenterX();
        int tile_y = hand_rect.TopY() + mPadPixels + cell_height/2;

        for (unsigned i = 0; i < hand_tiles.Count(); i++) {
            Tile tile = hand_tiles[i];
            Point point(tile_x, tile_y);
            if (mShowTilesFlag) {
                DrawTile(rCanvas, point, tile, false);
            } else {
                DrawBlankTile(rCanvas, point, false);
            } 
            tile_y += cell_height;
        }

		// pad between hands
        right_x = header_rect.LeftX() - mPadPixels;
    }
}

void TopWindow::DrawPaused(Canvas &rCanvas) {
	ASSERT(IsGamePaused());

    ColorType bg_color = COLOR_BLACK;
    ColorType text_color = COLOR_WHITE;
    rCanvas.UseColors(bg_color, text_color);

    LogicalXType x = 0;
    LogicalYType y = 0;
    Rect clientArea(y, x, ClientAreaWidth(), ClientAreaHeight());
    rCanvas.DrawText(clientArea, "The game is paused.  Click here to proceed.");
        
	if (mpGame != NULL) {
	    int top_y = mPadPixels;
        int left_x = mPadPixels;
        Hand active_hand = Hand(*mpGame);
        bool left = true;
        DrawHandHeader(rCanvas, top_y, left_x, active_hand, bg_color, left);
	}
}

Rect TopWindow::DrawTile(Canvas &rCanvas, Point point, Tile const &rTile, bool oddFlag) {
    TileIdType id = rTile.Id();

    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (mPartial.Contains(id)) {
        tile_color = COLOR_WHITE;
    }
    if (mPartial.IsActive(id)) {
       point.Offset(mDragTileDeltaX, mDragTileDeltaY);
    }
	
	AIndexType colorInd;
	if (mColorAttributeCnt == 1) {
        colorInd = rTile.Attribute(0);
	} else {
		ASSERT(mColorAttributeCnt == 0);
        colorInd = 0;
	}
	ASSERT(colorInd < 9);
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

	PCntType tile_height = TileHeight();

    Rect result = rCanvas.DrawTile(point, mTileWidth, tile_height,
                    numberOfGlyphAttributes, glyphs, tile_color, glyph_color, oddFlag);
    
    return result;
}

Cell TopWindow::GetCell(Point const &rPoint) const {
    PCntType grid_unit_x = GridUnitX();
	PCntType offset_x = grid_unit_x/2;
	LogicalXType dx = rPoint.X() - mStartCell.X() + offset_x;
	IndexType column;
    if (dx >= 0) {
		column = dx / grid_unit_x;
	} else {
		LogicalXType abs_num = grid_unit_x - dx - 1;
		ASSERT(abs_num > 1);
		column = -long(abs_num / grid_unit_x);
	}

    PCntType grid_unit_y = GridUnitY();
	PCntType offset_y = grid_unit_y/2;
	LogicalYType dy = rPoint.Y() - mStartCell.Y() + offset_y;
	IndexType row;
    if (dy >= 0) {
		row = -long(dy / grid_unit_y);
	} else {
		LogicalYType abs_num = grid_unit_y - dy - 1;
		ASSERT(abs_num > 1);
		row = abs_num / grid_unit_y;
	}

    Cell result(row, column);
    
    return result;
}

TileIdType TopWindow::GetTileId(Point const &rPoint) const {
	TileIdType result = 0;

	TileMapType::const_iterator i_tile;
    for (i_tile = mTileMap.begin(); i_tile != mTileMap.end(); i_tile++) {
        TileIdType id = i_tile->first;
        Rect rect = i_tile->second;
        if (rect.Contains(rPoint)) {
            if (mPartial.IsActive(id)) {
                result = id;
			    break;
            } else if (IsInSwapArea(rPoint)) {
                if (!mPartial.IsInSwap(id)) {                
			        continue;
                }
            } else if (IsInHandArea(rPoint)) {
                if (!mPartial.IsInHand(id)) {                
			        continue;
                }
            }
            result = id;
			break;
        }
    }

    return result;
}

PCntType TopWindow::GridUnitX(void) const {
    PCntType result = 2;

	switch (Cell::Grid()) {
	    case GRID_TRIANGLE:
		    result = CellWidth()/2;
			break;
		case GRID_4WAY:
		case GRID_8WAY:
		    result = CellWidth();
			break;
		case GRID_HEX:
		    result = PCntType(0.5 + 0.75*CellWidth());
			break;
		default:
			FAIL();
	}

    if (mShowGridFlag) {
        result -= 1; // shring by the width of one grid line
    }

    return result;
}

PCntType TopWindow::GridUnitY(void) const {
    PCntType result = 2;

	switch (Cell::Grid()) {
		case GRID_TRIANGLE:
		case GRID_4WAY:
		case GRID_8WAY:
		    result = CellHeight();
			break;
	    case GRID_HEX:
		    result = CellHeight()/2;
			break;
		default:
			FAIL();
	}

    if (mShowGridFlag) {
        result -= 1; // height of grid line
    }

    return result;
}

void TopWindow::HandleButtonDown(Point const &rMouse) {
	ASSERT(mpGame != NULL);
	ASSERT(IsGameOver() || !IsGamePaused());
    mMouseLast = rMouse;

	TileIdType id = GetTileId(rMouse);
    if (id != Tile::ID_NONE) {
        if (!IsMouseCaptured()) {
            // Capture mouse to drag the tile
            CaptureMouse();
            mPartial.Activate(id);
            mDragTileDeltaX = 0;
            mDragTileDeltaY = 0;
			mMouseUpCnt = 0;
		}
        
    } else if (!IsInHandArea(rMouse) && !IsInSwapArea(rMouse)) {
        // Capture mouse to drag the board
    	CaptureMouse();
        mDragBoardFlag = true;
		mDragBoardPixelCnt = 0;
    }
}

void TopWindow::HandleButtonUp(Point const &rMouse) {
	ASSERT(mpGame != NULL);
	HandleMouseMove(rMouse);

	if (mDragBoardFlag) {
		if (mDragBoardPixelCnt < 5 && !mpGame->IsOver()) {
		    // Drags shorter than five pixels 
			// are treated as normal mouse-clicks 
			// which activate/deactivate the cell.
			Cell cell = GetCell(rMouse);
			if (IsInCellArea(rMouse, cell)) {
			    if (mTargetCellFlag && cell == mTargetCell) {
     			    mTargetCellFlag = false;
			    } else if (mPartial.IsVisible(cell) && mPartial.IsEmpty(cell)) {
				    mTargetCell = cell;
				    mTargetCellFlag = true;
				}
			}
		}
   		StopDragging();
    } else {
        ASSERT(mPartial.GetActive() != Tile::ID_NONE);        
		ReleaseActiveTile(rMouse);
    }
}

void TopWindow::HandleMenuCommand(int command) {
	ASSERT(mpPlayMenu != NULL);
	ASSERT(mpViewMenu != NULL);

    switch (command) {
    // File menu options
        case IDM_NEW: {
	        if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
				OfferSaveGame();
			}
			OfferNewGame();
            break;
	    }

		case IDM_OPEN:
		case IDM_REOPEN:
		case IDM_REVERT:
		case IDM_SAVE:
		case IDM_SAVE_AS:
		case IDM_PRINT:
		    FAIL(); // TODO
		    break;

		case IDM_CLOSE:
	        if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
				OfferSaveGame();
			}
			SetGame(NULL);
		    break;

        case IDM_EXIT: // terminate the application
			Close();
            break;

	    // Play menu options
	    case IDM_PLAY_PLAY: 
			if (!IsGameOver() && !IsGamePaused()) {
                bool passFlag = false;
			    Play(passFlag);
			}
		    break;

	    case IDM_TAKE_BACK:
			mPartial.Reset();
            ForceRepaint();
			UpdateMenus();
		    break;
	    case IDM_PAUSE:
			TogglePause();
		    break;
	    case IDM_PASS: {
            bool passFlag = true;
            Play(passFlag);
            break;
        }
		case IDM_RESIGN:
			break;

		case IDM_RESTART:
		case IDM_UNDO:
		case IDM_REDO:
		    FAIL(); // TODO
			break;

		case IDM_AUTOPAUSE:
            mAutopauseFlag = !mAutopauseFlag;
	        UpdateMenus();
            break;

	    // View menu options
        case IDM_SMALL_TILES:
        case IDM_MEDIUM_TILES:
        case IDM_LARGE_TILES:
            SetTileWidth(command);
	        ForceRepaint();
	        UpdateMenus();
            break;
        case IDM_RECENTER:
            mIsStartCentered = false;
            Resize(ClientAreaWidth(), ClientAreaHeight());
            break;
		case IDM_ATTRIBUTES:
		    FAIL(); // TODO
			break;
	    case IDM_SHOW_CLOCKS:
            mShowClocksFlag = !mShowClocksFlag;
	        ForceRepaint();
			UpdateMenus();
            break;
        case IDM_SHOW_GRID:
            mShowGridFlag = !mShowGridFlag;
	        ForceRepaint();
			UpdateMenus();
            break;
        case IDM_HINTS: {
			HintType hint_strength = HintType(mPartial);
			HintBox box(hint_strength, mGameStyle);
			box.Run(this);
			hint_strength = HintType(box);
			mPartial.SetHintStrength(hint_strength);
			ForceRepaint();
			break;
		}
        case IDM_SHOW_SCORES:
            mShowScoresFlag = !mShowScoresFlag;
	        ForceRepaint();
			UpdateMenus();
            break;
        case IDM_SHOW_TILES:
			ASSERT(IsGameOver() || mGameStyle == GAME_STYLE_DEBUG);
            mShowTilesFlag = !mShowTilesFlag;
	        ForceRepaint();
			UpdateMenus();
            break;
		case IDM_ANIMATION:
		    FAIL(); // TODO
			break;
		case IDM_AUTOCENTER:
            mAutocenterFlag = !mAutocenterFlag;
	        UpdateMenus();
			break;

        // Help menu options
        case IDM_RULES: {
	   	    Dialog rules("RULES");
			rules.Run(this);
            break;
		}
        case IDM_ABOUT: {
	   	    Dialog about("ABOUT");
			about.Run(this);
            break;
		}
        case IDM_WARRANTY: {
	   	    Dialog warranty("WARRANTY");
			warranty.Run(this);
            break;
		}

		default:
			FAIL();
    }
}

LRESULT TopWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
	    case WM_CLOSE: {
		    if (mpGame != NULL && mpGame->HasUnsavedChanges()) {
				OfferSaveGame();
			}
			if (mpGame != NULL) {
			    SetGame(NULL);
			}
		    result = Window::HandleMessage(message, wParam, lParam);
			break;
	    }

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
			if (mpGame != NULL) {
			    if (IsGamePaused()) {
				    TogglePause();
                } else if (!IsDragging()) {
			        POINTS points = MAKEPOINTS(lParam);
			        Point mouse(points);
                    HandleButtonDown(mouse);
				}
			}
            break;

        case WM_LBUTTONUP: // complete left-click
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS points = MAKEPOINTS(lParam);
				    Point mouse(points);
                    HandleButtonUp(mouse);
					UpdateMenus();
				} else {
					StopDragging();
				}
    		    ForceRepaint();
        	}
  		    break;

        case WM_MOUSEMOVE: // mouse position update
			if (IsDragging()) {
                if (IsMouseCaptured()) {
				    POINTS points = MAKEPOINTS(lParam);
				    Point mouse(points);
	                HandleMouseMove(mouse);
				} else {
					StopDragging();
				}
                ForceRepaint();
			}
            break;

		case WM_MOUSEWHEEL: {
            int z_delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mStartCell.Offset(0, -z_delta/5);
			ForceRepaint();
			break;
		}

	    case WM_PAINT: // repaint
            Repaint();
            break;
           
        case WM_SIZE: { // resize
            PCntType clientAreaWidth = LOWORD(lParam);
            PCntType clientAreaHeight = HIWORD(lParam);
            Resize(clientAreaWidth, clientAreaHeight);
            break;
        }

		case WM_TIMER: { // timeout
			int timer_id = int(wParam);
			if (timer_id == ID_CLOCK_TIMER) {
			    if (mShowClocksFlag && !IsGamePaused() && !IsGameOver()) {
					// don't update menus because that would cause flicker
			        ForceRepaint();  // for active player's clock
			    } else {
					SetTimer(TIMEOUT_MSEC, timer_id);
				}
			}
			break;
		}

	    default:  // invoke message handler of the base class
		    result = Window::HandleMessage(message, wParam, lParam);
		    break;
    }

    return result;
}

void TopWindow::HandleMouseMove(Point const &rMouse) {
    long drag_x = rMouse.X() - mMouseLast.X();
    long drag_y = rMouse.Y() - mMouseLast.Y();
    mMouseLast = rMouse;

    if (mDragBoardFlag) {
        mStartCell.Offset(drag_x, drag_y);
		mDragBoardPixelCnt += ::abs(drag_x) + ::abs(drag_y);

	} else {
        ASSERT(mPartial.GetActive() != Tile::ID_NONE);        
        mDragTileDeltaX += drag_x;
        mDragTileDeltaY += drag_y;
    }
}

int TopWindow::MessageDispatchLoop(void) {
    int exitCode;

	HACCEL table = GetAcceleratorTable("HOTKEYS");

	while (true) {
        MSG message;
	    HWND any_window = NULL;
		UINT no_filtering = 0;
        BOOL success = Win::GetMessage(&message, any_window, no_filtering, no_filtering);
        if (success == 0) {   // retrieved a WM_QUIT message
			exitCode = message.wParam;
			break;
		} else if (success == -1) { // error in GetMessage()
            exitCode = -1;
			break;
		}

		int translated = Win::TranslateAccelerator(Handle(), table, &message);
		if (!translated) {
            Win::TranslateMessage(&message); 
            Win::DispatchMessage(&message); 
        } 
    }

	return exitCode;
}

char const *TopWindow::Name(void) const {
	return "Gold Tile - a game by Stephen Gold";
}

void TopWindow::OfferNewGame(void) {
	unsigned seconds_per_hand = SECONDS_PER_MINUTE * ParmBox1::PLAYER_MINUTES_DEFAULT;
	if (mpGame != NULL) {
		seconds_per_hand = mpGame->SecondsPerHand();
	}
	ParmBox1 parmbox1(mGameStyle, seconds_per_hand);

	bool wrap_flag;
	IndexType height, width;
	Cell::GetTopology(wrap_flag, height, width);
	GridType grid = Cell::Grid();
	ParmBox2 parmbox2(wrap_flag, height, width, grid);

	unsigned attribute_cnt = Tile::AttributeCnt();
	unsigned tile_redundancy = Game::TILE_REDUNDANCY_DEFAULT;
	unsigned hand_cnt = HAND_CNT_DEFAULT;
	unsigned hand_size = Game::HAND_SIZE_DEFAULT;
	if (mpGame != NULL) {
		tile_redundancy = mpGame->Redundancy();
	    hand_cnt = mpGame->InactiveHands().Count() + 1;
	    hand_size = mpGame->HandSize();
	}
	unsigned clones_per_tile = tile_redundancy - 1;
	ParmBox3 parmbox3(attribute_cnt, clones_per_tile, hand_size, hand_cnt);

	unsigned max_attribute_cnt = Tile::AttributeCnt();
	AValueType *max_attribute_values = new AValueType[max_attribute_cnt];
	for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		max_attribute_values[i_attr] = Tile::ValueMax(i_attr);
	}

	Strings player_names;
    Indices auto_hands;
    Indices remote_hands;
	LPARAM *ip_addresses = NULL;

STEP1:
	int result = parmbox1.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
		return;
	}
	ASSERT(result == Dialog::RESULT_OK);

STEP2:
	result = parmbox2.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
		return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP1;
	}
	ASSERT(result == Dialog::RESULT_OK);

STEP3:
	result = parmbox3.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
	    return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP2;
	}
	ASSERT(result == Dialog::RESULT_OK);

	attribute_cnt = parmbox3.AttributeCnt();
   	if (attribute_cnt > max_attribute_cnt) {
		// allocate storage for more attribute limits
	    AValueType *new_max_attribute_values = new AValueType[attribute_cnt];

		// copy old limits
	    for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
		    new_max_attribute_values[i_attr] = max_attribute_values[i_attr];
	    }
		delete[] max_attribute_values;

		// initialize new limits
		for (unsigned i_attr = max_attribute_cnt; i_attr < attribute_cnt; i_attr++) {
			new_max_attribute_values[i_attr] = 6; // TODO
		}

		max_attribute_cnt = attribute_cnt;
		max_attribute_values = new_max_attribute_values;
	}

STEP4:
	String template_name = "TILEBOX" + String(max_attribute_cnt);
	TileBox tilebox((char const *)template_name, max_attribute_cnt, max_attribute_values);
	result = tilebox.Run(this);
	if (result == Dialog::RESULT_CANCEL) {
	    return;
	} else if (result == Dialog::RESULT_BACK) {
		goto STEP3;
	}
	ASSERT(result == Dialog::RESULT_OK);
    TileBox::ValueType *num_values = tilebox.NumValues();

	// copy new limits
    for (unsigned i_attr = 0; i_attr < max_attribute_cnt; i_attr++) {
	    max_attribute_values[i_attr] = num_values[i_attr] - 1;
    }

	hand_cnt = parmbox3.HandCnt();
   	if (hand_cnt > player_names.Count()) {
		// allocate storage for more IP addresses
        IpAddressType *new_ip_addresses = new IpAddressType[hand_cnt];

		// copy old addresses
		for (unsigned i = 0; i < player_names.Count(); i++) {
			new_ip_addresses[i] = ip_addresses[i];
		}
		delete[] ip_addresses;

		// initialize new addresses and names
		for (unsigned i = player_names.Count(); i < hand_cnt; i++) {
			new_ip_addresses[i] = 0;  // produces 0.0.0.0
			String new_name = player_names.InventUnique("Player");
			player_names.Append(new_name);
		}

		ip_addresses = new_ip_addresses;
	}
	ASSERT(player_names.Count() == hand_cnt);
    
	Strings::IteratorType i_name = player_names.Begin();
	for (unsigned i = 0; i < hand_cnt; ) {
		bool more_flag = (i < hand_cnt - 1);
		bool auto_flag = auto_hands.Contains(i);
		bool remote_flag = remote_hands.Contains(i);
		HandBox box(i + 1, more_flag, *i_name, auto_flag, remote_flag, ip_addresses[i]);
		result = box.Run(this);
		if (result == Dialog::RESULT_CANCEL) {
			return;
		} else if (result == Dialog::RESULT_BACK) {
			if (i == 0) {
				goto STEP4;
			} else {
				i--;
				i_name--;
			}
		} else {
		    ASSERT(result == Dialog::RESULT_OK);
		    *i_name = box.PlayerName();
		    auto_hands.AddRemove(i, box.IsAutomatic());
		    remote_hands.AddRemove(i, box.IsRemote());
		    ip_addresses[i] = IpAddressType(box);

			i++;
			i_name++;
		}
	}

	Tile::SetStatic(attribute_cnt, max_attribute_values);

	grid = GridType(parmbox2);
	Cell::SetGrid(grid);

	wrap_flag = parmbox2.DoesWrap();
	height = parmbox2.Height();
	width = parmbox2.Width();
	Cell::SetTopology(wrap_flag, height, width);

	mGameStyle = GameStyleType(parmbox1);

	tile_redundancy = 1 + parmbox3.ClonesPerTile();
	hand_size = parmbox3.HandSize();
	seconds_per_hand = parmbox1.PlayerSeconds();
	Game *p_new_game = new Game(player_names, tile_redundancy, hand_size, seconds_per_hand);
	ASSERT(p_new_game != NULL);

	delete[] ip_addresses;
	delete[] max_attribute_values;

	SetGame(p_new_game);
}

void TopWindow::OfferSaveGame(void) {
#if 0
    YesNo box("UNSAVED");
	box.Run(this);
	// TODO
#endif
}

void TopWindow::Play(bool passFlag) {
	ASSERT(mpGame != NULL);
	ASSERT(mPartial.GetActive() == Tile::ID_NONE);
	ASSERT(!IsGamePaused() && !IsGameOver());
    Move move = mPartial.GetMove(true);
    
	char const *reason;
	bool is_legal = mpGame->IsLegalMove(move, reason);
    if (move.IsPass() == passFlag && is_legal) {
        mpGame->FinishTurn(move);
		mpGame->StopClock();
        if (mpGame->IsOver()) {
            mShowClocksFlag = true;
            mShowScoresFlag = true;
            mShowTilesFlag = true;
            mpGame->GoingOutBonus();
        } else {
			mTargetCellFlag = false;
            mpGame->ActivateNextHand();
            if (!mAutopauseFlag) {
				mpGame->StartClock();
			}
            if (mAutocenterFlag) {
                // center the start cell
                mIsStartCentered = false;
                Resize(ClientAreaWidth(), ClientAreaHeight());
            }
        }

        mPartial.Reset();
               
    } else if (!is_legal) { // explain the issue
        Dialog box(reason);
		box.Run(this);		
		if (::str_eq(reason, "FIRST")) {
			mPartial.Reset();
		}

	}
	UpdateMenus();
	ForceRepaint();
}

void TopWindow::Recenter(PCntType oldHeight, PCntType oldWidth) {
    if (mIsStartCentered) {
        //_startX += (_clientAreaWidth - oldWidth)/2;
        //_startY += (_clientAreaHeight - oldHeight)/2;
    } else if (ClientAreaWidth() > 250 && ClientAreaHeight() > 100) {
        LogicalXType x = ClientAreaWidth()/2; // TODO
        LogicalYType y = ClientAreaHeight()/2;
        mStartCell = Point(x, y);
        mIsStartCentered = true;
    }
}

void TopWindow::ReleaseActiveTile(Point const &rMouse) {
	ASSERT(mpGame != NULL);

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
	bool to_hand = IsInHandArea(rMouse);
	bool to_swap = (!to_hand) && IsInSwapArea(rMouse); // overlap is possible
	bool to_board = !(to_hand || to_swap);
	Cell to_cell;
	if (to_board) {
		to_cell = GetCell(rMouse);
		if (!IsInCellArea(rMouse, to_cell)) {
			return;
		}
	}

	if (from_hand && to_hand ||
	    from_swap && to_swap ||
	    from_board && to_board && from_cell == to_cell)
	{
		// Trivial drags which don't actually move the tile
		// are treated as normal mouse-clicks which
		// activate/deactivate the tile or play it to the
		// target cell.
		if (mMouseUpCnt == 1) {
			StopDragging(); // deactivates the tile
    		return;
		} else {
		    ASSERT(mMouseUpCnt == 0);
			if (mTargetCellFlag) {
			    to_board = true;
				to_cell = mTargetCell;
			} else {
        		mMouseUpCnt = 1;
				return;
			}
		}
	}

	if (to_board && (!to_cell.IsValid() || mPartial.GetCell(to_cell) != 0)) {
		// cell conflict - can't construct a move in the usual way
	    Dialog empty("EMPTY");
		empty.Run(this);
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

	// Check whether the new partial move is legal.
    Move move_so_far = mPartial.GetMove(true);
    char const *reason;
	bool legal = mpGame->IsLegalMove(move_so_far, reason);

	if (!legal && (to_swap || !::str_eq(reason, "FIRST"))) {  
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
		if (::str_eq(reason, "START") && !from_board) {
			reason = "STARTSIMPLE";
	    }
	    Dialog box(reason);
		box.Run(this);
	}

    if (mTargetCellFlag && !mPartial.IsEmpty(mTargetCell)) {
        // target cell got filled
        mTargetCellFlag = false;
    }

	StopDragging();
}

void TopWindow::Repaint(void) {
    HWND this_window = Handle();
    PAINTSTRUCT paint_struct;    
    HDC context = Win::BeginPaint(this_window, &paint_struct);
    ASSERT(context != NULL);
    
    bool release_me = false;
    PCntType width = ClientAreaWidth();
    PCntType height = ClientAreaHeight();
    Canvas canvas(context, this_window, release_me, width, height);
    
	if (mpGame == NULL) {
		// TODO

	} else if (IsGamePaused()) {
		DrawPaused(canvas);

    } else {
        DrawBoard(canvas, 0);
        DrawBoard(canvas, 1);
        DrawInactiveHands(canvas);
        DrawActiveHand(canvas);
        DrawHandTiles(canvas);
    }

    canvas.Close();
    Win::EndPaint(this_window, &paint_struct);

	// restart the timer
	SetTimer(TIMEOUT_MSEC, ID_CLOCK_TIMER);

	if (mInitialNewGame) {
		mInitialNewGame = false;
		ASSERT(mpGame == NULL);
		OfferNewGame();
	}
}

void TopWindow::Resize(PCntType clientAreaWidth, PCntType clientAreaHeight) {
    PCntType old_height = ClientAreaHeight();
    PCntType old_width = ClientAreaWidth();
    SetClientArea(clientAreaWidth, clientAreaHeight);
    Recenter(old_height, old_width);
    ForceRepaint();
}

void TopWindow::SetGame(Game *pGame) {
	// TODO: free old Game object?
	mpGame = pGame;

	if (mGameStyle == GAME_STYLE_CHALLENGE) {
	    mPartial = Partial(mpGame, HINT_CHALLENGE_DEFAULT);
        mAutocenterFlag = true;
        mAutopauseFlag = true;
	    mShowClocksFlag = true;
	} else {
	    mPartial = Partial(mpGame, HINT_DEFAULT);
        mAutocenterFlag = false;
        mAutopauseFlag = false;
	    mShowClocksFlag = false;
	}

    SetTileWidth(IDM_LARGE_TILES);

	if (mpGame != NULL && !mAutopauseFlag) {
		mpGame->StartClock();
	}

	ForceRepaint();
    UpdateMenus();
}

void TopWindow::SetTileWidth(int command) {
	PCntType small_width = 0;

	switch(Cell::Grid()) {
	case GRID_4WAY:
	case GRID_8WAY:
	    small_width = 20; // TODO define named constants
	    break;
	case GRID_HEX:
		small_width = 24;
		break;
	case GRID_TRIANGLE:
		small_width = 32;
		break;
	default:
		FAIL();
	}

	switch(command) {
    case IDM_SMALL_TILES:
        mTileWidth = small_width;
        break;
    case IDM_MEDIUM_TILES:
        mTileWidth = 2*small_width;
        break;
    case IDM_LARGE_TILES:
        mTileWidth = 3*small_width;
        break;
	default:
		FAIL();
	}
	
	mTileSizeCmd = command;
}

void TopWindow::StopDragging(void) {
	ASSERT(IsDragging());

	mDragBoardFlag = false;
	mPartial.Deactivate();
	Win::ReleaseCapture();

    ASSERT(mPartial.GetActive() == Tile::ID_NONE);
	ASSERT(!IsDragging());
	ASSERT(!IsMouseCaptured());
}

PCntType TopWindow::TileHeight(void) const {
    PCntType result = 0;

	switch(Cell::Grid()) {
	case GRID_4WAY:
	case GRID_8WAY:
	    result = mTileWidth;
	    break;
	case GRID_HEX:
	case GRID_TRIANGLE:
		result = PCntType((1 + SQRT_3*mTileWidth)/2);
		break;
	default:
	    FAIL();
	}

    return result;
}

void TopWindow::TogglePause(void) {
	if (mpGame != NULL && !IsGameOver()) {
		if (IsGamePaused()) {
		    mpGame->StartClock();
		} else {
			mpGame->StopClock();
		}
	}
    ForceRepaint();
    UpdateMenus();
}

void TopWindow::UpdateMenus(void) {
	bool is_game = (mpGame != NULL);
	bool is_over = IsGameOver();
	bool is_paused = IsGamePaused();
    bool is_pass = mPartial.IsPass();

	// "File" menu
	mpFileMenu->EnableItems(is_game);
	mpFileMenu->Enable(true);

	// "Play" menu
    mpPlayMenu->Autopause(mAutopauseFlag);
    mpPlayMenu->Pause(is_paused);
	mpPlayMenu->EnableItems(mGameStyle, is_over, is_paused, is_pass);
	mpPlayMenu->Enable(mpGame != NULL);

	// "View" menu
    mpViewMenu->TileSize(mTileSizeCmd);
    mpViewMenu->ShowClocks(mShowClocksFlag);
    mpViewMenu->ShowGrid(mShowGridFlag);
    mpViewMenu->ShowScores(mShowScoresFlag);
    mpViewMenu->ShowTiles(mShowTilesFlag);
    mpViewMenu->Autocenter(mAutocenterFlag);
	mpViewMenu->EnableItems(mGameStyle, is_over, is_paused, is_pass);
	mpViewMenu->Enable(!is_paused);
	
	UpdateMenuBar();
}


// inquiry methods

bool TopWindow::IsDragging(void) const {
	bool result = false;

	if (mpGame != NULL 
	  && (mDragBoardFlag || mPartial.GetActive() != Tile::ID_NONE))
	{
		result = true;
	}

	return result;
}

bool TopWindow::IsGameOver(void) const {
	bool result = false;
	if (mpGame != NULL) {
		result = mpGame->IsOver();
	}

	return result;
}

bool TopWindow::IsGamePaused(void) const {
	bool result = false;
	if (mpGame != NULL && !mpGame->IsOver()) {
		result = mpGame->IsPaused();
	}

	return result;
}

bool TopWindow::IsInCellArea(Point const &rPoint, Cell const &rCell) const {
	IndexType row = rCell.Row();
    IndexType column = rCell.Column();
	LogicalXType center_x = CellX(column);
    LogicalYType center_y = CellY(row);
    Point center(center_x, center_y);

	PCntType cell_height = CellHeight();
    PCntType cell_width = CellWidth();
	bool odd_flag = rCell.IsOdd();
	Rect cell_rect = Canvas::InteriorGridShape(center, cell_width, 
		               cell_height, odd_flag);

	bool result = cell_rect.Contains(rPoint);

	return result;
}

bool TopWindow::IsInHandArea(Point const &rPoint) const {
	bool result = mHandRect.Contains(rPoint);

	return result;
}

bool TopWindow::IsInSwapArea(Point const &rPoint) const {
	bool result = mSwapRect.Contains(rPoint);

	return result;
}

bool TopWindow::IsInTile(Point const &rPoint) const {
	TileIdType id = GetTileId(rPoint);
	bool result = (id != Tile::ID_NONE);

	return result;
}

