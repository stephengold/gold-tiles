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

std::vector<Poly> Canvas::msShapes;

Canvas::Canvas(
    HDC context,
    HWND window,
    bool releaseMe,
    unsigned width,
    unsigned height)
:
    Graphics(context, window, releaseMe, true, width, height)
{}

Rect Canvas::DrawBlankTile(
    Point const &rWhere,
    PCntType edge,
    ColorType tileColor)
{
    UseColors(tileColor, COLOR_DARK_GRAY);
    
    PCntType circleDiameter = edge/5;
	DrawRoundedSquare(rWhere, edge, circleDiameter);

    Rect result = Rect(rWhere, edge, edge);
	
	return result;
}

Rect Canvas::DrawCell(
    LogicalYType top,
    LogicalXType left,
    PCntType width,
    ColorType cellColor,
    ColorType gridColor)
{
    UseColors(cellColor, gridColor);
    PCntType height = width;
    Rect result = DrawRectangle(top, left, width, height);

	return result;
}

void Canvas::DrawGlyph(
    Rect const &rBounds, 
    AIndexType ind,
    AValueType glyph,
    ColorType backgroundColor,
    ColorType glyphColor)
{
    if (msShapes.size() == 0) {
        InitShapes();
    }
    
    // TODO less arbitrary
    if (ind == 0) {
        UseColors(glyphColor, glyphColor);
        
        ASSERT(glyph < msShapes.size());
        Poly polygon = msShapes[glyph];
        DrawPolygon(polygon, rBounds);
        
    } else { 
        UseColors(backgroundColor, glyphColor);

        String str = attributeToString(ind - 1, glyph);
        DrawText(rBounds, str);
    }
}

void Canvas::DrawTarget(Rect const &rBounds) {
    // for now, just draw an X
    Point ul = rBounds.Interpolate(0.1, 0.9);
    Point lr = rBounds.Interpolate(0.9, 0.1);
	DrawLine(ul, lr);

    Point ur = rBounds.Interpolate(0.9, 0.9);
    Point ll = rBounds.Interpolate(0.1, 0.1);
	DrawLine(ur, ll);
}

Rect Canvas::DrawTile(
    Point const &rWhere,
    PCntType edge,
    ACountType numGlyphs, 
    const AValueType glyphs[],
    ColorType tileColor,
    ColorType glyphColor)
{
    ASSERT(numGlyphs > 0);
    ASSERT(numGlyphs <= 4);
    
    UseColors(tileColor, COLOR_DARK_GRAY);
    PCntType circleDiameter = edge/5;
    Rect result = Rect(rWhere, edge, edge);
	Rect interior = DrawRoundedSquare(rWhere, edge, circleDiameter);
	
    PCntType width = interior.Width();
    PCntType height = interior.Height();    
    if (numGlyphs == 2) {
        width /= 2;
    } else if (numGlyphs == 3 || numGlyphs == 4) {
        width /= 2;
        height /= 2;
    } else {
        ASSERT(numGlyphs == 1);
    }
   
    for (unsigned ind = 0; ind < numGlyphs; ind++) {
        LogicalXType glyphLeft = interior.LeftX();
        LogicalYType glyphTop = interior.TopY();
        if (numGlyphs == 2) {
            glyphLeft += ind*width;
        } else if (numGlyphs == 3 || numGlyphs == 4) {
            glyphLeft += (ind%2)*width;
            glyphTop += (ind/2)*height;
        } else {
            ASSERT(numGlyphs == 1);
        }

        Rect glyphBounds(glyphTop, glyphLeft, width, height);
        AValueType glyph = glyphs[ind];
		DrawGlyph(glyphBounds, ind, glyph, tileColor, glyphColor);
    }
    
    return result;
}

/* static */ void Canvas::InitShapes(void) {
    ASSERT(msShapes.size() == 0);
    
    {
        Poly roundel;
        for (unsigned i = 0; i < 20; i++) {
            double phi = M_PI/10 * (double)i;
            double x = 0.5 + 0.45*::cos(phi);
            double y = 0.5 + 0.45*::sin(phi);
            roundel.Add(x, y);
        }
        msShapes.push_back(roundel);
    }
    {
        Poly delf;
        delf.Add(0.15, 0.15);
        delf.Add(0.85, 0.15);
        delf.Add(0.85, 0.85);
        delf.Add(0.15, 0.85);
        msShapes.push_back(delf);
    }        
    {
        Poly triangle;
        triangle.Add(0.0, 0.1);
        triangle.Add(0.5, 0.9);
        triangle.Add(1.0, 0.1);
        msShapes.push_back(triangle);
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
        msShapes.push_back(starCross);
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
        msShapes.push_back(mulletOfSeven);
    }        
    {
        Poly lozenge;
        lozenge.Add(0.5, 0.0);
        lozenge.Add(1.0, 0.5);
        lozenge.Add(0.5, 1.0);
        lozenge.Add(0.0, 0.5);
        msShapes.push_back(lozenge);
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
        msShapes.push_back(heart);
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
        msShapes.push_back(trefoil);
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
        msShapes.push_back(spade);
    }

    ASSERT(msShapes.size() == 9);
}

#endif
