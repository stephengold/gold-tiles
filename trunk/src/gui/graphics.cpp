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

#ifdef _QT
# include <QPainter>
#endif // defined(_QT)
#include <vector>
#include "gui/graphics.hpp"
#include "gui/poly.hpp"
#include "gui/window.hpp"
#ifdef _WINDOWS
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


// static data

Poly Graphics::msEquilateral;  // polygon for triangular cells and tiles
Poly Graphics::msHexagon;      // polygon for hexagonal cells and tiles


// lifecycle

Graphics::Graphics(Window& rWindow)
#ifdef _WINDOWS
:mRect(Rect(rWindow))
#endif // defined(_WINDOWS)
{
#ifdef _QT
    /*
      Since Qt 4.0, QWidget automatically
      double-buffers its painting.
     */
    mpDraw = new QPainter(&rWindow);
    ASSERT(mpDraw != NULL);

#elif defined(_WINDOWS)

    mDevice = rWindow.PaintDevice();
    mWindow = rWindow;

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

#endif // defined(_WINDOWS)

    // Create the default brush.
#ifdef _QT
    SetBrushColor(BRUSH_COLOR_DEFAULT);
#elif defined(_WINDOWS)
    mBrushColor = BRUSH_COLOR_DEFAULT;
    HBRUSH const brush = Win::CreateSolidBrush(mBrushColor);
    ASSERT(brush != NULL);
#endif // defined(_WINDOWS)

    // Initialize mFonts[].
    CreateFonts();

    // Create the default pen.
#ifdef _QT
    SetPenColor(PEN_COLOR_DEFAULT);
#elif defined(_WINDOWS)
    mPenColor = PEN_COLOR_DEFAULT;
    int const pen_style = PS_SOLID;
    int const pen_width = 0; // means single pixel
    HPEN const pen = Win::CreatePen(pen_style, pen_width, mPenColor);
    ASSERT(pen != NULL);

    // Save the original GDI handles for brush, font, and pen.
    mBrushSave = Win::SelectObject(mDraw, brush);
    ASSERT(mBrushSave != NULL);
    mFontSave = Win::SelectObject(mDraw, mFonts[0]);
    ASSERT(mFontSave != NULL);
    mPenSave = Win::SelectObject(mDraw, pen);
    ASSERT(mPenSave != NULL);
#endif // defined(_WINDOWS)

    // Initialize mFontHeight[] and mFontWidth[].
    MeasureFonts();

    // Initial size and colors for text.
    ColorType const background_color = BRUSH_COLOR_DEFAULT;
    SetBackgroundColor(background_color);

    ColorType const text_color = PEN_COLOR_DEFAULT;
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
#ifdef _WINDOWS
    HGDIOBJ const brush = Win::SelectObject(mDraw, mBrushSave);
    ASSERT(brush != NULL);
    BOOL was_successful = Win::DeleteObject(brush);
    ASSERT(was_successful);

    HGDIOBJ const pen = Win::SelectObject(mDraw, mPenSave);
    ASSERT(pen != NULL);
    was_successful = Win::DeleteObject(pen);
    ASSERT(was_successful);

    HGDIOBJ const font = Win::SelectObject(mDraw, mFontSave);
    ASSERT(font != NULL);
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        was_successful = Win::DeleteObject(mFonts[i_size]);
        ASSERT(was_successful);
    }

    if (mDraw != mDevice) {
        Win::DeleteDC(mDraw);
    }
    int const success = Win::ReleaseDC(mWindow, mDevice);
    ASSERT(success);
#endif // defined(_WINDOWS)
}


// misc methods

void Graphics::Close(void) {
#ifdef _QT
    mpDraw->end();
#elif defined(_WINDOWS)
    // Copy drawing buffer to device.
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
#endif // defined(_WINDOWS)
}

/* static */ COLORREF Graphics::ColorRef(ColorType color) {
    COLORREF result = PALETTERGB(0, 0, 0);

    switch (color) {
    case COLOR_BLACK:
        result = PALETTERGB(0, 0, 0);
        break;
    case COLOR_DARK_GRAY:
        result = PALETTERGB(80, 80, 80);
        break;
    case COLOR_MEDIUM_GRAY:
        result = PALETTERGB(128, 128, 128);
        break;
    case COLOR_LIGHT_GRAY:
        result = PALETTERGB(160, 160, 160);
        break;
    case COLOR_WHITE:
        result = PALETTERGB(255, 255, 255);
        break;
    case COLOR_RED:
        result = PALETTERGB(255, 0, 0);
        break;
    case COLOR_PINK:
        result = PALETTERGB(255, 128, 128);
        break;
    case COLOR_BROWN:
        result = PALETTERGB(120, 60, 0);
        break;
    case COLOR_YELLOW:
        result = PALETTERGB(255, 180, 0);
        break;
    case COLOR_DULL_GOLD:
        result = PALETTERGB(160, 140, 0);
        break;
    case COLOR_GOLD:
        result = PALETTERGB(255, 204, 0);
        break;
    case COLOR_DARK_GREEN:
        result = PALETTERGB(0, 100, 0);
        break;
    case COLOR_GREEN:
        result = PALETTERGB(0, 255, 0);
        break;
    case COLOR_LIGHT_GREEN:
        result = PALETTERGB(64, 255, 64);
        break;
    case COLOR_DARK_BLUE:
        result = PALETTERGB(0, 0, 160);
        break;
    case COLOR_LIGHT_BLUE:
        result = PALETTERGB(128, 128, 255);
        break;
    case COLOR_PURPLE:
        result = PALETTERGB(128, 0, 128);
        break;
    default:
        FAIL();
    }

    return result;
}

// create variable-width fonts in assorted sizes
void Graphics::CreateFonts(void) {
#ifdef _QT
    int const weight = (QFont::Normal + QFont::DemiBold)/2; // medium
    QString const family = "Tahoma";
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
        PixelCntType const height = NominalHeight(i_size);
        int const point_size = (50*height)/101;
        QFont* p_font = new QFont(family, point_size, weight);
        mpFonts[i_size] = p_font;
    }
#elif defined(_WINDOWS)
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
#endif // defined(_WINDOWS)
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
#ifdef _QT
    mpDraw->drawLine(x1, y1, x2, y2);
#elif defined(_WINDOWS)
    BOOL success = Win::MoveToEx(mDraw, x1, y1, NULL);
    ASSERT(success);
    success = Win::LineTo(mDraw, x2, y2);
    ASSERT(success);
#endif // defined(_WINDOWS)
}

void Graphics::DrawPolygon(
    Poly const& rPolygon,
    Rect const& rBounds,
    bool invertFlag,
    bool fillFlag)
{
    SizeType const point_cnt = rPolygon.Count();
#ifdef _QT
    QPoint* const points = new QPoint[point_cnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, point_cnt, rBounds, invertFlag);
    if (fillFlag) {
        mpDraw->drawPolygon(points, point_cnt);
    } else {
        mpDraw->drawPolyline(points, point_cnt);
    }
#elif defined(_WINDOWS)
    POINT* const points = new POINT[point_cnt];
    ASSERT(points != NULL);
    rPolygon.GetPoints(points, point_cnt, rBounds, invertFlag);
    BOOL success;
    if (fillFlag) {
        success = Win::Polygon(mDraw, points, point_cnt);
    } else {
        success = Win::Polyline(mDraw, points, point_cnt);
    }
    ASSERT(success);
#endif // defined(_WINDOWS)
    delete[] points;
}

Rect Graphics::DrawRectangle(Rect const& rRect) {
#ifdef _QT
    QRect const rect = QRect(rRect);
    mpDraw->drawRect(rect);
#elif defined(_WINDOWS)
    LogicalXType const left = rRect.LeftX();
    LogicalYType const top = rRect.TopY();
    LogicalXType const right = rRect.RightX();
    LogicalYType const bottom = rRect.BottomY();
    BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
    ASSERT(success);
#endif // defined(_WINDOWS)

    return rRect;
}

Rect Graphics::DrawRectangle(
    LogicalYType top,
    LogicalXType left,
    PixelCntType width,
    PixelCntType height)
{
#ifdef _QT
    QRect const rect = QRect(left, top, width, height);
    mpDraw->drawRect(rect);
#elif defined(_WINDOWS)
    LogicalXType const right = left + width;
    LogicalYType const bottom = top + height;
    BOOL const success = Win::Rectangle(mDraw, left, top, right, bottom);
    ASSERT(success);    
#endif // defined(_WINDOWS)
    Rect const result(top, left, width, height);

    return result;
}

void Graphics::DrawRoundedSquare(
    Point const& rCenter,
    PixelCntType edge,
    PixelCntType circleDiameter)
{
    ASSERT(edge > circleDiameter);

    LogicalXType const left_x = rCenter.X() - edge/2;
    LogicalYType const top_y = rCenter.Y() - edge/2;
#ifdef _QT
    qreal const radius = qreal(circleDiameter)/2;
    mpDraw->drawRoundedRect(left_x, top_y, edge, edge, radius, radius);
#elif defined(_WINDOWS)
    LogicalXType const right_x = left_x + edge;
    LogicalYType const bottom_y = top_y + edge;
    BOOL const success = Win::RoundRect(mDraw, left_x, top_y, right_x, bottom_y,
        circleDiameter, circleDiameter);
    ASSERT(success);
#endif // defined(_WINDOWS)
}

// Draw a single line of text, centered in a rectangle.
void Graphics::DrawTextLine(Rect const& rRect, TextType text, TextType altText) {
    ASSERT(text != NULL);

    TextType str = "";

    if (altText == NULL || TextWidth(text) <= rRect.Width()) {
        str = text;
    } else {
        str = altText;
    }

#ifdef _QT
    QRect const rect(rRect);
    int const flags = Qt::AlignCenter | Qt::TextSingleLine;
    mpDraw->drawText(rect, flags, str);
#elif defined(_WINDOWS)
    int const str_length = ::strlen(str);
    UINT const format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
        | DT_SINGLELINE | DT_VCENTER;
    RECT bounds = rRect;
    BOOL const success = Win::DrawText(mDraw, str, str_length, &bounds, format);
    ASSERT(success);
#endif // defined(_WINDOWS)
}

// Draw a multiple lines of text centered in a rectangle.
void Graphics::DrawTextMultiline(Rect const& rRect, TextType text) {
    ASSERT(text != NULL);

#ifdef _QT
    QRect const rect(rRect);
    int const flags = Qt::AlignCenter;
    QString const string(text);
    mpDraw->drawText(rect, flags, string);
#elif defined(_WINDOWS)
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
#endif // defined(_WINDOWS)
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

// Cache the actual height and width of each font.
void Graphics::MeasureFonts(void) {
    for (TextSizeType i_size = 0; i_size < TEXT_SIZE_CNT; i_size++) {
#ifdef _QT
        QFontMetrics const metrics(*mpFonts[i_size]);
        PixelCntType const height = metrics.height();
        PixelCntType const width = metrics.width('W');
#elif defined(_WINDOWS)
        SetTextSize(i_size);
        TEXTMETRIC metrics;
        BOOL const success = Win::GetTextMetrics(mDraw, &metrics);
        ASSERT(success);
        PixelCntType const height = metrics.tmHeight;
        PixelCntType const width = TextWidth("W");
#endif // defined(_WINDOWS)

        ASSERT(height >= FONT_HEIGHT_MIN);
        ASSERT(height <= NominalHeight(i_size));
        mFontHeight[i_size] = height;

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
        COLORREF const color_ref = ColorRef(color);
#ifdef _QT
        QBrush const brush(color_ref);
        mpDraw->setBackground(brush);
        mpDraw->setBackgroundMode(Qt::OpaqueMode);
#elif defined(_WINDOWS)
        COLORREF const old_color = Win::SetBkColor(mDraw, color_ref);
        ASSERT(old_color != CLR_INVALID);
        int const old_mode = SetBkMode(mDraw, OPAQUE);
        ASSERT(old_mode != 0);
#endif // defined(_WINDOWS)
    } else {
        // transparent background
#ifdef _QT
        mpDraw->setBackgroundMode(Qt::TransparentMode);
#elif defined(_WINDOWS)
        int const old_mode = SetBkMode(mDraw, TRANSPARENT);
        ASSERT(old_mode != 0);
#endif // defined(_WINDOWS)
    }

    mBackgroundColor = color;
}

void Graphics::SetBrushColor(ColorType color) {
    if (color != COLOR_TRANSPARENT) {
        // solid color brush
        COLORREF const color_ref = ColorRef(color);
#ifdef _QT
        QBrush const brush(color_ref);
        mpDraw->setBrush(brush);
#elif defined(_WINDOWS)
        HBRUSH const brush = Win::CreateSolidBrush(color_ref);
        ASSERT(brush != NULL);
        HGDIOBJ const old = Win::SelectObject(mDraw, brush);
        ASSERT(old != NULL);
        BOOL const success = Win::DeleteObject(old);
        ASSERT(success);
#endif // defined(_WINDOWS)
    } else {
        // TODO - transparent brush
    }

    mBrushColor = color;
}

void Graphics::SetPenColor(ColorType color) {
    ASSERT(color != COLOR_TRANSPARENT);

    COLORREF const color_ref = ColorRef(color);
#ifdef _QT
    QPen const pen(color_ref);
    mpDraw->setPen(pen);
#elif defined(_WINDOWS)
    int const pen_style = PS_SOLID;
    int const pen_width = 0; // means a pen one pixel wide
    HPEN const pen = Win::CreatePen(pen_style, pen_width, color_ref);
    ASSERT(pen != NULL);

    HGDIOBJ const old = Win::SelectObject(mDraw, pen);
    ASSERT(old != NULL);
    BOOL const success = Win::DeleteObject(old);
    ASSERT(success);
#endif // defined(_WINDOWS)

    mPenColor = color;
}

void Graphics::SetTextColor(ColorType color) {
    ASSERT(color != COLOR_TRANSPARENT);

#ifdef _WINDOWS
    COLORREF const color_ref = ColorRef(color);
    COLORREF const old_color = Win::SetTextColor(mDraw, color_ref);
    ASSERT(old_color != CLR_INVALID);
#endif // defined(_WINDOWS)

    mTextColor = color;
}

void Graphics::SetTextSize(TextSizeType size) {
    ASSERT(size <= TEXT_SIZE_MAX);

#ifdef _QT
    QFont const* const p_font = mpFonts[size];
    mpDraw->setFont(*p_font);
#elif defined(_WINDOWS)
    HFONT const font = mFonts[size];
    ASSERT(font != NULL);

    HGDIOBJ old_font = Win::SelectObject(mDraw, font);
    ASSERT(old_font != NULL);
#endif // defined(_WINDOWS)

    mTextSize = size;
}

PixelCntType Graphics::TextHeight(void) const {
#ifdef _QT
    QFontMetrics const metrics = mpDraw->fontMetrics();
    PixelCntType const result = metrics.height();
#elif defined(_WINDOWS)
    PixelCntType result = 0;
    if (mTextSize <= TEXT_SIZE_MAX) {
        result = mFontHeight[mTextSize];
    } else {
        TEXTMETRIC metrics;
        BOOL const success = Win::GetTextMetrics(mDraw, &metrics);
        ASSERT(success);
        result = metrics.tmHeight;
    }
#endif // defined(_WINDOWS)

    ASSERT(result >= FONT_HEIGHT_MIN);
    ASSERT(result <= FONT_HEIGHT_MAX);

    return result;
}

PixelCntType Graphics::TextWidth(TextType text) const {
#ifdef _QT
    QFontMetrics const metrics = mpDraw->fontMetrics();
    PixelCntType const result = metrics.width(text);
#elif defined(_WINDOWS)
    int const length = ::strlen(text);
    SIZE extent;
    BOOL const success = Win::GetTextExtentPoint32(mDraw, text, length, &extent);
    ASSERT(success);
    PixelCntType const result = extent.cx;
#endif // defined(_WINDOWS)

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


#ifdef _QT
# ifdef _GRAPHICS_TEST

// unit test

#  include <QApplication>
#  include "string.hpp"

class TestWindow: public Window {
protected:
    void paintEvent(QPaintEvent*);
};

void TestWindow::paintEvent(QPaintEvent* p_event) {
    p_event; // unused

    Graphics graphics(*this);
    graphics.UseColors(COLOR_BLACK, COLOR_BLACK);

    Rect const client_area(*this);
    graphics.DrawRectangle(client_area);

    graphics.UseColors(COLOR_BLACK, COLOR_GREEN);
    String const msg = "Type Ctrl+N to start a new game.";
    graphics.DrawTextMultiline(client_area, msg);

    graphics.UseColors(COLOR_DARK_BLUE, COLOR_WHITE);
    Rect const area(20, 20, 40, 40);
    graphics.DrawHexagon(area);

    graphics.UseColors(COLOR_GOLD, COLOR_MEDIUM_GRAY);
    Point const center(80, 30);
    graphics.DrawRoundedSquare(center, 40, 5);

    graphics.UseColors(COLOR_TRANSPARENT, COLOR_RED);
    LogicalYType y = 50;
    for (PixelCntType height = 10; height < 61; height += 10) {
        Rect const text_area(y, 50, 120, height);
        graphics.UseFont(height);
        graphics.DrawTextLine(text_area, "Long version", "L");
        y = text_area.BottomY();
    }
    graphics.Close();
}

int main(int argCnt, char** argValues) {
    // Instantiate a test window and display it.
    QApplication application(argCnt, argValues);

    TestWindow* p_window = new TestWindow;
    p_window->show();

    int exit_code = application.exec();

    return exit_code;
}

# endif // defined(_GRAPHICS_TEST)
#endif // defined(_QT)
