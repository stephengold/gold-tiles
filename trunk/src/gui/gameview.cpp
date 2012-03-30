// File:    gameview.cpp
// Purpose: implement GameView class
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

#ifdef _WINDOWS
#include "gui/canvas.hpp"
#include "gui/color.hpp"
#include "gui/gameview.hpp"
#include "gui/menubar.hpp"
#include "gui/player.hpp"
#include "gui/rect.hpp"
#include "gui/resource.hpp"
#include "gui/topwindow.hpp"


// static data

// lifecycle

GameView::GameView(Game const &rGame):
    Partial(&rGame, HINT_DEFAULT),
    mHandRect(0, 0, 0, 0),
    mStartCell(0, 0),
    mSwapRect(0, 0, 0, 0)
{
    mpMenuBar = NULL;
    mPadPixels = 6;
	mTargetCellFlag = false;
	mpWindow = NULL;

	SetTileWidth(IDM_LARGE_TILES);
}

// The compiler-generated destructor is OK.

void GameView::SetWindow(TopWindow *pWindow, MenuBar *pMenuBar) {
	mpMenuBar = pMenuBar;
	mpWindow = pWindow;
}


// operators

GameView::operator DisplayModes(void) const {
	return mDisplayModes;
}


// misc methods

PCntType GameView::CellHeight(void) const {
    PCntType result = TileHeight();
    if (IsGridVisible()) {
        result += 2; // add room for two grid lines
    }

    return result;
}

PCntType GameView::CellWidth(void) const {
    PCntType result = mTileWidth;
    if (IsGridVisible()) {
        result += 2; // add room for two grid lines
    }

	ASSERT(::is_even(result));
    return result;
}

LogicalXType GameView::CellX(IndexType column) const {
    PCntType grid_unit = GridUnitX();
    LogicalXType result = mStartCell.X() + grid_unit*column;

    return result;
}

LogicalYType GameView::CellY(IndexType row) const {
    PCntType grid_unit = GridUnitY();
    LogicalYType result = mStartCell.Y() - grid_unit*row;

    return result;
}

String GameView::ClockText(Hand &rHand) const {
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

	unsigned minutes = seconds / SECONDS_PER_MINUTE;
	seconds -= minutes*SECONDS_PER_MINUTE;
    unsigned tens_of_seconds = seconds / 10;
	seconds -= tens_of_seconds*10;

	ASSERT(seconds >= 0);
	ASSERT(seconds <= 9);
	ASSERT(tens_of_seconds >= 0);
	ASSERT(tens_of_seconds <= 5);
	ASSERT(minutes >= 0);

	String result;
	if (minus_sign) {
		result = "-";
	}
	result += String(minutes) + ":" 
		   + String(tens_of_seconds) + String(seconds);

	return result;
}

void GameView::DrawActiveHand(Canvas &rCanvas) {
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
    unsigned tile_cnt = CountHand();
    PCntType cell_height = CellHeight();
	PCntType height = rCanvas.TextHeight() + 2*mPadPixels;
	if (!active_hand.HasResigned() && !active_hand.HasGoneOut()) {
        height = tile_cnt*cell_height + 2*mPadPixels;
        if (tile_cnt < CountTiles()) {
            // show that there's room for more
            height += cell_height/2;
	    }
	}

	// choose colors for hand area (mHandRect)
	TileIdType active_tile = GetActive();
	if (IsInHand(active_tile)) {
        // The active tile started from this hand.
		ASSERT(tile_cnt > 0);
		--tile_cnt;
	}

	if (!IsLocalPlayer()) {
	    area_color = COLOR_DARK_BLUE;
	} else if (tile_cnt < CountTiles()) {
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

    if (active_hand.HasResigned()) {
		String text = "resigned";
		rCanvas.DrawText(mHandRect, text);
	} else if (active_hand.HasGoneOut()) {
		String text = "went out";
		rCanvas.DrawText(mHandRect, text);
	}

    unsigned stock_cnt = mpGame->CountStock();
    top_y = mHandRect.BottomY() - 1;

	if (active_hand.HasGoneOut() || active_hand.HasResigned()) {
        mSwapRect = Rect(top_y, left_x, width, 0);
	} else {
        // calculate height of swap area (mSwapRect)
        tile_cnt = CountSwap();
        height = tile_cnt*cell_height + rCanvas.TextHeight() + 3*mPadPixels;
	    unsigned played_tile_cnt = CountPlayed();
        if (tile_cnt < CountTiles() && tile_cnt < stock_cnt) {
            // there's room for more tiles
            height += cell_height/2;
	    }

        // choose color for swap area (mSwapRect)
	    if (IsInSwap(active_tile)) {
		    ASSERT(tile_cnt > 0);
		    --tile_cnt;
	    }
	    if (IsOnBoard(active_tile)) {
		    ASSERT(played_tile_cnt > 0);
		    --played_tile_cnt;
	    }
	    if (!IsLocalPlayer()) {
	        area_color = COLOR_DARK_BLUE;
	    } else if (played_tile_cnt == 0
                && tile_cnt < CountTiles()
                && tile_cnt < stock_cnt)
        {
            area_color = COLOR_DARK_GREEN;
        } else { // can't add more tiles to swap area
            area_color = COLOR_BROWN;
        }
        rCanvas.UseColors(area_color, edge_color);

        // draw swap area (mSwapRect)
        mSwapRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    
        String swap_text = "swap area";
        LogicalYType y = mSwapRect.BottomY() - mPadPixels - rCanvas.TextHeight();
        Rect bounds(y, left_x, width, rCanvas.TextHeight());
        rCanvas.DrawText(bounds, swap_text);
	}

    // calculate height of stock area
    height = 2*rCanvas.TextHeight() + 3*mPadPixels;

	// choose colors for stock area
    area_color = COLOR_DARK_BLUE;
    rCanvas.UseColors(area_color, edge_color);

	// draw stock area
    top_y = mSwapRect.BottomY() - 1;
    Rect stock_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

    String stock_text1 = plural(stock_cnt, "tile");
	LogicalYType y = top_y + mPadPixels;
    Rect bounds1(y, left_x, width, rCanvas.TextHeight());
    rCanvas.DrawText(bounds1, stock_text1);

    String stock_text2 = "in the stock bag";
	y = bounds1.BottomY();
    Rect bounds2(y, left_x, width, rCanvas.TextHeight() + mPadPixels);
    rCanvas.DrawText(bounds2, stock_text2);
}

void GameView::DrawBlankTile(Canvas &rCanvas, Point const &rCenter, bool oddFlag) {
	PCntType height = TileHeight();
    ColorType tile_color = COLOR_LIGHT_GRAY;
    rCanvas.DrawBlankTile(rCenter, mTileWidth, height, tile_color, oddFlag);
}

void GameView::DrawBoard(Canvas &rCanvas, unsigned showLayer) {
    Board board = Board(*this);

	int const column_fringe = 1;
	int row_fringe = 1;
	if (Cell::Grid() == GRID_HEX) {
		row_fringe = 2;
	}

    IndexType top_row = row_fringe + board.NorthMax();
    IndexType bottom_row = -row_fringe - board.SouthMax();
    IndexType right_column = column_fringe + board.EastMax();
    IndexType left_column = -column_fringe - board.WestMax();
    ASSERT(bottom_row <= top_row);
    ASSERT(left_column <= right_column);

	unsigned swap_cnt = CountSwap();
	TileIdType const active_tile = GetActive();
	if (IsInSwap(active_tile)) {
	    ASSERT(swap_cnt > 0);
		--swap_cnt;
	}

	if (IsLocalPlayer() && !mTargetCellFlag && CountHinted() == 1) {
		mTargetCell = FirstHinted();
		mTargetCellFlag = true;
	}

    for (IndexType row = top_row; row >= bottom_row; row--) {
        if (CellY(row) > LogicalYType(mpWindow->ClientAreaHeight())) {
            break;
        }
        for (IndexType column = left_column; column <= right_column; column++) {
            if (CellX(column) > LogicalXType(mpWindow->ClientAreaWidth())) {
                break;
            } else if (Cell::IsValid(row, column)) {
                Cell const cell(row, column);
				bool const hinted = IsHinted(cell);
				bool const empty = IsEmpty(cell);
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

void GameView::DrawCell(Canvas &rCanvas, Cell const &rCell, unsigned swapCnt) {
	IndexType const row = rCell.Row();
    IndexType const column = rCell.Column();
	LogicalXType const center_x = CellX(column);
    LogicalYType const center_y = CellY(row);
    Point const center(center_x, center_y);

    bool const hinted = IsHinted(rCell);
	bool const used = !IsEmpty(rCell);

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
    if (IsGridVisible()) {
		grid_color = feature_color;
	}

    PCntType const cell_height = CellHeight();
    PCntType const cell_width = CellWidth();
	bool const odd_flag = rCell.IsOdd();
    Rect const rect = rCanvas.DrawCell(center, cell_width, cell_height, 
		   cell_color, grid_color, odd_flag);
    
	// draw cell features
	rCanvas.UseColors(cell_color, feature_color);
	if (rCell.IsStart()) {
	    rCanvas.DrawText(rect, "START", "S");
	}
	
	if (mTargetCellFlag && rCell == mTargetCell) {
	    rCanvas.DrawTarget(rect);
	}

    // Draw the active tile later (not now) so it won't get obscured.
	TileIdType const id = Partial::GetCellTile(rCell);
    if (id != 0 && !IsActive(id)) {
        Tile const tile = GetTileById(id);
        DrawTile(rCanvas, center, tile, odd_flag);
    }
}

Rect GameView::DrawHandHeader(
    Canvas &rCanvas, 
    LogicalYType topY, 
    LogicalXType leftRight, 
    Hand &rHand, 
    ColorType areaColor, 
    bool leftFlag)
{
    ASSERT(mpGame != NULL);

    PCntType const cell_width = CellWidth();

    String const name_text = rHand.Name();
    Hand const active_hand = Hand(*mpGame);
    PCntType const w = rCanvas.TextWidth(name_text);
    PCntType width = (cell_width > w) ? cell_width : w;
    if (name_text == active_hand.Name()) {
	    PCntType w2 = rCanvas.TextWidth("in the stock bag");
	    if (w2 > width) {
		    width = w2;
	    }
    }

    String scoreText;
    if (mpMenuBar->AreScoresVisible()) {
        unsigned score = rHand.Score();
        scoreText = plural(score, "point");
        PCntType w3 = rCanvas.TextWidth(scoreText);
        if (w3 > width) {
            width = w3;
        }
    }
    
    String clock_text;
    if (mpMenuBar->AreClocksVisible()) {
        clock_text = ClockText(rHand);
        PCntType w4 = rCanvas.TextWidth(clock_text);
        if (w4 > width) {
            width = w4;
        }
    }
                                 
    width += 2*mPadPixels;
    LogicalXType left_x, right_x;
    if (leftFlag) {
        left_x = leftRight;
        right_x = left_x + width;
    } else {
        right_x = leftRight;
        left_x = right_x - width;
    }
    PCntType const text_height = rCanvas.TextHeight();

    PCntType score_height = 0;
    if (mpMenuBar->AreScoresVisible()) {
        score_height = text_height;
    }
    PCntType clock_height = 0;
    if (mpMenuBar->AreClocksVisible()) {
        clock_height = text_height;
    }
    PCntType height = text_height + score_height + clock_height + 2*mPadPixels;
    rCanvas.UseColors(areaColor, areaColor);
    Rect result = rCanvas.DrawRectangle(topY, left_x, width, height);

    ColorType const text_color = COLOR_WHITE;
    rCanvas.UseColors(areaColor, text_color);

    LogicalYType y = topY + mPadPixels;
    Rect bounds(y, left_x, width, text_height);
    rCanvas.DrawText(bounds, name_text);
    y += text_height;
    
    if (mpMenuBar->AreScoresVisible()) {
        Rect score_box(y, left_x, width, score_height);
        rCanvas.DrawText(score_box, scoreText);
        y += score_height;
    }

    if (mpMenuBar->AreClocksVisible()) {
        Rect clock_box(y, left_x, width, text_height);
        rCanvas.DrawText(clock_box, clock_text);
    }
    
    return result;
}

void GameView::DrawHandTile(
    Canvas &rCanvas,
    Point const &rCenter,
    Tile const &rTile,
	bool oddFlag)
{
    TileIdType const id = rTile.Id();

	if (!IsLocalPlayer() && !IsOnBoard(id) && !mpMenuBar->IsPeeking()) {
		// draw the tile's backside
        DrawBlankTile(rCanvas, rCenter, oddFlag);

    } else {  // draw the tile's face
        Rect const rect = DrawTile(rCanvas, rCenter, rTile, oddFlag);
        
        TilePair const pair(id, rect);
        TileInsResult const ins_result = mTileMap.insert(pair);
        bool const success = ins_result.second;
        ASSERT(success);
   }
}

void GameView::DrawHandTiles(Canvas &rCanvas) {
	ASSERT(mpGame != NULL);

    PCntType const cell_height = CellHeight();
    LogicalYType hand_y = mHandRect.TopY() + mPadPixels + cell_height/2;
    LogicalYType swap_y = mSwapRect.TopY() + mPadPixels + cell_height/2;

    unsigned tile_cnt = CountSwap();
    unsigned stock_cnt = mpGame->CountStock();
    if (tile_cnt < CountTiles()  && tile_cnt < stock_cnt) {
        swap_y += cell_height/2;
    }

    mTileMap.clear();
    
    Point active_base(0, 0);
	bool active_odd = false;
    for (unsigned i = 0; i < CountTiles(); i++) {
        Tile tile = GetTileByIndex(i);
        TileIdType id = tile.Id();
	    bool odd_flag;
        LogicalXType x;
        LogicalYType y;
        if (IsOnBoard(id)) {
            Cell cell = LocateTile(id);
			IndexType row = cell.Row();
			IndexType column = cell.Column();
			odd_flag = (is_odd(column) != is_odd(row));
            x = CellX(column);
            y = CellY(row);
        } else if (IsInSwap(id)) {
			odd_flag = false;
            x = mSwapRect.CenterX();
            y = swap_y;
            swap_y += cell_height;
        } else {
            ASSERT(IsInHand(id)); 
			odd_flag = false;
            x = mHandRect.CenterX();
            y = hand_y;
            hand_y += cell_height;
        }
        
        Point const base(x, y);
        if (!IsActive(id)) {
            DrawHandTile(rCanvas, base, tile, odd_flag);
        } else {
            active_base = base;
			active_odd = odd_flag;
        }
    }
    
    TileIdType const active_id = GetActive();
    if (active_id != Tile::ID_NONE) {
		// there's an active tile
        Tile const active_tile = GetTileById(active_id);
        DrawHandTile(rCanvas, active_base, active_tile, active_odd);
    }

	ASSERT(!IsLocalPlayer() || mTileMap.size() == CountTiles());
}

void GameView::DrawInactiveHands(Canvas &rCanvas) {
	ASSERT(mpGame != NULL);

    PCntType const cell_height = CellHeight();
    ColorType area_color = COLOR_DARK_BLUE;
    ColorType edge_color = COLOR_LIGHT_GRAY;

    Hands other_hands = mpGame->InactiveHands();
    Hands::Iterator i_hand;
    LogicalXType right_x = mpWindow->ClientAreaWidth() - mPadPixels;
    LogicalYType top_y = mPadPixels;
    for (i_hand = other_hands.begin(); i_hand < other_hands.end(); i_hand++) {
        // draw header
        bool rightFlag = false;
        ColorType header_color = COLOR_BLACK;
        Rect header_rect = DrawHandHeader(rCanvas, top_y, right_x, *i_hand, header_color, rightFlag);

        // draw hand area below the header
        top_y = header_rect.BottomY() - 1;
        LogicalXType left_x = header_rect.LeftX();
        PCntType width = header_rect.Width();
        Tiles hand_tiles = Tiles(*i_hand);
        unsigned tile_count = hand_tiles.Count();
        area_color = COLOR_DARK_BLUE;
        rCanvas.UseColors(area_color, edge_color);
		PCntType height = rCanvas.TextHeight() + 2*mPadPixels;
		if (!i_hand->HasResigned() && !i_hand->HasGoneOut() && mpMenuBar->IsPeeking()) {
            height = tile_count*cell_height + 2*mPadPixels;
		}
        Rect hand_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

		if (i_hand->HasResigned()) {
			String text = "resigned";
			rCanvas.DrawText(hand_rect, text);
		} else if (i_hand->HasGoneOut()) {
			String text = "went out";
			rCanvas.DrawText(hand_rect, text);
		} else if (!mpMenuBar->IsPeeking()) {
			String text = ::plural(tile_count, "tile");
			rCanvas.DrawText(hand_rect, text);
		} else {
            // draw tiles
            LogicalXType const tile_x = hand_rect.CenterX();
            LogicalYType tile_y = hand_rect.TopY() + mPadPixels + cell_height/2;

            for (unsigned i = 0; i < tile_count; i++) {
                Tile tile = hand_tiles[i];
                Point point(tile_x, tile_y);
                if (mpMenuBar->IsPeeking()) {
                    DrawTile(rCanvas, point, tile, false);
                } else {
                    DrawBlankTile(rCanvas, point, false);
                } 
                tile_y += cell_height;
			}

		}

		// pad between hands
		if (mpMenuBar->IsPeeking()) {
		    // right to left
            right_x = header_rect.LeftX() - mPadPixels;
            top_y = mPadPixels;
		} else {
		    // top to bottom
            right_x = mpWindow->ClientAreaWidth() - mPadPixels;
            top_y += height + mPadPixels;
		}
    }
}

void GameView::DrawPaused(Canvas &rCanvas) {
	ASSERT(IsGamePaused());
	ASSERT(!IsGameOver());

    ColorType bg_color = COLOR_BLACK;
    ColorType text_color = COLOR_WHITE;
    rCanvas.UseColors(bg_color, text_color);

    LogicalXType const x = 0;
    LogicalYType const y = 0;
    Rect clientArea(y, x, mpWindow->ClientAreaWidth(), mpWindow->ClientAreaHeight());
    rCanvas.DrawText(clientArea, "The game is paused.  Click here to proceed.");
        
	if (mpGame != NULL) {
	    LogicalYType const top_y = mPadPixels;
        LogicalXType const left_x = mPadPixels;
        Hand active_hand = Hand(*mpGame);
        bool const left = true;
        DrawHandHeader(rCanvas, top_y, left_x, active_hand, bg_color, left);
	}
}

Rect GameView::DrawTile(Canvas &rCanvas, Point const &rCenter, Tile const &rTile, bool oddFlag) {
    TileIdType id = rTile.Id();

    ColorType tile_color = COLOR_LIGHT_GRAY;
	if (rTile.HasBonus()) {
		tile_color = COLOR_DULL_GOLD;
	}
    if (Contains(id)) {
        tile_color = COLOR_WHITE;
	    if (rTile.HasBonus()) {
		    tile_color = COLOR_GOLD;
	    }
    }

	Point center = rCenter;
    if (IsActive(id)) {
       center.Offset(mpWindow->DragTileDeltaX(), mpWindow->DragTileDeltaY());
    }
	
	TileDisplay const tile_display(rTile, mDisplayModes);

	PCntType const tile_height = TileHeight();
    Rect result = rCanvas.DrawTile(tile_display, tile_color, center, mTileWidth, 
		               tile_height, oddFlag);
    
    return result;
}

Cell GameView::GetPointCell(Point const &rPoint) const {
    PCntType grid_unit_x = GridUnitX();
	PCntType offset_x = grid_unit_x/2;
	LogicalXType dx = rPoint.X() - mStartCell.X() + offset_x;
	IndexType column;
    if (dx >= 0) {
		column = dx / grid_unit_x;
	} else {
		LogicalXType const abs_num = grid_unit_x - dx - 1;
		ASSERT(abs_num > 1);
		column = -long(abs_num / grid_unit_x);
	}

    PCntType const grid_unit_y = GridUnitY();
	PCntType const offset_y = grid_unit_y/2;
	LogicalYType dy = rPoint.Y() - mStartCell.Y() + offset_y;
	IndexType row;
    if (dy >= 0) {
		row = -long(dy / grid_unit_y);
	} else {
		LogicalYType const abs_num = grid_unit_y - dy - 1;
		ASSERT(abs_num > 1);
		row = abs_num / grid_unit_y;
	}

    Cell const result(row, column);
    
    return result;
}

TileIdType GameView::GetTileId(Point const &rPoint) const {
	TileIdType result = 0;

	TileConstIter i_tile;
    for (i_tile = mTileMap.begin(); i_tile != mTileMap.end(); i_tile++) {
        TileIdType const id = i_tile->first;
        Rect const rect = i_tile->second;
        if (rect.Contains(rPoint)) {
            if (IsActive(id)) {
                result = id;
			    break;
            } else if (IsInSwapArea(rPoint)) {
                if (!IsInSwap(id)) {                
			        continue;
                }
            } else if (IsInHandArea(rPoint)) {
                if (!IsInHand(id)) {                
			        continue;
                }
            }
            result = id;
			break;
        }
    }

    return result;
}

PCntType GameView::GridUnitX(void) const {
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

    if (IsGridVisible()) {
        result -= 1; // shrink by the width of one grid line
    }

    return result;
}

PCntType GameView::GridUnitY(void) const {
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

    if (IsGridVisible()) {
        result -= 1; // height of grid line
    }

    return result;
}

void GameView::LoadPlayerOptions(Player const &rPlayer) {
	Point const start_cell_position = Point(rPlayer);
	SetStartCellPosition(start_cell_position);
}

void GameView::Recenter(PCntType oldHeight, PCntType oldWidth) {
    if (mpWindow->ClientAreaWidth() > 250 && mpWindow->ClientAreaHeight() > 100) {
        LogicalXType const x = mpWindow->ClientAreaWidth()/2; // TODO
        LogicalYType const y = mpWindow->ClientAreaHeight()/2;
        mStartCell = Point(x, y);
    }
}

void GameView::Repaint(Canvas &rCanvas) {
	if (mpGame == NULL) {
		// TODO

	} else if (IsGamePaused()) {
		DrawPaused(rCanvas);

    } else {
        DrawBoard(rCanvas, 0);
        DrawBoard(rCanvas, 1);
        DrawInactiveHands(rCanvas);
        DrawActiveHand(rCanvas);
        DrawHandTiles(rCanvas);
    }

    rCanvas.Close();
}

void GameView::ResetTargetCell(void) {
	mTargetCellFlag = false;
}

void GameView::SavePlayerOptions(Player &rPlayer) const {
	Point const start_cell_position = StartCellPosition();
	rPlayer.SetStartCellPosition(start_cell_position);

}

void GameView::SetDisplayModes(DisplayModes const &rDisplayModes) {
	mDisplayModes = rDisplayModes;
}

void GameView::SetGame(Game *pGame) {
	// TODO: free old Game object?
	mpGame = pGame;

    if (pGame != NULL && pGame->Style() == GAME_STYLE_CHALLENGE) {
	    Reset(mpGame, HINT_CHALLENGE_DEFAULT);
	} else {
	    Reset(mpGame, HINT_DEFAULT);
	}
	
    SetTileWidth(IDM_LARGE_TILES);
	ResetTargetCell();
}

void GameView::SetStartCellPosition(Point const &rPoint) {
	mStartCell = rPoint;
}

void GameView::SetTileWidth(IdType command) {
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
}

void GameView::StartCellOffset(long dx, long dy) {
    mStartCell.Offset(dx, dy);
}

Point GameView::StartCellPosition(void) const {
    return mStartCell;
}

Cell GameView::TargetCell(void) const {
	return mTargetCell;
}

Point GameView::TileCenter(TileIdType id) const {
	TileConstIter const i_tile = mTileMap.find(id);
    Rect const rect = i_tile->second;
    Point const result = rect.Center();

	return result;
}

PCntType GameView::TileHeight(void) const {
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

void GameView::ToggleTargetCell(Point const &rPoint) {
	Cell const cell = GetPointCell(rPoint);

	if (cell.IsValid() && IsInCellArea(rPoint, cell)) {
		if (mTargetCellFlag && cell == mTargetCell) {
			ResetTargetCell();
		} else if (IsVisible(cell) && IsEmpty(cell)) {
			// activate new target cell
			mTargetCell = cell;
			mTargetCellFlag = true;
		}
	}
}


// inquiry methods

bool GameView::IsDragging(void) const {
	bool result = false;

	if (mpGame != NULL 
	  && (mpWindow->IsDraggingBoard() || GetActive() != Tile::ID_NONE))
	{
		result = true;
	}

	return result;
}

bool GameView::IsGridVisible(void) const {
	bool const result = mpMenuBar->IsGridVisible();

	return result;
}

bool GameView::IsInCellArea(Point const &rPoint, Cell const &rCell) const {
	IndexType const row = rCell.Row();
    IndexType const column = rCell.Column();
	LogicalXType const center_x = CellX(column);
    LogicalYType const center_y = CellY(row);
    Point const center(center_x, center_y);

	PCntType const cell_height = CellHeight();
    PCntType const cell_width = CellWidth();
	bool const odd_flag = rCell.IsOdd();
	Rect const cell_rect = Canvas::InteriorGridShape(center, cell_width, 
		               cell_height, odd_flag);

	bool const result = cell_rect.Contains(rPoint);

	return result;
}

bool GameView::IsInHandArea(Point const &rPoint) const {
	bool const result = mHandRect.Contains(rPoint);

	return result;
}

bool GameView::IsInSwapArea(Point const &rPoint) const {
	bool result = mSwapRect.Contains(rPoint);

	return result;
}

bool GameView::IsInTile(Point const &rPoint) const {
	TileIdType const id = GetTileId(rPoint);
	bool const result = (id != Tile::ID_NONE);

	return result;
}

bool GameView::IsTargetSet(void) const {
    return mTargetCellFlag;
}

bool GameView::IsTargetUsed(void) const {
	bool result = false;

	if (mTargetCellFlag) {
		TileIdType const id = GetCellTile(mTargetCell);
		result = (id != Tile::ID_NONE);
	}

	return result;
}

#endif // defined(_WINDOWS)
