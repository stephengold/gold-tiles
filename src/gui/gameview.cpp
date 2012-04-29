// File:     gameview.cpp
// Location: src/gui
// Purpose:  implement GameView class
// Author:   Stephen Gold sgold@sonic.net
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
#include "gui/gamewindow.hpp"
#include "gui/menubar.hpp"
#include "gui/player.hpp"
#include "network.hpp"


// lifecycle

GameView::GameView(Game const& rGame):
Partial(&rGame, HINT_DEFAULT, 0.0),
    mHandRect(0, 0, 0, 0),
    mStartCell(0, 0),
    mSwapRect(0, 0, 0, 0)
{
    mpMenuBar = NULL;
    mPadPixels = PAD_PIXELS_DEFAULT;
    mTargetCellFlag = false;
    mpWindow = NULL;

    SetTileSize(TILE_SIZE_DEFAULT);
}

// The compiler-generated destructor is OK.


// operators

GameView::operator DisplayModes(void) const {
    return mDisplayModes;
}


// misc methods

PixelCntType GameView::CellHeight(void) const {
    PixelCntType result = TileHeight();
    if (IsGridVisible()) {
        result += 2; // add room for two grid lines
    }

    return result;
}

PixelCntType GameView::CellWidth(void) const {
    PixelCntType result = mTileWidth;
    if (IsGridVisible()) {
        result += 2; // add room for two grid lines
    }

    ASSERT(::is_even(result));
    return result;
}

LogicalXType GameView::CellX(ColumnType column) const {
    PixelCntType const grid_unit = GridUnitX();
    LogicalXType result = mStartCell.X() + grid_unit*column;

    return result;
}

LogicalYType GameView::CellY(RowType row) const {
    PixelCntType const grid_unit = GridUnitY();
    LogicalYType result = mStartCell.Y() - grid_unit*row;

    return result;
}

String GameView::ClockText(Hand& rHand) const {
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
    ASSERT(tens_of_seconds <= 5);

    String result;
    if (minus_sign) {
        result = "-";
    }
    result += String(minutes) + ":" 
        + String(tens_of_seconds) + String(seconds);

    return result;
}

void GameView::DrawBlankTile(
    Canvas& rCanvas,
    Point const& rCenter,
    bool hasBonus,
    bool oddFlag)
{
    PixelCntType const height = TileHeight();
    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (hasBonus) {
        tile_color = COLOR_DULL_GOLD;
    }

    rCanvas.DrawBlankTile(rCenter, mTileWidth, height, tile_color, oddFlag);
}

void GameView::DrawBoard(Canvas& rCanvas, unsigned showLayer) {
    Board const board = *this;
    int const column_fringe = 1;
    int const row_fringe = Cell::RowFringe();

    // determine which cells are visible in the client area
    Point const ulc(0,0);
    Cell const ulc_cell = GetPointCell(ulc);
    RowType top_see_row = ulc_cell.Row() + row_fringe - 1;
    ColumnType left_see_column = ulc_cell.Column();

    Point const brc = mpWindow->Brc();
    Cell const brc_cell = GetPointCell(brc);
    RowType bottom_see_row = brc_cell.Row() - row_fringe + 1;
    ColumnType right_see_column = brc_cell.Column();

    if (!Cell::DoesBoardWrap()) {
        ASSERT(bottom_see_row <= top_see_row);
        ASSERT(left_see_column <= right_see_column);

        // get the range of "might use" cells
        RowType const top_use_row = row_fringe + board.NorthMax();
        RowType const bottom_use_row = -row_fringe - board.SouthMax();
        ColumnType const right_use_column = column_fringe + board.EastMax();
        ColumnType const left_use_column = -column_fringe - board.WestMax();

        ASSERT(bottom_use_row <= top_use_row);
        ASSERT(left_use_column <= right_use_column);

        // reduce visible range to only include "might use" cells
        if (top_use_row < top_see_row) {
            top_see_row = top_use_row;
        }
        if (bottom_use_row > bottom_see_row) {
            bottom_see_row = bottom_use_row;
        }
        if (left_use_column > left_see_column) {
            left_see_column = left_use_column;
        }
        if (right_use_column < right_see_column) {
            right_see_column = right_use_column;
        }
    }
    ASSERT(bottom_see_row <= top_see_row);
    ASSERT(left_see_column <= right_see_column);

    unsigned swap_cnt = CountSwap();
    Tile::IdType const active_tile = GetActive();
    if (IsInSwap(active_tile)) {
        ASSERT(swap_cnt > 0);
        --swap_cnt;
    }

    // automatic targeting when exactly one cell is hinted
    if (IsLocalUsersTurn() && !mTargetCellFlag && CountHinted() == 1) {
        mTargetCell = FirstHinted();
        mTargetCellFlag = true;
    }

    for (RowType row = top_see_row; row >= bottom_see_row; row--) {
        LogicalYType const center_y = CellY(row);
        for (ColumnType column = left_see_column; column <= right_see_column; column++) {
            Cell const cell(row, column);
            Cell wrap_cell = cell;
            if (Cell::DoesBoardWrap()) {
                wrap_cell.Wrap();
            }
            if ((MightUse(cell) || MightUse(wrap_cell))
                && wrap_cell.IsValid()) {
                    LogicalXType const center_x = CellX(column);
                    Point const center(center_x, center_y);
                    if (showLayer == 2) {
                        Tile::IdType const id = GetCellTile(wrap_cell);
                        if (id != Tile::ID_NONE && IsActive(id)) {
                            // active tile from board -- draw it now
                            Tile const tile(id);
                            bool const odd_flag = cell.IsOdd();
                            DrawTile(rCanvas, center, tile, odd_flag);
                        }
                    } else {
                        bool const empty = IsEmpty(wrap_cell);
                        bool const hinted = IsHinted(wrap_cell);
                        unsigned layer = 0;
                        if (hinted || empty) {
                            layer = 1;
                        }
                        if (layer == showLayer) {
                            DrawCell(rCanvas, wrap_cell, center, swap_cnt);
                        }
                    }
            }
        }
    }
}

void GameView::DrawCell(
    Canvas& rCanvas, 
    Cell const& rCell, 
    Point const& rCenter, 
    unsigned swapCnt)
{
    ASSERT(rCell.IsValid());

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

    PixelCntType const cell_height = CellHeight();
    PixelCntType const cell_width = CellWidth();
    bool const odd_flag = rCell.IsOdd();
    Rect const rect = rCanvas.DrawCell(rCenter, cell_width, cell_height, 
        cell_color, grid_color, odd_flag);

    // draw cell features
    if (mTargetCellFlag && rCell == mTargetCell) {
        // it's the target cell
        ColorType const target_color = COLOR_MEDIUM_GRAY;
        rCanvas.UseColors(target_color, target_color);
        rCanvas.DrawTargetArrow(rect);
    }
    if (rCell.IsStart()) {
        // it's the start cell
        rCanvas.UseColors(COLOR_TRANSPARENT, feature_color);
        rCanvas.DrawTextLine(rect, "START", "S");
    }

    /*  If the active tile came from the board, draw it later 
    (not now) so it won't get obscured. */
    Tile::IdType const id = Partial::GetCellTile(rCell);
    if (id != Tile::ID_NONE && !IsActive(id)) {
        // inactive tile -- draw it now
        Tile const tile(id);
        DrawTile(rCanvas, rCenter, tile, odd_flag);
    }
}

Rect GameView::DrawHandHeader(
    Canvas& rCanvas, 
    LogicalYType topY, 
    LogicalXType leftRight, 
    Hand& rHand, 
    ColorType areaColor, 
    bool leftFlag)
{
    ASSERT(mpGame != NULL);

    PixelCntType const cell_width = CellWidth();

    String const name_text = rHand.Name();
    Hand const playable_hand = *mpGame;
    bool const is_playable = (name_text == playable_hand.Name());

    PixelCntType const w = rCanvas.TextWidth(name_text);
    PixelCntType width = (cell_width > w) ? cell_width : w;
    if (is_playable) {
        PixelCntType w2 = rCanvas.TextWidth("in the stock bag");
        if (w2 > width) {
            width = w2;
        }
    }

    String score_text;
    if (mpMenuBar->AreScoresVisible()) {
        score_text = ScoreText(rHand, is_playable);
        PixelCntType const score_width = rCanvas.TextWidth(score_text);
        if (score_width > width) {
            width = score_width;
        }
    }

    String clock_text;
    if (mpMenuBar->AreClocksVisible()) {
        clock_text = ClockText(rHand);
        PixelCntType const clock_width = rCanvas.TextWidth(clock_text);
        if (clock_width > width) {
            width = clock_width;
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
    PixelCntType const text_height = rCanvas.TextHeight();

    PixelCntType score_height = 0;
    if (mpMenuBar->AreScoresVisible()) {
        score_height = text_height;
    }
    PixelCntType clock_height = 0;
    if (mpMenuBar->AreClocksVisible()) {
        clock_height = text_height;
    }
    PixelCntType height = text_height + score_height + clock_height + 2*mPadPixels;
    rCanvas.UseColors(areaColor, areaColor);
    Rect result = rCanvas.DrawRectangle(topY, left_x, width, height);

    ColorType const text_color = COLOR_WHITE;
    rCanvas.UseColors(areaColor, text_color);

    LogicalYType y = topY + mPadPixels;
    Rect bounds(y, left_x, width, text_height);
    rCanvas.DrawTextLine(bounds, name_text);
    y += text_height;

    if (mpMenuBar->AreScoresVisible()) {
        Rect score_box(y, left_x, width, score_height);
        rCanvas.DrawTextLine(score_box, score_text);
        y += score_height;
    }

    if (mpMenuBar->AreClocksVisible()) {
        Rect clock_box(y, left_x, width, text_height);
        rCanvas.DrawTextLine(clock_box, clock_text);
    }

    return result;
}

void GameView::DrawHandTile(
    Canvas& rCanvas,
    Point const& rCenter,
    Tile const& rTile,
    bool oddFlag)
{
    Tile::IdType const id = rTile.Id();

    if (!IsLocalUsersTurn() && !IsOnBoard(id) && !mpMenuBar->IsPeeking()) {
        // draw the tile's backside
        DrawBlankTile(rCanvas, rCenter, rTile.HasBonus(), oddFlag);

    } else {  // draw the tile's face
        DrawTile(rCanvas, rCenter, rTile, oddFlag);
    }
}

void GameView::DrawHandTiles(Canvas& rCanvas) {
    ASSERT(mpGame != NULL);

    PixelCntType const cell_height = CellHeight();
    LogicalYType hand_y = mHandRect.TopY() + mPadPixels + cell_height/2;
    LogicalYType swap_y = mSwapRect.TopY() + mPadPixels + cell_height/2;

    unsigned const tile_cnt = CountSwap();
    unsigned const stock_cnt = mpGame->CountStock();
    if (tile_cnt < CountTiles()  && tile_cnt < stock_cnt) {
        swap_y += cell_height/2;
    }

    Tiles::ConstIterator i_tile;
    for (i_tile = mTiles.begin(); i_tile != mTiles.end(); i_tile++) {
        Tile const tile = *i_tile;
        Tile::IdType const id = tile.Id();
        bool odd_flag = false;
        LogicalXType x;
        LogicalYType y;
        if (IsOnBoard(id)) {
            Cell const cell = LocateTile(id);
            odd_flag = cell.IsOdd();
            ColumnType const column = cell.Column();
            x = CellX(column);
            RowType const row = cell.Row();
            y = CellY(row);
        } else if (IsInSwap(id)) {
            x = mSwapRect.CenterX();
            y = swap_y;
            swap_y += cell_height;
        } else {
            ASSERT(IsInHand(id)); 
            x = mHandRect.CenterX();
            y = hand_y;
            hand_y += cell_height;
        }

        /*  If the active tile came from the board, draw it later 
        (not now) so it won't get obscured. */
        if (!IsActive(id) || !IsOnBoard(id)) {
            // inactive tile or off-board tile -- draw it now
            Point const base(x, y);
            DrawHandTile(rCanvas, base, tile, odd_flag); 
        }
    }
}

void GameView::DrawIdle(Canvas& rCanvas) {
    ASSERT(!HasGame());

    ColorType const bg_color = COLOR_BLACK;
    ColorType const text_color = COLOR_GREEN;
    rCanvas.UseColors(bg_color, text_color);

    Point const ulc(0, 0);
    Point const brc = mpWindow->Brc(); 
    Rect const client_area(ulc, brc);
    String msg = "Type Ctrl+N to start a new game.";
#ifdef _SERVER
    msg = Network::AddressReport() + "\n\n" + msg;
#endif  // defined(_SERVER)
    rCanvas.DrawTextMultiline(client_area, msg);
}

void GameView::DrawPaused(Canvas& rCanvas) {
    ASSERT(HasGame());
    ASSERT(IsGamePaused());
    ASSERT(!IsGameOver());

    ColorType const bg_color = COLOR_BLACK;
    ColorType const text_color = COLOR_GREEN;
    rCanvas.UseColors(bg_color, text_color);

    Point const ulc(0, 0);
    Point const brc = mpWindow->Brc(); 
    Rect const client_area(ulc, brc);
    rCanvas.DrawTextLine(client_area, "The game is paused.  Click here to resume.");

    LogicalYType const top_y = mPadPixels;
    LogicalXType const left_x = mPadPixels;
    Hand playable_hand = *mpGame;
    bool const left = true;
    DrawHandHeader(rCanvas, top_y, left_x, playable_hand, bg_color, left);
}

void GameView::DrawPlayableHand(Canvas& rCanvas) {
    // draw header
    ASSERT(mpGame != NULL);

    LogicalYType top_y = mPadPixels;
    LogicalXType left_x = mPadPixels;
    Hand playable_hand = *mpGame;
    ColorType area_color = COLOR_BLACK;
    bool const left = true;
    Rect header_rect = DrawHandHeader(rCanvas, top_y, left_x, playable_hand, area_color, left);
    left_x = header_rect.LeftX();
    PixelCntType width = header_rect.Width();

    // calculate height of hand area (mHandRect)
    unsigned tile_cnt = CountHand();
    PixelCntType const cell_height = CellHeight();
    PixelCntType height = rCanvas.TextHeight() + 2*mPadPixels;
    if (!playable_hand.HasResigned() && !playable_hand.HasGoneOut()) {
        height = tile_cnt*cell_height + 2*mPadPixels;
        if (tile_cnt < CountTiles()) {
            // show that there's room for more
            height += cell_height/2;
        }
    }

    // choose colors for hand area (mHandRect)
    Tile::IdType const active_tile = GetActive();
    if (IsInHand(active_tile)) {
        // The active tile started from this hand.
        ASSERT(tile_cnt > 0);
        --tile_cnt;
    }

    if (!IsLocalUsersTurn()) {
        area_color = COLOR_DARK_BLUE;
    } else if (tile_cnt < CountTiles()) {
        area_color = COLOR_DARK_GREEN;
    } else { // hand is full
        area_color = COLOR_BROWN;
    }
    ColorType const edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);

    // draw hand area (mHandRect)
    top_y = header_rect.BottomY() - 1;
    mHandRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    left_x = mHandRect.LeftX();
    width = mHandRect.Width();

    if (playable_hand.HasResigned()) {
        rCanvas.DrawTextLine(mHandRect, "resigned");
    } else if (playable_hand.HasGoneOut()) {
        rCanvas.DrawTextLine(mHandRect, "went out");
    }

    // draw swap area (mSwapRect)
    top_y = mHandRect.BottomY() - 1;
    if (playable_hand.HasGoneOut() || playable_hand.HasResigned()) {
        mSwapRect = Rect(top_y, left_x, width, 0);
    } else {
        mSwapRect = DrawSwapArea(rCanvas, top_y, left_x, width);
    }

    // draw stock area (mSwapRect)
    top_y = mSwapRect.BottomY() - 1;
    DrawStockArea(rCanvas, top_y, left_x, width);
}

void GameView::DrawStockArea(
    Canvas& rCanvas,
    LogicalYType top_y,
    LogicalXType left_x,
    PixelCntType width)
{
    // calculate height of the stock area
    PixelCntType const height = 2*rCanvas.TextHeight() + 3*mPadPixels;

    // set colors for the stock area
    ColorType const area_color = COLOR_DARK_BLUE;
    ColorType const edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);

    // draw the stock area
    Rect const stock_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

    unsigned const stock_cnt = mpGame->CountStock();
    String const stock_text1 = plural(stock_cnt, "tile");
    LogicalYType y = top_y + mPadPixels;
    Rect const bounds1(y, left_x, width, rCanvas.TextHeight());
    rCanvas.DrawTextLine(bounds1, stock_text1);

    y = bounds1.BottomY();
    Rect const bounds2(y, left_x, width, rCanvas.TextHeight() + mPadPixels);
    rCanvas.DrawTextLine(bounds2, "in the stock bag");
}

Rect GameView::DrawSwapArea(
    Canvas& rCanvas,
    LogicalYType topY,
    LogicalXType leftX,
    PixelCntType width)
{
    // calculate height of swap area (mSwapRect)
    PixelCntType const cell_height = CellHeight();
    unsigned const stock_cnt = mpGame->CountStock();
    unsigned tile_cnt = CountSwap();
    PixelCntType height = tile_cnt*cell_height + rCanvas.TextHeight() + 3*mPadPixels;

    if (tile_cnt < CountTiles() && tile_cnt < stock_cnt) {
        // show that there's room for more tiles
        height += cell_height/2;
    }

    // choose color for swap area
    Tile::IdType const active_tile = GetActive();
    if (IsInSwap(active_tile)) {
        ASSERT(tile_cnt > 0);
        --tile_cnt;
    }
    unsigned played_tile_cnt = CountPlayed();
    if (IsOnBoard(active_tile)) {
        ASSERT(played_tile_cnt > 0);
        --played_tile_cnt;
    }
    ColorType area_color;
    if (!IsLocalUsersTurn()) {
        area_color = COLOR_DARK_BLUE;
    } else if (played_tile_cnt == 0
        && tile_cnt < CountTiles()
        && tile_cnt < stock_cnt)
    {
        area_color = COLOR_DARK_GREEN;
    } else { // can't add more tiles to swap area
        area_color = COLOR_BROWN;
    }
    ColorType const edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);

    // draw swap area
    Rect const swap_rect = rCanvas.DrawRectangle(topY, leftX, width, height);

    // label swap area
    LogicalYType const y = swap_rect.BottomY() - mPadPixels - rCanvas.TextHeight();
    Rect bounds(y, leftX, width, rCanvas.TextHeight());
    rCanvas.DrawTextLine(bounds, "swap area");

    return swap_rect;
}

void GameView::DrawTile(Canvas& rCanvas, Point const& rCenter, Tile const& rTile, bool oddFlag) {
    Tile::IdType const id = rTile.Id();

    AttrCntType const marking_cnt = mDisplayModes.MarkingCnt();
    ASSERT(marking_cnt <= Markings::MARKING_CNT_MAX);

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

    Markings const markings(rTile, mDisplayModes);
    PixelCntType const tile_height = TileHeight();
    bool const warm_flag = IsWarmTile(id);
    Rect const result = rCanvas.DrawTile(markings, tile_color, center, mTileWidth, 
        tile_height, warm_flag, oddFlag);

    if (Contains(id)) {
        TilePair const pair(id, result);
        mTileMap.insert(pair);
    }
}

void GameView::DrawUnplayableHands(Canvas& rCanvas) {
    ASSERT(mpGame != NULL);

    PixelCntType const cell_height = CellHeight();
    ColorType area_color = COLOR_DARK_BLUE;
    ColorType const edge_color = COLOR_LIGHT_GRAY;

    Hands hands = mpGame->UnplayableHands();
    Hands::Iterator i_hand;
    LogicalXType right_x = mpWindow->ClientAreaWidth() - mPadPixels;
    LogicalYType top_y = mPadPixels;
    for (i_hand = hands.begin(); i_hand < hands.end(); i_hand++) {
        // draw header
        bool const rightFlag = false;
        ColorType const header_color = COLOR_BLACK;
        Rect header_rect = DrawHandHeader(rCanvas, top_y, right_x, *i_hand, header_color, rightFlag);

        // draw hand area below the header
        top_y = header_rect.BottomY() - 1;
        LogicalXType const left_x = header_rect.LeftX();
        PixelCntType const width = header_rect.Width();
        Tiles const hand_tiles = *i_hand;
        unsigned const tile_count = hand_tiles.Count();
        area_color = COLOR_DARK_BLUE;
        rCanvas.UseColors(area_color, edge_color);
        PixelCntType height = rCanvas.TextHeight() + 2*mPadPixels;
        if (!i_hand->HasResigned() && !i_hand->HasGoneOut() && mpMenuBar->IsPeeking()) {
            height = tile_count*cell_height + 2*mPadPixels;
        }
        Rect const hand_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

        if (i_hand->HasResigned()) {
            rCanvas.DrawTextLine(hand_rect, "resigned");
        } else if (i_hand->HasGoneOut()) {
            rCanvas.DrawTextLine(hand_rect, "went out");
        } else if (!mpMenuBar->IsPeeking()) {
            String const text = ::plural(tile_count, "tile");
            rCanvas.DrawTextLine(hand_rect, text);
        } else { // peeking:  draw tiles
            LogicalXType const tile_x = hand_rect.CenterX();
            LogicalYType tile_y = hand_rect.TopY() + mPadPixels + cell_height/2;

            Tiles::ConstIterator i_tile;
            for (i_tile = hand_tiles.begin(); i_tile != hand_tiles.end(); i_tile++) {
                Tile const tile = *i_tile;
                Point const center(tile_x, tile_y);
                if (mpMenuBar->IsPeeking()) {
                    DrawTile(rCanvas, center, tile, false);
                } else {
                    DrawBlankTile(rCanvas, center, tile.HasBonus(), false);
                } 
                tile_y += cell_height;
            }
        } // if peeking

        // add padding between hands
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

Cell GameView::GetPointCell(Point const& rPoint) const {
    PixelCntType const grid_unit_x = GridUnitX();
    PixelCntType const offset_x = grid_unit_x/2;
    LogicalXType const dx = rPoint.X() - mStartCell.X() + offset_x;
    ColumnType column;
    if (dx >= 0) {
        column = ColumnType(dx / grid_unit_x);
    } else {
        // integer division is not defined for negative numerator
        LogicalXType const abs_num = grid_unit_x - dx - 1;
        ASSERT(abs_num > 1);
        column = -ColumnType(abs_num / grid_unit_x);
    }

    PixelCntType const grid_unit_y = GridUnitY();
    PixelCntType const offset_y = grid_unit_y/2;
    LogicalYType const dy = rPoint.Y() - mStartCell.Y() + offset_y;
    RowType row;
    if (dy >= 0) {
        row = -RowType(dy / grid_unit_y);
    } else {
        // integer division is not defined for negative numerator
        LogicalYType const abs_num = grid_unit_y - dy - 1;
        ASSERT(abs_num > 1);
        row = RowType(abs_num / grid_unit_y);
    }

    Cell const result(row, column);

    return result;
}

Tile::IdType GameView::GetTileId(Point const& rPoint) const {
    Tile::IdType result = Tile::ID_NONE;

    TileConstIterator i_tile;
    for (i_tile = mTileMap.begin(); i_tile != mTileMap.end(); i_tile++) {
        Tile::IdType const id = i_tile->first;
        Rect const rect = i_tile->second;
        if (rect.Contains(rPoint)) {
            // make sure the tile in question is visible
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

PixelCntType GameView::GridUnitX(void) const {
    PixelCntType result = 2;

    switch (Cell::Grid()) {
    case GRID_TRIANGLE:
        result = CellWidth()/2;
        break;
    case GRID_4WAY:
    case GRID_8WAY:
        result = CellWidth();
        break;
    case GRID_HEX:
        result = PixelCntType(0.5 + 0.75*CellWidth());
        break;
    default:
        FAIL();
    }

    if (IsGridVisible()) {
        result -= 1; // shrink by the width of one grid line
    }

    return result;
}

PixelCntType GameView::GridUnitY(void) const {
    PixelCntType result = 2;

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

void GameView::LoadPlayerOptions(Player const& rPlayer) {
    mDisplayModes = DisplayModes(rPlayer);
    ASSERT(mDisplayModes.MarkingCnt() <= Markings::MARKING_CNT_MAX);

    mStartCell = Point(rPlayer);

    unsigned const size = rPlayer.TileSize();
    SetTileSize(size);
}

void GameView::Recenter(PixelCntType oldHeight, PixelCntType oldWidth) {
    if (mpWindow->ClientAreaWidth() > 250 && mpWindow->ClientAreaHeight() > 100) {
        LogicalXType const x = mpWindow->ClientAreaWidth()/2; // TODO
        LogicalYType const y = mpWindow->ClientAreaHeight()/2;
        mStartCell = Point(x, y);
    }
}

void GameView::Repaint(Canvas& rCanvas) {
    if (mpGame == NULL) {
        DrawIdle(rCanvas);

    } else if (IsGamePaused()) {
        DrawPaused(rCanvas);

    } else {
        mTileMap.clear();
        DrawBoard(rCanvas, 0);
        DrawBoard(rCanvas, 1);
        DrawUnplayableHands(rCanvas);
        DrawPlayableHand(rCanvas);
        DrawHandTiles(rCanvas);
        DrawBoard(rCanvas, 2);
    }

    rCanvas.Close();
}

void GameView::ResetTargetCell(void) {
    mTargetCellFlag = false;
}

void GameView::SavePlayerOptions(Player& rPlayer) const {
    rPlayer.SetDisplayModes(mDisplayModes);
    rPlayer.SetStartCellPosition(mStartCell);
    // tile size is saved from the menu bar
}

String GameView::ScoreText(Hand const& rHand, bool isPlayable) const {
    ASSERT(mpGame != NULL);

    unsigned const score = rHand.Score();

    unsigned points_this_turn = 0;
    if (isPlayable) {
        points_this_turn = Points();
    }

    String result;
    if (points_this_turn == 0) {
        result = plural(score, "point");
    } else {
        result = String(score) + "+" + plural(points_this_turn, "point");
    }

    return result;
}

void GameView::SetDisplayModes(DisplayModes const& rDisplayModes) {
    mDisplayModes = rDisplayModes;
}

void GameView::SetGame(Game* pGame) {
    // TODO: free old Game object?
    mpGame = pGame;

    if (pGame != NULL && pGame->Style() == GAME_STYLE_CHALLENGE) {
        Reset(mpGame, HINT_CHALLENGE_DEFAULT, 0.0);
    } else {
        Reset(mpGame, HINT_DEFAULT, 0.0);
    }

    SetTileSize(TILE_SIZE_DEFAULT);
    ResetTargetCell();
    mDisplayModes.Cleanup();
}

void GameView::SetTileSize(unsigned size) {
    ASSERT(size >= TILE_SIZE_MIN);
    ASSERT(size <= TILE_SIZE_MAX);

    PixelCntType tiny_width = 0;

    switch(Cell::Grid()) {
    case GRID_4WAY:
    case GRID_8WAY:
        tiny_width = WIDTH_TINY_SQUARE;
        break;
    case GRID_HEX:
        tiny_width = WIDTH_TINY_HEX;
        break;
    case GRID_TRIANGLE:
        tiny_width = WIDTH_TINY_TRIANGLE;
        break;
    default:
        FAIL();
    }

    mTileWidth = size * tiny_width;

    ASSERT(mTileWidth > 0);
    ASSERT(::is_even(mTileWidth));
}

void GameView::SetWarmTiles(Indices const& rIndices) {
    mWarmTiles = rIndices;
}

void GameView::SetWindow(GameWindow* pWindow, MenuBar* pMenuBar) {
    mpMenuBar = pMenuBar;
    mpWindow = pWindow;
}

void GameView::StartCellOffset(long dx, long dy) {
    mStartCell.Offset(dx, dy);
}

Cell GameView::TargetCell(void) const {
    return mTargetCell;
}

Point GameView::TileCenter(Tile::IdType id, Point const& rMouse) const {
    Point result(0,0);
    bool found = false;
    PixelCntType best_distance = 0;
    TileConstIteratorPair const pair = mTileMap.equal_range(id);
    TileConstIterator i_tile;
    for (i_tile = pair.first; i_tile != pair.second; i_tile++) {
        Rect const rect = i_tile->second;
        Point const center = rect.Center();
        PixelCntType const distance = center.Distance(rMouse);
        if (!found || distance < best_distance) {
            result = center;
            best_distance = distance;
            found = true;
        }
    }
    ASSERT(found);

    return result;
}

PixelCntType GameView::TileHeight(void) const {
    PixelCntType result = 0;

    switch(Cell::Grid()) {
    case GRID_4WAY:
    case GRID_8WAY:
        result = mTileWidth;
        break;
    case GRID_HEX:
    case GRID_TRIANGLE:
        result = PixelCntType((1 + SQRT_3*mTileWidth)/2);
        break;
    default:
        FAIL();
    }

    return result;
}

void GameView::ToggleTargetCell(Point const& rPoint) {
    Cell const raw_cell = GetPointCell(rPoint);
    Cell wrap_cell = raw_cell;
    if (Cell::DoesBoardWrap()) {
        wrap_cell.Wrap();
    }

    if (wrap_cell.IsValid() && IsInCellArea(rPoint, raw_cell)) {
        if (mTargetCellFlag && wrap_cell == mTargetCell) {
            ResetTargetCell();
        } else if (MightUse(wrap_cell) && IsEmpty(wrap_cell)) {
            // activate new target cell
            mTargetCell = wrap_cell;
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

bool GameView::IsInCellArea(Point const& rPoint, Cell const& rCell) const {
    RowType const row = rCell.Row();
    ColumnType const column = rCell.Column();
    LogicalXType const center_x = CellX(column);
    LogicalYType const center_y = CellY(row);
    Point const center(center_x, center_y);

    PixelCntType const cell_height = CellHeight();
    PixelCntType const cell_width = CellWidth();
    bool const odd_flag = rCell.IsOdd();
    Rect const cell_rect = Canvas::InteriorGridShape(center, cell_width, 
        cell_height, odd_flag);

    bool const result = cell_rect.Contains(rPoint);

    return result;
}

bool GameView::IsInHandArea(Point const& rPoint) const {
    bool const result = mHandRect.Contains(rPoint);

    return result;
}

bool GameView::IsInSwapArea(Point const& rPoint) const {
    bool result = mSwapRect.Contains(rPoint);

    return result;
}

bool GameView::IsInTile(Point const& rPoint) const {
    Tile::IdType const id = GetTileId(rPoint);
    bool const result = (id != Tile::ID_NONE);

    return result;
}

bool GameView::IsTargetSet(void) const {
    return mTargetCellFlag;
}

bool GameView::IsTargetUsed(void) const {
    bool result = false;

    if (mTargetCellFlag) {
        Tile::IdType const id = GetCellTile(mTargetCell);
        result = (id != Tile::ID_NONE);
    }

    return result;
}

bool GameView::IsWarmTile(Tile::IdType tileId) const {
    bool const result = mWarmTiles.Contains(tileId);

    return result;
}
#endif // defined(_WINDOWS)
