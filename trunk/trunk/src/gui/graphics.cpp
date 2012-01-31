// File:    graphics.cpp
// Purpose: Graphics class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/color.hpp"
#include "gui/graphics.hpp"

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
    COLORREF brushColor = WHITE_COLOR;
    HBRUSH brush = ::CreateSolidBrush(brushColor);
    ASSERT(brush != NULL);
    _brushSave = ::SelectObject(_draw, brush);
    ASSERT(_brushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    COLORREF success = ::SetBkColor(_draw, brushColor);
    ASSERT(success != CLR_INVALID);

    // pen color for outlining shapes
    int penStyle = PS_SOLID;
    int penWidth = 0; // means single pixel
    COLORREF penColor = BLACK_COLOR;
    HPEN pen = ::CreatePen(penStyle, penWidth, penColor);
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

    if (_releaseMe) {
        int success = ::ReleaseDC(_window, _device);
        ASSERT(success != 0);
    }
    if (_draw != _device) {
        ::DeleteDC(_draw);
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
    HBRUSH brush = ::CreateSolidBrush(brushBkColor);
    ASSERT(brush != NULL);
    HGDIOBJ old = ::SelectObject(_draw, brush);
    ASSERT(old != NULL);
    BOOL success = ::DeleteObject(old);
    ASSERT(success != 0);

    COLORREF oldColor = ::SetBkColor(_draw, brushBkColor);
    ASSERT(oldColor != CLR_INVALID);
       
    oldColor = ::SetTextColor(_draw, penTextColor);
    ASSERT(oldColor != CLR_INVALID);

    int penStyle = PS_SOLID;
    int penWidth = 0; // means a pen one pixel wide
    HPEN pen = ::CreatePen(penStyle, penWidth, penTextColor);
    ASSERT(pen != NULL);
    old = ::SelectObject(_draw, pen);
    ASSERT(old != NULL);
    success = ::DeleteObject(old);
    ASSERT(success != 0);
}

RECT Graphics::drawRectangle(int top, int left, unsigned width, unsigned height) {
	int right = left + width;
	int bottom = top + height;
	BOOL success = ::Rectangle(_draw, left, top, right, bottom);
	ASSERT(success != 0);
	
	RECT result;
	result.left = left;
	result.right = right;
	result.top = top;
	result.bottom = bottom;
	
	return result;
}

RECT Graphics::drawRoundedSquare(
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
        
    RECT result;
	result.left = left;
    result.right = right;
    result.top = top;
    result.bottom = bottom;
    
    return result;
}

void Graphics::drawText(
    int topY, 
    int leftX, 
    unsigned width, 
    unsigned height, 
    char const *text)
{
    int textLen = strlen(text);
     
	RECT rect;
	rect.top = topY;
	rect.left = leftX;
    rect.bottom = rect.top + height;
    rect.right = rect.left + width;

    UINT format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    BOOL success = ::DrawText(_draw, text, textLen, &rect, format);
    ASSERT(success != 0);
}

void Graphics::drawText(
    int topY, 
    int leftX, 
    unsigned width, 
    unsigned height, 
    string text)
{
    unsigned length = text.size();
    char *copyText = new char[length + 1];
    strcpy(copyText, text.c_str());
    drawText(topY, leftX, width, height, copyText);
    delete[] copyText;
}

#endif
