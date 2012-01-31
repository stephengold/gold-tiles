// File:    canvas.cpp
// Purpose: Canvas class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/canvas.hpp"

Canvas::Canvas(
    HDC context,
    HWND window,
    bool releaseMe,
    unsigned width,
    unsigned height)
:
    Graphics(context, window, releaseMe, true, width, height)
{}

void Canvas::drawCell(
    int top,
    int left,
    unsigned width,
    COLORREF cellColor,
    COLORREF gridColor)
{
    useColors(cellColor, gridColor);
    int height = width;
    drawRectangle(top, left, width, height);
}

void Canvas::drawGlyph(
    int top,
    int left,
    unsigned width,
    unsigned height,
    AIndex ind,
    AValue glyph)
{
    string str = attributeToString(ind, glyph);
    drawText(top, left, width, height, str);
}

RECT Canvas::drawTile(
    int top, 
    int left, 
    unsigned edge,
    ACount numGlyphs, 
    const AValue glyphs[])
{
    assert(numGlyphs <= 4);
    
    unsigned circleDiameter = edge/5;
	RECT result = drawRoundedSquare(top, left, edge, circleDiameter);
 
    top += circleDiameter/2;
    left += circleDiameter/2;
    int width = edge - circleDiameter;
    int height = edge - circleDiameter;
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

#endif
