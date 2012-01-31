// File:    canvas.cpp
// Purpose: Canvas class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/canvas.hpp"

DevContext::DevContext(HDC context, HWND window, bool releaseMe) {
    _contextHandle = context;
    _windowHandle = window;
    _releaseMe = releaseMe;

    // brush color for filling shapes
    COLORREF brushColor = WHITE_COLOR;
    HBRUSH brush = ::CreateSolidBrush(brushColor);
    assert(brush != NULL);
    _brushSave = ::SelectObject(_contextHandle, brush);
    assert(_brushSave != NULL);
    
    // background color for text and broken lines
    // (same as brush color)
    COLORREF success = ::SetBkColor(_contextHandle, brushColor);
    assert(success != CLR_INVALID);

    // pen color for outlining shapes
    int penStyle = PS_SOLID;
    int penWidth = 0; // means single pixel
    COLORREF penColor = BLACK_COLOR;
    HPEN pen = ::CreatePen(penStyle, penWidth, penColor);
    assert(pen != NULL);
    _penSave = ::SelectObject(_contextHandle, pen);
    assert(_penSave != NULL);
}

DevContext::~DevContext(void) {
    HGDIOBJ brush = ::SelectObject(_contextHandle, _brushSave);
    assert(brush != NULL);
    BOOL success = ::DeleteObject(brush);
    assert(success != 0);

    HGDIOBJ pen = ::SelectObject(_contextHandle, _penSave);
    assert(pen != NULL);
    success = ::DeleteObject(pen);
    assert(success != 0);

    if (_releaseMe) {
        int success = ::ReleaseDC(_windowHandle, _contextHandle);
        assert(success != 0);
    }
}

DevContext::operator HDC(void) const {
    return _contextHandle;
}

void DevContext::useColors(COLORREF brushBkColor, COLORREF penTextColor) {
    HBRUSH brush = ::CreateSolidBrush(brushBkColor);
    assert(brush != NULL);
    HGDIOBJ old = ::SelectObject(_contextHandle, brush);
    assert(old != NULL);
    BOOL success = ::DeleteObject(old);
    assert(success != 0);

    COLORREF oldColor = ::SetBkColor(_contextHandle, brushBkColor);
    assert(oldColor != CLR_INVALID);   
    oldColor = ::SetTextColor(_contextHandle, penTextColor);
    assert(oldColor != CLR_INVALID);

    int penStyle = PS_SOLID;
    int penWidth = 0; // means a pen one pixel wide
    HPEN pen = ::CreatePen(penStyle, penWidth, penTextColor);
    assert(pen != NULL);
    old = ::SelectObject(_contextHandle, pen);
    assert(old != NULL);
    success = ::DeleteObject(old);
    assert(success != 0);
}

Canvas::Canvas(HDC contextHandle, HWND windowHandle, bool releaseMe):
    DevContext(contextHandle, windowHandle, releaseMe)
{}

void Canvas::drawCell(int top, int left, unsigned width, COLORREF cellColor,
          COLORREF gridColor)
{
    useColors(cellColor, gridColor);
    int height = width;
    int bottom = top + height;
    int right = left + width;
    BOOL success = ::Rectangle((HDC)*this, left, top, right, bottom);
    assert(success != 0);
}

void Canvas::drawGlyph(int top, int left, unsigned width, unsigned height,
                  AIndex ind, AValue glyph)
{
    unsigned textLen = 1;
    char text[1];
    string str = attributeToString(ind, glyph);
    text[0] = str[0];

	RECT rect;
	rect.top = top;
	rect.left = left;
    rect.bottom = top + height;
    rect.right = left + width;

    UINT format = DT_CENTER | DT_EXTERNALLEADING
                    | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER;
    BOOL success = ::DrawText((HDC)*this, text, textLen, &rect, format);
    assert(success != 0);
}

RECT Canvas::drawRectangle(int top, int left, unsigned width, unsigned height) {
	int right = left + width;
	int bottom = top + height;
	BOOL success = ::Rectangle((HDC)*this, left, top, right, bottom);
	assert(success != 0);
	
	RECT result;
	result.left = left;
	result.right = right;
	result.top = top;
	result.bottom = bottom;
	
	return result;
}

void Canvas::drawText(int x, int y, unsigned width, unsigned height, 
    char const *text)
{
    int textLen = strlen(text);
     
	RECT rect;
	rect.top = y;
	rect.left = x;
    rect.bottom = rect.top + height;
    rect.right = rect.left + width;

    UINT format = DT_CENTER | DT_EXTERNALLEADING | DT_NOPREFIX 
                | DT_SINGLELINE | DT_VCENTER;
    BOOL success = ::DrawText((HDC)*this, text, textLen, &rect, format);
    assert(success != 0);
}

void Canvas::drawText(int x, int y, unsigned width, unsigned height, 
    string text)
{
    unsigned length = text.size();
    char *copyText = new char[length + 1];
    strcpy(copyText, text.c_str());
    drawText(x, y, width, height, copyText);
    delete[] copyText;
}

RECT Canvas::drawTile(int top, int left, unsigned width, ACount numGlyphs, 
    const AValue glyphs[])
{
    assert(numGlyphs <= 4);
    
	RECT result;
    unsigned height = width;
    unsigned ellipseWidth = width/5;
    unsigned ellipseHeight = ellipseWidth;
    {
        int bottom = top + height;
        int right = left + width;
        BOOL success = ::RoundRect((HDC)*this, left, top, right, bottom,
                             ellipseWidth, ellipseHeight);
        assert(success != 0);
        
	    result.left = left;
	    result.right = right;
	    result.top = top;
	    result.bottom = bottom;
    }
 
    top += ellipseHeight/2;
    left += ellipseWidth/2;
    width -= ellipseWidth;
    height -= ellipseHeight;
    if (numGlyphs == 2) {
        width /= 2;
    } else if (numGlyphs == 3 || numGlyphs == 4) {
        width /= 2;
        height /= 2;
    }
   
    for (unsigned ind = 0; ind < numGlyphs; ind++) {
        int glyphTop, glyphLeft;
        if (numGlyphs == 1) {
            glyphTop = top;
            glyphLeft = left;
        } else if (numGlyphs == 2) {
            glyphTop = top;
            glyphLeft = left + ind*width;
        } else {
            assert(numGlyphs == 3 || numGlyphs == 4);
            glyphTop = top + (ind/2)*height;
            glyphLeft = left + (ind%2)*width;
        }

        AValue glyph = glyphs[ind];
		drawGlyph(glyphTop, glyphLeft, width, height, ind, glyph);
    }
	
	return result;
}

void Canvas::useColors(COLORREF brushBkColor, COLORREF penTextColor) {
    DevContext::useColors(brushBkColor, penTextColor);
}

#endif
