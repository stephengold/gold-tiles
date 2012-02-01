// File:    graphics.cpp
// Purpose: Graphics class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/color.hpp"
#include "gui/graphics.hpp"
#include "gui/poly.hpp"

Rect::Rect(int topY, int leftX, unsigned width, unsigned height) {
    _bounds.top = topY;
    _bounds.left = leftX;
    _bounds.right = leftX + width;
    _bounds.bottom = topY + height;
}

Rect Rect::centerSquare(void) const {
    unsigned width = getWidth();
    unsigned height = getHeight();
    int left = getLeftX();
    int top = getTopY();
    
    if (width > height) {
        left += (width - height)/2;
        width = height;
    } else if (height > width) {
        top += (height - width)/2;
        height = width;
    } 
    Rect result(top, left, width, height);
    
    return result;
}

bool Rect::contains(POINT const &point) const {
    unsigned dx = point.x - _bounds.left;
    unsigned dy = point.y - _bounds.top;
    bool result = (dx < getWidth() && dy < getHeight());
    
    return result; 
}

int Rect::getBottomY(void) const {
    int result = _bounds.bottom;
    
    return result;
}

unsigned Rect::getHeight(void) const {
    ASSERT(_bounds.bottom >= _bounds.top);
    unsigned result = _bounds.bottom - _bounds.top;
    
    return result;
}

int Rect::getLeftX(void) const {
    int result = _bounds.left;
    
    return result;
}

int Rect::getTopY(void) const {
    int result = _bounds.top;
    
    return result;
}

unsigned Rect::getWidth(void) const {
    ASSERT(_bounds.right >= _bounds.left);
    unsigned result = _bounds.right - _bounds.left;
    
    return result;
}

Rect::operator RECT(void) const {
    RECT result = _bounds;
    
    return result;
}

Graphics::Graphics(
    HDC device,
    HWND window,
    bool releaseMe,
    bool bufferFlag,
    unsigned width,
    unsigned height)
{
    _device = device;
    _window = window;
    _releaseMe = releaseMe;
    _height = height;
    _width = width;
    
    if (!bufferFlag) {
        _draw = device;

    } else { // double buffering
        _draw = ::CreateCompatibleDC(_device);
        ASSERT(_draw != NULL);
        ASSERT(_draw != _device);

        HGDIOBJ bitmap = ::CreateCompatibleBitmap(_device, _width, _height);
        ASSERT(bitmap != NULL);
        _bitmapSave = ::SelectObject(_draw, bitmap);
        ASSERT(_bitmapSave != NULL);
    }

    // brush color for filling shapes
    _brushBkColor = WHITE_COLOR;
    HBRUSH brush = ::CreateSolidBrush(_brushBkColor);
    ASSERT(brush != NULL);
    _brushSave = ::SelectObject(_draw, brush);
    ASSERT(_brushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    COLORREF success = ::SetBkColor(_draw, _brushBkColor);
    ASSERT(success != CLR_INVALID);

    // foreground color for text and broken lines
    // (same as pen color)
    success = ::SetTextColor(_draw, _penTextColor);
    ASSERT(success != CLR_INVALID);

    // pen color for outlining shapes
    int penStyle = PS_SOLID;
    int penWidth = 0; // means single pixel
    _penTextColor = BLACK_COLOR;
    HPEN pen = ::CreatePen(penStyle, penWidth, _penTextColor);
    ASSERT(pen != NULL);
    _penSave = ::SelectObject(_draw, pen);
    ASSERT(_penSave != NULL);
}

Graphics::~Graphics(void) {
    HGDIOBJ brush = ::SelectObject(_draw, _brushSave);
    ASSERT(brush != NULL);
    BOOL success = ::DeleteObject(brush);
    ASSERT(success != 0);

    HGDIOBJ pen = ::SelectObject(_draw, _penSave);
    ASSERT(pen != NULL);
    success = ::DeleteObject(pen);
    ASSERT(success != 0);

    if (_draw != _device) {
        ::DeleteDC(_draw);
    }
    if (_releaseMe) {
        int success = ::ReleaseDC(_window, _device);
        ASSERT(success != 0);
    }
}

void Graphics::close(void) {
    if (_draw != _device) {
        int destX = 0;
        int destY = 0;
        int sourceX = 0;
        int sourceY = 0;
        DWORD options = SRCCOPY;
        BOOL success = ::BitBlt(_device, destX, destY, _width, _height, 
                                _draw, sourceX, sourceY, options);
        ASSERT(success != 0);
         
        HGDIOBJ bitmap = ::SelectObject(_draw, _bitmapSave);
        ASSERT(bitmap != NULL);
        success = ::DeleteObject(bitmap);
        ASSERT(success != 0);
    }
}

void Graphics::drawPolygon(Poly const &polygon, Rect const &bounds) {
    Rect squared = bounds.centerSquare();
    
    unsigned numberOfPoints = polygon.size();
    POINT *points = new POINT[numberOfPoints];
    ASSERT(points != NULL);
    polygon.getPoints(points, numberOfPoints, squared);

    BOOL success = ::Polygon(_draw, points, numberOfPoints);
    ASSERT(success);
} 

Rect Graphics::drawRectangle(
    int top,
    int left,
    unsigned width,
    unsigned height)
{
	int right = left + width;
	int bottom = top + height;
	BOOL success = ::Rectangle(_draw, left, top, right, bottom);
	ASSERT(success != 0);
	
	Rect result(top, left, width, height);
	
	return result;
}

Rect Graphics::drawRoundedSquare(
    int top,
    int left,
    unsigned edge,
    unsigned circleDiameter)
{
    ASSERT(edge > circleDiameter);
    
    unsigned ellipseWidth = circleDiameter;
    unsigned ellipseHeight = circleDiameter;
    int bottom = top + edge;
    int right = left + edge;
    BOOL success = ::RoundRect(_draw, left, top, right, bottom,
                             ellipseWidth, ellipseHeight);
    ASSERT(success != 0);
        
    Rect result(top, left, edge, edge);
    
    return result;
}

void Graphics::drawText(Rect const &rect, char const *text) {
    int textLen = strlen(text);
     
    UINT format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    RECT r = (RECT)rect;
    BOOL success = ::DrawText(_draw, text, textLen, &r, format);
    ASSERT(success != 0);
}

void Graphics::drawText(Rect const &rect, string text) {
    unsigned length = text.size();
    char *copyText = new char[length + 1];
    strcpy(copyText, text.c_str());
    drawText(rect, copyText);
    delete[] copyText;
}

void Graphics::getColors(COLORREF &brushBkColor, COLORREF &penTextColor) const {
    penTextColor = _penTextColor;
    brushBkColor = _brushBkColor;
}

unsigned Graphics::getTextHeight(void) const {
    unsigned result = 16;
    
    return result;
}

unsigned Graphics::getTextWidth(char const *text) const {
    int length = strlen(text);
    SIZE extent;
    BOOL success = ::GetTextExtentPoint32(_draw, text, length, &extent);
    unsigned result = extent.cx;
    
    return result;
}

unsigned Graphics::getTextWidth(string text) const {
    unsigned length = text.size();
    char *copyText = new char[length + 1];
    strcpy(copyText, text.c_str());
    unsigned result = getTextWidth(copyText);
    delete[] copyText;
    return result;
}

void Graphics::useColors(COLORREF brushBkColor, COLORREF penTextColor) {
    if (brushBkColor != _brushBkColor) {
        HBRUSH brush = ::CreateSolidBrush(brushBkColor);
        ASSERT(brush != NULL);
        HGDIOBJ old = ::SelectObject(_draw, brush);
        ASSERT(old != NULL);
        BOOL success = ::DeleteObject(old);
        ASSERT(success != 0);

        COLORREF oldColor = ::SetBkColor(_draw, brushBkColor);
        ASSERT(oldColor != CLR_INVALID);

        _brushBkColor = brushBkColor;
    }
       
    if (penTextColor != _penTextColor) {
        COLORREF oldColor = ::SetTextColor(_draw, penTextColor);
        ASSERT(oldColor != CLR_INVALID);

        int penStyle = PS_SOLID;
        int penWidth = 0; // means a pen one pixel wide
        HPEN pen = ::CreatePen(penStyle, penWidth, penTextColor);
        ASSERT(pen != NULL);
        HGDIOBJ old = ::SelectObject(_draw, pen);
        ASSERT(old != NULL);
        BOOL success = ::DeleteObject(old);
        ASSERT(success != 0);
        
        _penTextColor = penTextColor;
    }
}

#endif
