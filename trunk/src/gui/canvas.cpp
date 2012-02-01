// File:    canvas.cpp
// Purpose: Canvas class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/canvas.hpp"
#include "gui/poly.hpp"
#include <math.h>

vector<Poly> Canvas::_glyphs;

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
    int topY,
    int leftX,
    unsigned width,
    unsigned height,
    AIndex ind,
    AValue glyph)
{
    if (_glyphs.size() == 0) {
        initGlyphs();
    }
    
    if (ind == 0) {
        COLORREF bg, fg;
        getColors(bg, fg);
        useColors(fg, fg);
        
        ASSERT(glyph < _glyphs.size());
        Poly polygon = _glyphs[glyph];
        Rect bounds(topY, leftX, width, height);
        drawPolygon(polygon, bounds);
        useColors(bg, fg);
    } else { 
        string str = attributeToString(ind - 1, glyph);
        drawText(topY, leftX, width, height, str);
    }
}

RECT Canvas::drawTile(
    int top, 
    int left, 
    unsigned edge,
    ACount numGlyphs, 
    const AValue glyphs[])
{
    assert(numGlyphs <= 4);
    
    COLORREF bg, fg;
    getColors(bg, fg);
    useColors(bg, bg);
    unsigned circleDiameter = edge/5;
	RECT result = drawRoundedSquare(top, left, edge, circleDiameter);
    useColors(bg, fg);
 
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

void Canvas::initGlyphs(void) {
    ASSERT(_glyphs.size() == 0);
    {
        Poly roundel;
        for (unsigned i = 0; i < 20; i++) {
            float phi = M_PI/10 * (float)i;
            float x = 0.5 + 0.45*::cos(phi);
            float y = 0.5 + 0.45*::sin(phi);
            roundel.add(x, y);
        }
        _glyphs.push_back(roundel);
    }
    {
        Poly delf;
        delf.add(0.15, 0.15);
        delf.add(0.85, 0.15);
        delf.add(0.85, 0.85);
        delf.add(0.15, 0.85);
        _glyphs.push_back(delf);
    }        
    {
        Poly triangle;
        triangle.add(0.0, 0.1);
        triangle.add(0.5, 0.9);
        triangle.add(1.0, 0.1);
        _glyphs.push_back(triangle);
    }        
    {
        Poly starCross;
        starCross.add(0.0, 0.5);
        starCross.add(0.4, 0.6);
        starCross.add(0.5, 1.0);
        starCross.add(0.6, 0.6);
        starCross.add(1.0, 0.5);
        starCross.add(0.6, 0.4);
        starCross.add(0.5, 0.0);
        starCross.add(0.4, 0.4);
        _glyphs.push_back(starCross);
    }        
    {
        Poly mulletOfSeven;
        for (unsigned i = 0; i < 14; i++) {
            float phi = M_PI/7 * (float)i;
            float x, y;
            if (i % 2) {
                x = 0.5 + 0.2*::sin(phi);
                y = 0.5 + 0.2*::cos(phi);
            } else {
                x = 0.5 * (1 + ::sin(phi));
                y = 0.5 * (1 + ::cos(phi));
            }
            mulletOfSeven.add(x, y);
        }
        _glyphs.push_back(mulletOfSeven);
    }        
    {
        Poly lozenge;
        lozenge.add(0.5, 0.0);
        lozenge.add(1.0, 0.5);
        lozenge.add(0.5, 1.0);
        lozenge.add(0.0, 0.5);
        _glyphs.push_back(lozenge);
    }        
    {
        Poly heart;
        heart.add(0.5, 0.0);
        heart.add(0.1, 0.5);
        heart.add(0.05, 0.7);
        heart.add(0.1, 0.9);
        heart.add(0.2, 1.0);
        heart.add(0.4, 1.0);
        heart.add(0.5, 0.85);
        heart.add(0.6, 1.0);
        heart.add(0.8, 1.0);
        heart.add(0.9, 0.9);
        heart.add(0.95, 0.7);
        heart.add(0.9, 0.5);
        _glyphs.push_back(heart);
    }        
    {
        Poly trefoil;
        trefoil.add(0.5, 1.0);
        trefoil.add(0.36, 0.94);
        trefoil.add(0.3, 0.8);
        trefoil.add(0.36, 0.66);
        trefoil.add(0.34, 0.64);
        trefoil.add(0.2, 0.7);
        trefoil.add(0.06, 0.64);
        trefoil.add(0.0, 0.5);
        trefoil.add(0.06, 0.36);
        trefoil.add(0.2, 0.3);
        trefoil.add(0.34, 0.36);
        trefoil.add(0.44, 0.34);
        trefoil.add(0.4, 0.1);            
        trefoil.add(0.3, 0.0);

        trefoil.add(0.7, 0.0);
        trefoil.add(0.6, 0.1);            
        trefoil.add(0.56, 0.34);
        trefoil.add(0.66, 0.36);
        trefoil.add(0.8, 0.3);
        trefoil.add(0.94, 0.36);
        trefoil.add(1.0, 0.5);
        trefoil.add(0.94, 0.64);
        trefoil.add(0.8, 0.7);
        trefoil.add(0.66, 0.64);
        trefoil.add(0.64, 0.66);
        trefoil.add(0.7, 0.8);
        trefoil.add(0.64, 0.94);
        _glyphs.push_back(trefoil);
    }        
    {
        Poly spade;
        spade.add(0.5, 1.0);
        spade.add(0.1, 0.6);
        spade.add(0.05, 0.44);
        spade.add(0.1, 0.28);
        spade.add(0.2, 0.2);
        spade.add(0.4, 0.2);
        spade.add(0.49, 0.3);
        spade.add(0.3, 0.0);
         
        spade.add(0.7, 0.0);
        spade.add(0.51, 0.3);
        spade.add(0.6, 0.2);
        spade.add(0.8, 0.2);
        spade.add(0.9, 0.28);
        spade.add(0.95, 0.44);
        spade.add(0.9, 0.6);
        _glyphs.push_back(spade);
    }
}

#endif
