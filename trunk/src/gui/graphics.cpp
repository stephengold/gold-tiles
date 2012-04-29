// File:     graphics.cpp
// Location: src/gui
// Purpose:  implement Graphics class
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
#include <vector>
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/win_types.hpp"
#include "gui/window.hpp"


// static data

Poly Graphics::msEquilateral;  // polygon for triangular cells and tiles
Poly Graphics::msHexagon;      // polygon for hexagonal cells and tiles


// lifecycle

Graphics::Graphics(HDC device, Window& rWindow, bool releaseMe, bool bufferFlag):
mRect(Rect(rWindow))
{
    mDevice = device;
    mWindow = rWindow;
    mReleaseMe = releaseMe;

    if (!bufferFlag) {
        mDraw = device;

    } else { // double buffering
        mDraw = Win::CreateCompatibleDC(mDevice);
        ASSERT(mDraw != NULL);
        ASSERT(mDraw != mDevice);

        PixelCntType const width = mRect.Width();
        PixelCntType const height = mRect.Height();
        HGDIOBJ const bitmap = Win::CreateCompatibleBitmap(mDevice, width, height);
        ASSERT(bitmap != NULL);

        // save original GDI handle
        mBitmapSave = Win::SelectObject(mDraw, bitmap);
        ASSERT(mBitmapSave != NULL);
    }

    // create brush
    mBrushColor = COLOR_WHITE;
    HBRUSH const brush = Win::CreateSolidBrush(mBrushColor);
    ASSERT(brush != NULL);

    // initialize mFonts[]
    CreateFonts();

    // create pen
    mPenColor = COLOR_BLACK;
    int const pen_style = PS_SOLID;
    int const pen_width = 0; // means single pixel
    HPEN const pen = Win::CreatePen(pen_style, pen_width, mPenColor);
    ASSERT(pen != NULL);

    // save the original GDI handles for brush, font, and pen
    mBrushSave = Win::SelectObject(mDraw, brush);
    ASSERT(mBrushSave != NULL);
    mFontSave = Win::SelectObject(mDraw, mFonts[0]);
    ASSERT(mFontSave != NULL);
    mPenSave = Win::SelectObject(mDraw, pen);
    ASSERT(mPenSave != NULL);

    // initialize mFontHeight[] and mFontWidth[]
    MeasureFonts();

    // initial size and colors for text
    ColorType const background_color = mBrushColor;
    SetBackgroundColor(background_color);

    ColorType const text_color = mPenColor;
    SetTextColor(text_color);

    TextSizeType const text_size = FindTextSize(FONT_HEIGHT_DEFAULT);
    SetTextSize(text_size);

    // static polygons initialized on first instantiation of this class
    if (msEquilateral.Count() == 0) {
        InitializeEquilateral();
    }
    if (msHexagon.Count() == 0) {
        InitializeHexagon();
    }
}

/* virtual */ Graphics::~Graphics(void) {
    HGDIOBJ const brush = Win::SelectObject(mDraw, mBrushSave);
    ASSERT(brush != NULL);
    BOOL success = Win::DeleteObject(brush);
    ASSERT(success);

    HGDIOBJ const pen = Win::SelectObject(mDraw, mPenSave);
    ASSERT(pen != NULL);
    success = Win::DeleteObject(pen);
    ASSERT(success);

    HGDIOBJ const font = Win::SelectObject(mDraw, mFontSave);
    ASSERT(font != NULL);
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        success = Win::DeleteObject(mFonts[i_size]);
        ASSERT(success);
    }

    if (mDraw != mDevice) {
        Win::DeleteDC(mDraw);
    }
    if (mReleaseMe) {
        int const success = Win::ReleaseDC(mWindow, mDevice);
        ASSERT(success);
    }
}


// misc methods

void Graphics::Close(void) {
    if (mDraw != mDevice) {
        // copy buffer to device
        LogicalXType const dest_x = 0;
        LogicalYType const dest_y = 0;
        PixelCntType const width = mRect.Width();
        PixelCntType const height = mRect.Height();
        LogicalXType const source_x = 0;
        LogicalYType const source_y = 0;
        DWORD const options = SRCCOPY;
        BOOL success = Win::BitBlt(mDevice, dest_x, dest_y, width, height, 
            mDraw, source_x, source_y, options);
        ASSERT(success);

        HGDIOBJ const bitmap = Win::SelectObject(mDraw, mBitmapSave);
        ASSERT(bitmap != NULL);
        success = Win::DeleteObject(bitmap);
        ASSERT(success);
    }
}

// create variable-width fonts in assorted sizes
void Graphics::CreateFonts(void) {
    int const orientation = 0;
    int const weight = FW_MEDIUM;
    DWORD const not = FALSE;
    LPCTSTR const face_name = "Tahoma";
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        PixelCntType const height = NominalHeight(i_size);
        PixelCntType const width = height/3;  // narrow
        HFONT const font = Win::CreateFont(
            height, width, orientation, orientation, weight,
            not /* italic */, not /* underlined */, not /* strikeout */,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
            (VARIABLE_PITCH | FF_SWISS), face_name);
        ASSERT(font != NULL);

        mFonts[i_size] = font;
    }
}

void Graphics::DrawEquilateral(Rect const& rBounds, bool pointDownFlag) {
    ASSERT(msEquilateral.Count() == 3);

    DrawPolygon(msEquilateral, rBounds, pointDownFlag, true);
}

void Graphics::DrawHexagon(Rect const& rBounds) {
    ASSERT(msHexagon.Count() == 6);

    DrawPolygon(msHexagon, rBounds);
}

void Graphics::DrawLine(Point const& rPoint1, Point const& rPoint2) {
    LogicalXType const x1 = rPoint1.X();
    LogicalYType const y1 = rPoint1.Y();
    LogicalXType const x2 = rPoint2.X();
    LogicalYType const y2 = rPoint2.Y();

    DrawLine(x1, y1, x2, y2);
}

void Graphics::DrawLine(
    LogicalXType x1,
    LogicalYType y1,
    LogicalXType x2,
    LogicalYType y2)
{
    BOOL success = Win::MoveToEx(mDraw, x1, y1, NULL);
    ASSERT(success);

    success = Win::LineTo(mDraw, x2, y2);
    ASSERT(success);
}

void Graphics::DrawPolygon(Poly const& rPolygon, Rect const& rBounds, bool invertFlag, bool fillFlag) {
    unsigned const pointCnt = rPolygon.Count();
    POINT* const points = new POINT[pointCnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, pointCnt, rBounds, invertFlag);

    BOOL success;
    if (fillFlag) {
        success = Win::Polygon(mDraw, points, pointCnt);
    } else {
        success = Win::Polyline(mDraw, points, pointCnt);
    }
    ASSERT(success);

    delete[] points;
} 

Rect Graphics::DrawRectangle(Rect const& rRect) {
    LogicalXType const left = rRect.LeftX();
    LogicalYType const top = rRect.TopY();
    LogicalXType const right = rRect.RightX();
    LogicalYType const bottom = rRect.BottomY();
    BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
    ASSERT(success);

    return rRect;
}

Rect Graphics::DrawRectangle(
    LogicalYType top,
    LogicalXType left,
    PixelCntType width,
    PixelCntType height)
{
    LogicalXType const right = left + width;
    LogicalYType const bottom = top + height;
    BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
    ASSERT(success);

    Rect const result(top, left, width, height);

    return result;
}

void Graphics::DrawRoundedSquare(
    Point const& rCenter,
    PixelCntType edge,
    PixelCntType circleDiameter)
{
    ASSERT(edge > circleDiameter);

    PixelCntType const ellipse_width = circleDiameter;
    PixelCntType const ellipse_height = circleDiameter;
    LogicalXType const left_x = rCenter.X() - edge/2;
    LogicalXType const right_x = left_x + edge;
    LogicalYType const top_y = rCenter.Y() - edge/2;
    LogicalYType const bottom_y = top_y + edge;
    BOOL const success = Win::RoundRect(mDraw, left_x, top_y, right_x, bottom_y,
        ellipse_width, ellipse_height);
    ASSERT(success);
}

// Draw a single line of text centered in a rectangle
void Graphics::DrawTextLine(Rect const& rRect, TextType text, TextType altText) {
    ASSERT(text != NULL);

    TextType str = "";

    if (altText == NULL || TextWidth(text) <= rRect.Width()) {
        str = text;
    } else {
        str = altText;
    }

    int const str_length = ::strlen(str);
    UINT const format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
        | DT_SINGLELINE | DT_VCENTER;
    RECT bounds = rRect;
    BOOL const success = Win::DrawText(mDraw, str, str_length, &bounds, format);
    ASSERT(success);
}

// Draw a multiple lines of text centered in a rectangle
void Graphics::DrawTextMultiline(Rect const& rRect, TextType text) {
    ASSERT(text != NULL);

    int const text_length = ::strlen(text);
    int line_cnt = 1;
    for (TextType p_char = text; *p_char != '\0'; p_char++) {
        if (*p_char == '\n') {
            line_cnt++;
        }
    }
    PixelCntType const text_height = line_cnt * TextHeight();
    Rect const rect = rRect.ShrinkHeight(text_height);

    UINT const format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX;
    RECT bounds = rect;
    BOOL const success = Win::DrawText(mDraw, text, text_length, &bounds, format);
    ASSERT(success);
}

Graphics::TextSizeType Graphics::FindTextSize(PixelCntType maxHeight) {
    ASSERT(maxHeight >= 8);

    TextSizeType result = 0;
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        if (mFontHeight[i_size] <= maxHeight) {
            result = i_size;
        }
    }

    return result;
}

Graphics::TextSizeType Graphics::FindTextSize(
    PixelCntType maxHeight,
    PixelCntType maxWidth)
{
    ASSERT(maxHeight >= 3);
    ASSERT(maxWidth >= 3);

    TextSizeType result = 0;
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        if (mFontHeight[i_size] <= maxHeight
            && mFontWidth[i_size] <= maxWidth) {
                result = i_size;
        }
    }

    return result;
}

/* static */ void Graphics::InitializeEquilateral(void) {
    ASSERT(msEquilateral.Count() == 0);

    msEquilateral.Add(0.0, 0.0);
    msEquilateral.Add(0.5, 1.0);
    msEquilateral.Add(1.0, 0.0);

    ASSERT(msEquilateral.Count() == 3);
}

/* static */ void Graphics::InitializeHexagon(void) {
    ASSERT(msHexagon.Count() == 0);

    msHexagon.Add(0.0, 0.5);
    msHexagon.Add(0.25, 1.0);
    msHexagon.Add(0.75, 1.0);
    msHexagon.Add(1.0, 0.5);
    msHexagon.Add(0.75, 0.0);
    msHexagon.Add(0.25, 0.0);

    ASSERT(msHexagon.Count() == 6);
}

Rect Graphics::InteriorEquilateral(Rect const& rBounds, bool pointDownFlag) {
    FractionPair pair_ulc(0,0);
    FractionPair pair_brc(0,0);
    if (pointDownFlag) {
        pair_ulc = FractionPair(0.291, 0.900);
        pair_brc = FractionPair(0.709, 0.418);
    } else {
        pair_ulc = FractionPair(0.291, 0.582);
        pair_brc = FractionPair(0.709, 0.100);
    }

    Point const ulc = rBounds.Interpolate(pair_ulc);
    Point const brc = rBounds.Interpolate(pair_brc);

    Rect const result(ulc, brc);

    return result;
}

Rect Graphics::InteriorHexagon(Rect const& rBounds) {
    double const den = 4 + 4*SQRT_3;
    FractionPair const pair_ulc(2/den, (6 + 2*SQRT_3)/den);
    Point const ulc = rBounds.Interpolate(pair_ulc);

    FractionPair const pair_brc((2 + 4*SQRT_3)/den, (2*SQRT_3 - 2)/den);
    Point const brc = rBounds.Interpolate(pair_brc);

    Rect const result(ulc, brc);

    return result;
}

Rect Graphics::InteriorRoundedSquare(
    Point const& rCenter,
    PixelCntType edge,
    PixelCntType circleDiameter)
{
    LogicalXType left_x = rCenter.X() - edge/2;
    LogicalYType top_y = rCenter.Y() - edge/2;

    // estimate the interior sqaure
    PixelCntType const radius = circleDiameter/2;
    PixelCntType pad = radius - PixelCntType(0.7 * float(radius));
    top_y += pad;
    left_x += pad;
    edge -= 2*pad;
    Rect const result(top_y, left_x, edge, edge);

    return result;
}

// cache the actual height and width of each font
void Graphics::MeasureFonts(void) {
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        SetTextSize(i_size);

        TEXTMETRIC metrics;
        BOOL const success = Win::GetTextMetrics(mDraw, &metrics);
        ASSERT(success);

        PixelCntType const height = metrics.tmHeight;
        ASSERT(height >= FONT_HEIGHT_MIN);
        ASSERT(height <= NominalHeight(i_size));
        mFontHeight[i_size] = height;

        PixelCntType const width = TextWidth("W");
        ASSERT(width >= FONT_WIDTH_MIN);
        ASSERT(width <= 3*height);
        mFontWidth[i_size] = width;
    }
}

/* static */ PixelCntType Graphics::NominalHeight(TextSizeType size) {
    ASSERT(size <= TEXT_SIZE_MAX);

    PixelCntType const result = 4*(size + 2);

    ASSERT(result >= FONT_HEIGHT_MIN);
    ASSERT(result <= FONT_HEIGHT_MAX);
    return result;
}

void Graphics::SetBackgroundColor(ColorType color) {
    if (color != COLOR_TRANSPARENT) {
        // solid color background
        COLORREF const old_color = Win::SetBkColor(mDraw, color);
        ASSERT(old_color != CLR_INVALID);
        int const old_mode = SetBkMode(mDraw, OPAQUE);
        ASSERT(old_mode != 0);
    } else {
        // transparent background
        int const old_mode = SetBkMode(mDraw, TRANSPARENT);
        ASSERT(old_mode != 0);
    }

    mBackgroundColor = color;
}

void Graphics::SetBrushColor(ColorType color) {
    if (color != COLOR_TRANSPARENT) {
        // solid color brush
        HBRUSH const brush = Win::CreateSolidBrush(color);
        ASSERT(brush != NULL);

        HGDIOBJ const old = Win::SelectObject(mDraw, brush);
        ASSERT(old != NULL);
        BOOL const success = Win::DeleteObject(old);
        ASSERT(success);
    } else {
        // TODO - transparent brush
    }

    mBrushColor = color;
}

void Graphics::SetPenColor(ColorType color) {
    ASSERT(color != COLOR_TRANSPARENT);

    int const pen_style = PS_SOLID;
    int const pen_width = 0; // means a pen one pixel wide
    HPEN const pen = Win::CreatePen(pen_style, pen_width, color);
    ASSERT(pen != NULL);

    HGDIOBJ const old = Win::SelectObject(mDraw, pen);
    ASSERT(old != NULL);
    BOOL const success = Win::DeleteObject(old);
    ASSERT(success);

    mPenColor = color;
}

void Graphics::SetTextColor(ColorType color) {
    ASSERT(color != COLOR_TRANSPARENT);

    COLORREF const old_color = Win::SetTextColor(mDraw, color);
    ASSERT(old_color != CLR_INVALID);

    mTextColor = color;
}

void Graphics::SetTextSize(TextSizeType size) {
    ASSERT(size <= TEXT_SIZE_MAX);

    HFONT const font = mFonts[size];
    ASSERT(font != NULL);

    HGDIOBJ old_font = Win::SelectObject(mDraw, font);
    ASSERT(old_font != NULL);

    mTextSize = size;
}

PixelCntType Graphics::TextHeight(void) const {
    PixelCntType result = FONT_HEIGHT_MIN;
    if (mTextSize <= TEXT_SIZE_MAX) {
        result = mFontHeight[mTextSize];
    } else {
        TEXTMETRIC metrics;
        BOOL const success = Win::GetTextMetrics(mDraw, &metrics);
        ASSERT(success);
        result = metrics.tmHeight;
    }

    ASSERT(result >= FONT_HEIGHT_MIN);
    ASSERT(result <= FONT_HEIGHT_MAX);

    return result;
}

PixelCntType Graphics::TextWidth(TextType text) const {
    int const length = ::strlen(text);
    SIZE extent;
    BOOL const success = Win::GetTextExtentPoint32(mDraw, text, length, &extent);
    ASSERT(success);
    PixelCntType const result = extent.cx;

    return result;
}

void Graphics::UseBrushBackgroundColors(ColorType color) {
    if (color != mBrushColor) {
        SetBrushColor(color);
    }
    if (color != mBackgroundColor) {
        SetBackgroundColor(color);
    }

    ASSERT(mBrushColor == color);
    ASSERT(mBackgroundColor == color);
}

void Graphics::UseColors(ColorType brushBackgroundColor, ColorType penTextColor) {
    UseBrushBackgroundColors(brushBackgroundColor);
    UsePenTextColors(penTextColor);
}

void Graphics::UseFont(PixelCntType maxHeight) {
    TextSizeType const best_size = FindTextSize(maxHeight);

    if (best_size != mTextSize) {
        SetTextSize(best_size);
    }
    ASSERT(mTextSize == best_size);
}

void Graphics::UseFont(PixelCntType maxHeight, PixelCntType maxWidth) {
    TextSizeType const best_size = FindTextSize(maxHeight, maxWidth);

    if (best_size != mTextSize) {
        SetTextSize(best_size);
    }
    ASSERT(mTextSize == best_size);
}

void Graphics::UsePenTextColors(ColorType color) {
    ASSERT(color != COLOR_TRANSPARENT);

    if (color != mPenColor) {
        SetPenColor(color);
    }
    if (color != mTextColor) {
        SetTextColor(color);
    }

    ASSERT(mPenColor == color);
    ASSERT(mTextColor == color);
}
#endif // defined(_WINDOWS)
