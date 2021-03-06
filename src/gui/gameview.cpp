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

#include "gui/canvas.hpp"
#include "gui/gamewindow.hpp"
#include "gui/menubar.hpp"
#include "gui/user.hpp"
#include "network.hpp"
#include "strings.hpp"


// lifecycle

GameView::GameView(Game const* pGame)
:   Partial(pGame, HINT_DEFAULT, 0.0),
    mHandRect(0, 0, 0, 0),
    mStartCell(0, 0),
    mSwapRect(0, 0, 0, 0)
{
    mpMenuBar = NULL;
    mPadPixels = PAD_PIXELS_DEFAULT;
    mRecenterLeftX = 0;
    mRecenterRightX = 0;
    mTargetCellFlag = false;
    mpWindow = NULL;

    SetBoardTileSize(TILE_SIZE_DEFAULT);
    SetHandTileSize(TILE_SIZE_DEFAULT);
}

// The implicitly defined destructor is OK.


// operators

GameView::operator DisplayModes(void) const {
    return mDisplayModes;
}


// misc methods

Area GameView::CellArea(PlaceType place) const {
    Area result = TileArea(place);
    if (IsGridVisible()) {
        result.Expand(2); // add room for two grid lines
    }

    return result;
}

LogicalXType GameView::CellX(ColumnType column) const {
    PixelCntType const grid_unit = GridUnitX();
    LogicalXType const result = mStartCell.X() + grid_unit*column;

    return result;
}

LogicalYType GameView::CellY(RowType row) const {
    PixelCntType const grid_unit = GridUnitY();
    LogicalYType const result = mStartCell.Y() - grid_unit*row;

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

    MinutesType const minutes = MinutesType(seconds / SECONDS_PER_MINUTE);
    seconds -= minutes*SECONDS_PER_MINUTE;
    unsigned const tens_of_seconds = seconds / 10;
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
    Area const area = TileArea(rCenter);
    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (hasBonus) {
        tile_color = COLOR_DULL_GOLD;
    }

    rCanvas.DrawBlankTile(rCenter, area, tile_color, oddFlag);
}

void GameView::DrawBoard(Canvas& rCanvas, unsigned showLayer) {
    Board const& r_board = BoardReference();
    int const column_fringe = 1;
    int const row_fringe = Cell::RowFringe();

    // Determine which cells are visible in the client area.
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

        // Get the range of "might use" cells.
        RowType const top_use_row = row_fringe + r_board.NorthMax();
        RowType const bottom_use_row = -row_fringe - r_board.SouthMax();
        ColumnType const right_use_column = column_fringe + r_board.EastMax();
        ColumnType const left_use_column = -column_fringe - r_board.WestMax();

        ASSERT(bottom_use_row <= top_use_row);
        ASSERT(left_use_column <= right_use_column);

        // Reduce the visible range to only include "might use" cells.
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

    SizeType swap_cnt = CountSwap();
    Tile::IdType const active_tile = GetActive();
    if (IsInSwap(active_tile)) {
        ASSERT(swap_cnt > 0);
        --swap_cnt;
    }

    // Set target automatically if exactly one cell is hinted.
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
    SizeType swapCnt)
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

    Area const cell_area = CellArea(PLACE_BOARD);
    bool const odd_flag = rCell.IsOdd();
    Rect const rect = rCanvas.DrawCell(rCenter, cell_area, 
        cell_color, grid_color, odd_flag);

    // Draw the cell's features.
    if (mTargetCellFlag && rCell == mTargetCell) {
        // It's the target cell.
        ColorType const target_color = COLOR_MEDIUM_GRAY;
        rCanvas.UseColors(target_color, target_color);
        rCanvas.DrawTargetArrow(rect);
    }
    if (rCell.IsStart()) {
        // It's the start cell.
        rCanvas.UseColors(COLOR_TRANSPARENT, feature_color);
        rCanvas.DrawTextLine(rect, "START", "S");
    }

    /*  
     * If the active tile came from the board, draw it later 
     * (not now) so it won't get obscured.
     */
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

    Area const cell_area = CellArea(PLACE_HAND);
    PixelCntType const cell_width = cell_area.Width();

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

void GameView::DrawIdle(Canvas& rCanvas) {
    ColorType const bg_color = COLOR_BLACK;
    ColorType const text_color = COLOR_GREEN;
    rCanvas.UseColors(bg_color, text_color);

    Strings message_list;
#ifdef _SERVER
    if (Network::IsServerStarted()) {
        message_list.Append(Network::AddressReport());
    } else {
        String const warning = String("Can't listen for invitations on ")  
                             + Network::DescribeListenPort() + "!";
        message_list.Append(warning);
    }
#endif  // defined(_SERVER)
    if (mpWindow->IsWaiting()) {
        message_list.Append(mpWindow->WaitMessage());
    }
#ifdef _CLIENT
    message_list.Append("Type Ctrl+N to start a game.");
#endif  // defined(_CLIENT)
    String const messages(message_list, "\n\n"); 

    Point const ulc(0, 0);
    Point const brc = mpWindow->Brc(); 
    Rect const client_area(ulc, brc);
    rCanvas.DrawTextMultiline(client_area, messages);
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
    rCanvas.DrawTextMultiline(client_area, "The game is paused.\n\nClick here to resume.");

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
    Rect const header_rect = DrawHandHeader(rCanvas, top_y, left_x, 
        playable_hand, area_color, left);
    left_x = header_rect.LeftX();
    PixelCntType width = header_rect.Width();
    mRecenterLeftX = header_rect.RightX();

    // Calculate height of playable hand area (mHandRect).
    SizeType tile_cnt = CountHand();
    Area const cell_area = CellArea(PLACE_HAND);
    PixelCntType const cell_height = cell_area.Height();
    PixelCntType height = rCanvas.TextHeight() + 2*mPadPixels;
    if (!playable_hand.HasResigned() && !playable_hand.HasGoneOut()) {
        height = tile_cnt*cell_height + 2*mPadPixels;
        if (tile_cnt < CountTiles()) {
            // show that there's room for more
            height += cell_height/2;
        }
    }

    // Determine the colors of the playable hand area (mHandRect).
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

    // Draw the hand area (mHandRect).
    top_y = header_rect.BottomY() - 1;
    mHandRect = rCanvas.DrawRectangle(top_y, left_x, width, height);
    left_x = mHandRect.LeftX();
    width = mHandRect.Width();

    if (playable_hand.IsDisconnected()) {
        rCanvas.DrawTextLine(mHandRect, "disconnected");
    } else if (playable_hand.HasResigned()) {
        rCanvas.DrawTextLine(mHandRect, "resigned");
    } else if (playable_hand.HasGoneOut()) {
        rCanvas.DrawTextLine(mHandRect, "went out");
    }

    // Draw the swap area (mSwapRect) below the hand.
    top_y = mHandRect.BottomY() - 1;
    if (playable_hand.HasGoneOut() || playable_hand.HasResigned()) {
        mSwapRect = Rect(top_y, left_x, width, 0);
    } else {
        mSwapRect = DrawSwapArea(rCanvas, top_y, left_x, width);
    }

    // Draw stock bag below the swap area.
    top_y = mSwapRect.BottomY() - 1;
    DrawStockArea(rCanvas, top_y, left_x, width);
}

void GameView::DrawPlayableTile(
    Canvas& rCanvas,
    Point const& rCenter,
    Tile const& rTile,
    bool oddFlag)
{
    Tile::IdType const id = rTile.Id();

    if (!IsLocalUsersTurn() && !IsOnBoard(id) && !mpMenuBar->IsPeeking()) {
        // Draw the tile's backside.
        DrawBlankTile(rCanvas, rCenter, rTile.HasBonus(), oddFlag);

    } else {  // Draw the tile's face.
        DrawTile(rCanvas, rCenter, rTile, oddFlag);
    }
}

void GameView::DrawPlayableTiles(Canvas& rCanvas) {
    ASSERT(mpGame != NULL);

    Area const cell_area = CellArea(PLACE_HAND);
    PixelCntType const cell_height = cell_area.Height();
    LogicalYType hand_y = mHandRect.TopY() + mPadPixels + cell_height/2;
    LogicalYType swap_y = mSwapRect.TopY() + mPadPixels + cell_height/2;

    SizeType const tile_cnt = CountSwap();
    SizeType const stock_cnt = mpGame->CountStock();
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

        /*  
        If the active tile came from the board, draw it later 
        (not now) so it won't get obscured.
        */
        if (!IsActive(id) || !IsOnBoard(id)) {
            // inactive tile or off-board tile -- draw it now
            Point const base(x, y);
            DrawPlayableTile(rCanvas, base, tile, odd_flag); 
        }
    }
}

void GameView::DrawStockArea(
    Canvas& rCanvas,
    LogicalYType top_y,
    LogicalXType left_x,
    PixelCntType width)
{
    // Calculate the height of the stock area.
    PixelCntType const height = 2*rCanvas.TextHeight() + 3*mPadPixels;

    // Set the colors of the stock area.
    ColorType const area_color = COLOR_DARK_BLUE;
    ColorType const edge_color = COLOR_WHITE;
    rCanvas.UseColors(area_color, edge_color);

    // Draw the stock area.
    rCanvas.DrawRectangle(top_y, left_x, width, height);

    // Draw two lines of descriptive text in the stock area.
    SizeType const stock_cnt = mpGame->CountStock();
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
    Area const cell_area = CellArea(PLACE_HAND);
    PixelCntType const cell_height = cell_area.Height();
    SizeType const stock_cnt = mpGame->CountStock();
    SizeType tile_cnt = CountSwap();
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
    SizeType played_tile_cnt = CountPlayed();
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

    // Determine the tile's background color.
    ColorType tile_color = COLOR_LIGHT_GRAY;
    if (rTile.HasBonus()) {
        tile_color = COLOR_DULL_GOLD;
    }
    if (Contains(id) && IsLocalUsersTurn()) {
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
    Area const tile_area = TileArea(center);
    bool const warm_flag = IsWarmTile(id);
    Rect const result = rCanvas.DrawTile(markings, tile_color, center, tile_area, 
        warm_flag, oddFlag);

    if (Contains(id) && IsLocalUsersTurn()) {
        TilePair const pair(id, result);
        mTileMap.insert(pair);
    }
}

void GameView::DrawUnplayableHands(Canvas& rCanvas) {
    ASSERT(mpGame != NULL);

    Area const cell_area = CellArea(PLACE_BOARD);
    PixelCntType const cell_height = cell_area.Height();
    ColorType area_color = COLOR_DARK_BLUE;
    ColorType const edge_color = COLOR_LIGHT_GRAY;

    Hands hands = mpGame->UnplayableHands();
    Hands::Iterator i_hand;
    LogicalXType right_x = mpWindow->ClientArea().Width() - mPadPixels;
    LogicalYType top_y = mPadPixels;
    for (i_hand = hands.begin(); i_hand < hands.end(); i_hand++) {
        // Draw the header.
        bool const rightFlag = false;
        ColorType const header_color = COLOR_BLACK;
        Rect header_rect = DrawHandHeader(rCanvas, top_y, right_x, *i_hand, header_color, rightFlag);

        // Draw hand area below the header.
        top_y = header_rect.BottomY() - 1;
        LogicalXType const left_x = header_rect.LeftX();
        PixelCntType const width = header_rect.Width();
        Tiles const hand_tiles = *i_hand;
        SizeType const tile_count = hand_tiles.Count();
        area_color = COLOR_DARK_BLUE;
        rCanvas.UseColors(area_color, edge_color);
        PixelCntType height = rCanvas.TextHeight() + 2*mPadPixels;
        if (!i_hand->HasResigned() && !i_hand->HasGoneOut() && mpMenuBar->IsPeeking()) {
            height = tile_count*cell_height + 2*mPadPixels;
        }
        Rect const hand_rect = rCanvas.DrawRectangle(top_y, left_x, width, height);

        if (i_hand->IsDisconnected()) {
            rCanvas.DrawTextLine(hand_rect, "disconnected");
        } else if (i_hand->HasResigned()) {
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

        if (mRecenterRightX > left_x) {
            mRecenterRightX = left_x;
        }

        // Add padding between hands.
        if (mpMenuBar->IsPeeking()) {
            // right to left
            right_x = left_x - mPadPixels;
            top_y = mPadPixels;
        } else {
            // top to bottom
            right_x = mpWindow->ClientArea().Width() - mPadPixels;
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
    Area const cell_area = CellArea(PLACE_BOARD);
    PixelCntType const cell_width = cell_area.Width();

    PixelCntType result = 2;
    switch (Cell::Grid()) {
    case GRID_TRIANGLE:
        result = cell_width/2;
        break;
    case GRID_4WAY:
    case GRID_8WAY:
        result = cell_width;
        break;
    case GRID_HEX:
        result = PixelCntType(0.5 + 0.75*cell_width);
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
    Area const cell_area = CellArea(PLACE_BOARD);
    PixelCntType const cell_height = cell_area.Height();

    PixelCntType result = 2;
    switch (Cell::Grid()) {
    case GRID_TRIANGLE:
    case GRID_4WAY:
    case GRID_8WAY:
        result = cell_height;
        break;
    case GRID_HEX:
        result = cell_height/2;
        break;
    default:
        FAIL();
    }

    if (IsGridVisible()) {
        result -= 1;  // height of a grid line
    }

    return result;
}

void GameView::LoadUserOptions(User const& rUser) {
    mDisplayModes = DisplayModes(rUser);
    ASSERT(mDisplayModes.MarkingCnt() <= Markings::MARKING_CNT_MAX);

    mStartCell = Point(rUser);

    TileSizeType const board_tile_size = rUser.BoardTileSize();
    SetBoardTileSize(board_tile_size);
    TileSizeType const hand_tile_size = rUser.HandTileSize();
    SetHandTileSize(hand_tile_size);
}

void GameView::MoveTarget(int rows, int columns) {
    if (!mTargetCellFlag) {
        mTargetCell = Cell();
        mTargetCellFlag = true;
    }

    for (;;) {
        mTargetCell.Offset(rows, columns);
        if (mTargetCell.IsValid() && IsEmpty(mTargetCell)) {
            break;
        }
    }

    if (!MightUse(mTargetCell)) {
        mTargetCellFlag = false;
    }
}

void GameView::Recenter(void) {
    Area const client_area = mpWindow->ClientArea();
    if (mRecenterLeftX >= mRecenterRightX) {
        mRecenterLeftX = 0;
        mRecenterRightX = client_area.Width();
    }
    LogicalXType x = (mRecenterLeftX + mRecenterRightX)/2;
    LogicalYType y = client_area.Height()/2;

    Board const& r_board = BoardReference();
    x -= long(GridUnitX())*(r_board.EastMax() - r_board.WestMax())/2;
    y += long(GridUnitY())*(r_board.NorthMax() - r_board.SouthMax())/2;

    mStartCell = Point(x, y);
}

void GameView::Repaint(Canvas& rCanvas) {
    if (mpGame == NULL || mpWindow->IsWaiting()) {
        DrawIdle(rCanvas);

    } else if (IsGamePaused()) {
        DrawPaused(rCanvas);

    } else {
        Area const client_area = mpWindow->ClientArea();
        mRecenterLeftX = 0;
        mRecenterRightX = client_area.Width();
        mTileMap.clear();

        DrawBoard(rCanvas, 0);
        DrawBoard(rCanvas, 1);
        DrawUnplayableHands(rCanvas);
        DrawPlayableHand(rCanvas);
        DrawPlayableTiles(rCanvas);
        DrawBoard(rCanvas, 2);
    }

    rCanvas.Close();
}

void GameView::ResetTargetCell(void) {
    mTargetCellFlag = false;
}

void GameView::SaveUserOptions(User& rUser) const {
    rUser.SetDisplayModes(mDisplayModes);
    rUser.SetStartCellPosition(mStartCell);
    // tile size is saved from the menu bar
}

String GameView::ScoreText(Hand const& rHand, bool isPlayable) const {
    ASSERT(mpGame != NULL);

    ScoreType const score = rHand.Score();

    ScoreType points_this_turn = 0;
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

void GameView::SetBoardTileSize(TileSizeType size) {
    ASSERT(size >= TILE_SIZE_MIN);
    ASSERT(size <= TILE_SIZE_MAX);

    mBoardTileWidth = size * TinyWidth();

    ASSERT(mBoardTileWidth > 0);
    ASSERT(::is_even(mBoardTileWidth));
}

void GameView::SetDisplayModes(DisplayModes const& rDisplayModes) {
    mDisplayModes = rDisplayModes;
}

void GameView::SetGame(Game* pGame) {
    mpGame = pGame;

    if (pGame != NULL && pGame->Style() == GAME_STYLE_CHALLENGE) {
        Reset(mpGame, HINT_CHALLENGE_DEFAULT, 0.0);
    } else {
        Reset(mpGame, HINT_DEFAULT, 0.0);
    }

    Recenter();
    ResetTargetCell();
    mDisplayModes.Cleanup();
}

void GameView::SetHandTileSize(TileSizeType size) {
    ASSERT(size >= TILE_SIZE_MIN);
    ASSERT(size <= TILE_SIZE_MAX);

    mHandTileWidth = size * TinyWidth();

    ASSERT(mHandTileWidth > 0);
    ASSERT(::is_even(mHandTileWidth));
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

Area GameView::TileArea(PlaceType place) const {
    PixelCntType const width = TileWidth(place);
    PixelCntType height = 0;

    switch(Cell::Grid()) {
    case GRID_4WAY:
    case GRID_8WAY:
        height = width;
        break;
    case GRID_HEX:
    case GRID_TRIANGLE:
        height = PixelCntType((1 + SQRT_3*width)/2);
        break;
    default:
        FAIL();
    }
    Area const result(width, height);

    return result;
}

Area GameView::TileArea(Point const& rCenter) const {
    Area result = TileArea(PLACE_BOARD);

    if (mHandRect.Contains(rCenter) || mSwapRect.Contains(rCenter)) {
        result = TileArea(PLACE_HAND);
    }

    return result;
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

PixelCntType GameView::TileWidth(PlaceType place) const {
    PixelCntType result = 2;

    switch (place) {
    case PLACE_BOARD:
        result = mBoardTileWidth;
        break;
    case PLACE_HAND:
        result = mHandTileWidth;
        break;
    default:
        FAIL();
    }

    return result;
}

// tile width for the smallest size
/* static */  PixelCntType GameView::TinyWidth(void) {
    PixelCntType result = 0;

    switch(Cell::Grid()) {
    case GRID_4WAY:
    case GRID_8WAY:
        result = WIDTH_TINY_SQUARE;
        break;
    case GRID_HEX:
        result = WIDTH_TINY_HEX;
        break;
    case GRID_TRIANGLE:
        result = WIDTH_TINY_TRIANGLE;
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

    Area const cell_area = CellArea(PLACE_BOARD);
    bool const odd_flag = rCell.IsOdd();
    Rect const cell_rect = Canvas::InteriorGridShape(center, cell_area, odd_flag);

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
