// File:    canvas.cpp
// Purpose: Canvas class
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

#include "project.hpp"

#ifdef _WINDOWS
#include "gui/canvas.hpp"
#include "gui/poly.hpp"
#include "gui/rect.hpp"
#include "string.hpp"

#include <math.h>
#ifndef M_PI
#define M_PI 3.141592653589793
#endif

std::vector<Poly> Canvas::msGlyphs;

Canvas::Canvas(
    HDC context,
    HWND window,
    bool releaseMe,
    unsigned width,
    unsigned height)
:
    Graphics(context, window, releaseMe, true, width, height)
{}

Rect Canvas::DrawCell(
    int top,
    int left,
    unsigned width,
    ColorType cellColor,
    ColorType gridColor)
{
    UseColors(cellColor, gridColor);
    int height = width;
    Rect result = DrawRectangle(top, left, width, height);

	return result;
}

void Canvas::DrawGlyph(
    Rect bounds, 
    AIndexType ind,
    AValueType glyph,
    ColorType backgroundColor,
    ColorType glyphColor)
{
    if (msGlyphs.size() == 0) {
        InitGlyphs();
    }
    
    if (ind == 0) {
        UseColors(glyphColor, glyphColor);
        
        ASSERT(glyph < msGlyphs.size());
        Poly polygon = msGlyphs[glyph];
        DrawPolygon(polygon, bounds);
        
    } else { 
        UseColors(backgroundColor, glyphColor);

        String str = attributeToString(ind - 1, glyph);
        DrawText(bounds, str);
    }
}

void Canvas::DrawTarget(Rect bounds) {
	DrawLine(bounds.LeftX(), bounds.TopY(), bounds.RightX(), bounds.BottomY());
	DrawLine(bounds.LeftX(), bounds.BottomY(), bounds.RightX(), bounds.TopY());
}

Rect Canvas::DrawTile(
    int top, 
    int left, 
    unsigned edge,
    ACountType numGlyphs, 
    const AValueType glyphs[],
    ColorType tileColor,
    ColorType glyphColor)
{
    assert(numGlyphs <= 4);
    
    UseColors(tileColor, tileColor);
    unsigned circleDiameter = edge/5;
	Rect result = DrawRoundedSquare(top, left, edge, circleDiameter);
 
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

        Rect glyphBounds(glyphTop, glyphLeft, width, height);
        AValueType glyph = glyphs[ind];
		DrawGlyph(glyphBounds, ind, glyph, tileColor, glyphColor);
    }
	
	return result;
}

void Canvas::InitGlyphs(void) {
    ASSERT(msGlyphs.size() == 0);
    {
        Poly roundel;
        for (unsigned i = 0; i < 20; i++) {
            double phi = M_PI/10 * (double)i;
            double x = 0.5 + 0.45*::cos(phi);
            double y = 0.5 + 0.45*::sin(phi);
            roundel.Add(x, y);
        }
        msGlyphs.push_back(roundel);
    }
    {
        Poly delf;
        delf.Add(0.15, 0.15);
        delf.Add(0.85, 0.15);
        delf.Add(0.85, 0.85);
        delf.Add(0.15, 0.85);
        msGlyphs.push_back(delf);
    }        
    {
        Poly triangle;
        triangle.Add(0.0, 0.1);
        triangle.Add(0.5, 0.9);
        triangle.Add(1.0, 0.1);
        msGlyphs.push_back(triangle);
    }        
    {
        Poly starCross;
        starCross.Add(0.0, 0.5);
        starCross.Add(0.4, 0.6);
        starCross.Add(0.5, 1.0);
        starCross.Add(0.6, 0.6);
        starCross.Add(1.0, 0.5);
        starCross.Add(0.6, 0.4);
        starCross.Add(0.5, 0.0);
        starCross.Add(0.4, 0.4);
        msGlyphs.push_back(starCross);
    }        
    {
        Poly mulletOfSeven;
        for (unsigned i = 0; i < 14; i++) {
            double phi = M_PI/7 * (float)i;
            double x, y;
            if (i % 2) {
                x = 0.5 + 0.2*::sin(phi);
                y = 0.5 + 0.2*::cos(phi);
            } else {
                x = 0.5 * (1 + ::sin(phi));
                y = 0.5 * (1 + ::cos(phi));
            }
            mulletOfSeven.Add(x, y);
        }
        msGlyphs.push_back(mulletOfSeven);
    }        
    {
        Poly lozenge;
        lozenge.Add(0.5, 0.0);
        lozenge.Add(1.0, 0.5);
        lozenge.Add(0.5, 1.0);
        lozenge.Add(0.0, 0.5);
        msGlyphs.push_back(lozenge);
    }        
    {
        Poly heart;
        heart.Add(0.5, 0.0);
        heart.Add(0.1, 0.5);
        heart.Add(0.05, 0.7);
        heart.Add(0.1, 0.9);
        heart.Add(0.2, 1.0);
        heart.Add(0.4, 1.0);
        heart.Add(0.5, 0.85);
        heart.Add(0.6, 1.0);
        heart.Add(0.8, 1.0);
        heart.Add(0.9, 0.9);
        heart.Add(0.95, 0.7);
        heart.Add(0.9, 0.5);
        msGlyphs.push_back(heart);
    }        
    {
        Poly trefoil;
        trefoil.Add(0.5, 1.0);
        trefoil.Add(0.36, 0.94);
        trefoil.Add(0.3, 0.8);
        trefoil.Add(0.36, 0.66);
        trefoil.Add(0.34, 0.64);
        trefoil.Add(0.2, 0.7);
        trefoil.Add(0.06, 0.64);
        trefoil.Add(0.0, 0.5);
        trefoil.Add(0.06, 0.36);
        trefoil.Add(0.2, 0.3);
        trefoil.Add(0.34, 0.36);
        trefoil.Add(0.44, 0.34);
        trefoil.Add(0.4, 0.1);            
        trefoil.Add(0.3, 0.0);

        trefoil.Add(0.7, 0.0);
        trefoil.Add(0.6, 0.1);            
        trefoil.Add(0.56, 0.34);
        trefoil.Add(0.66, 0.36);
        trefoil.Add(0.8, 0.3);
        trefoil.Add(0.94, 0.36);
        trefoil.Add(1.0, 0.5);
        trefoil.Add(0.94, 0.64);
        trefoil.Add(0.8, 0.7);
        trefoil.Add(0.66, 0.64);
        trefoil.Add(0.64, 0.66);
        trefoil.Add(0.7, 0.8);
        trefoil.Add(0.64, 0.94);
        msGlyphs.push_back(trefoil);
    }        
    {
        Poly spade;
        spade.Add(0.5, 1.0);
        spade.Add(0.1, 0.6);
        spade.Add(0.05, 0.44);
        spade.Add(0.1, 0.28);
        spade.Add(0.2, 0.2);
        spade.Add(0.4, 0.2);
        spade.Add(0.49, 0.3);
        spade.Add(0.3, 0.0);
         
        spade.Add(0.7, 0.0);
        spade.Add(0.51, 0.3);
        spade.Add(0.6, 0.2);
        spade.Add(0.8, 0.2);
        spade.Add(0.9, 0.28);
        spade.Add(0.95, 0.44);
        spade.Add(0.9, 0.6);
        msGlyphs.push_back(spade);
    }
}

#endif
