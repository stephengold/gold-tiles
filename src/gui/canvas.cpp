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

#ifdef _WINDOWS
#include <math.h>
#include "cell.hpp"
#include "gui/canvas.hpp"
#include "gui/poly.hpp"
#include "gui/rect.hpp"
#include "gui/window.hpp"
#include "string.hpp"

// static data

std::vector<Poly> Canvas::msShapes; // shapes for glyphs

// lifecycle

Canvas::Canvas(Window &rWindow):
    Graphics(rWindow.PaintDevice(), rWindow, false, true)
{}


// misc methods

void Canvas::DrawBlankTile(
    Point const &rCenter,
    PCntType width,
	PCntType height,
    ColorType tileColor,
	bool oddFlag)
{
	ASSERT(::is_even(width));

	ColorType border_color = COLOR_DARK_GRAY;
    UseColors(tileColor, border_color);

	switch (Cell::Grid()) {
	    case GRID_4WAY:
	    case GRID_8WAY: {
			ASSERT(width == height);
            PCntType circle_diameter = width/TILE_POINTEDNESS;
	        DrawRoundedSquare(rCenter, width, circle_diameter);
			break;
		}
	    case GRID_HEX:
	    case GRID_TRIANGLE: 
			DrawGridShape(rCenter, width, height, oddFlag);
			break;
		default:
			FAIL();
	}
}

Rect Canvas::DrawCell(
    Point const &rCenter,
    PCntType width,
	PCntType height,
    ColorType cellColor,
    ColorType gridColor,
	bool oddFlag)
{
	ASSERT(::is_even(width));

	UseColors(cellColor, gridColor);
	DrawGridShape(rCenter, width, height, oddFlag);
	Rect const interior = InteriorGridShape(rCenter, width, height, oddFlag);

	return interior;
}

void Canvas::DrawGridShape(
    Point const &rCenter,
    PCntType width,
	PCntType height,
	bool oddFlag)
{
    Point ulc = rCenter;
	ulc.Offset(-long(width/2), -long(height/2));
	Rect const rectangle = Rect(ulc, width, height);

	Rect interior(0,0,0,0);
	switch (Cell::Grid()) {
	    case GRID_4WAY:
	    case GRID_8WAY: // square
            ASSERT(height == width);
            DrawRectangle(rectangle);
			break;
	    case GRID_HEX:
			ASSERT(!oddFlag);
            ASSERT(height < width);
			DrawHexagon(rectangle);
			break;
	    case GRID_TRIANGLE: 
			ASSERT(height < width);
			DrawEquilateral(rectangle, oddFlag);
			break;
		default:
			FAIL();
	}
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
        Poly const polygon = msShapes[glyph];
		Rect const square = rBounds.CenterSquare();
        DrawPolygon(polygon, square);
        
    } else { 
        UseColors(backgroundColor, glyphColor);

        String const str = ::attribute_to_string(ind - 1, glyph);
        DrawText(rBounds, str);
    }
}

void Canvas::DrawTarget(Rect const &rBounds) {
    // for now, just draw an X
	FractionPair pair_ulc(0.1, 0.9);
    Point const ulc = rBounds.Interpolate(pair_ulc);
	FractionPair pair_brc(0.9, 0.1);
    Point const brc = rBounds.Interpolate(pair_brc);
	DrawLine(ulc, brc);

	FractionPair pair_urc(0.9, 0.9);
    Point const urc = rBounds.Interpolate(pair_urc);
	FractionPair pair_blc(0.1, 0.1);
    Point const blc = rBounds.Interpolate(pair_blc);
	DrawLine(urc, blc);
}

Rect Canvas::DrawTile(
    Point const &rCenter,
	PCntType width,
    PCntType height,
    ACountType numGlyphs, 
    const AValueType glyphs[],
    ColorType tileColor,
    ColorType glyphColor,
	bool oddFlag)
{
    ASSERT(numGlyphs > 0);
    ASSERT(numGlyphs <= 4);
	ASSERT(::is_even(width));

	ColorType border_color = COLOR_DARK_GRAY;
    UseColors(tileColor, border_color);

	Rect interior(0,0,0,0);
	switch (Cell::Grid()) {
	    case GRID_4WAY:
	    case GRID_8WAY: {
			ASSERT(width == height);
            PCntType const circle_diameter = width/TILE_POINTEDNESS;
	        DrawRoundedSquare(rCenter, width, circle_diameter);
	        interior = InteriorRoundedSquare(rCenter, width, circle_diameter);
			break;
		}
	    case GRID_HEX:
	    case GRID_TRIANGLE: 
			DrawGridShape(rCenter, width, height, oddFlag);
			interior = InteriorGridShape(rCenter, width, height, oddFlag);
			break;
		default:
			FAIL();
	}

    PCntType glyph_width = interior.Width();
    PCntType glyph_height = interior.Height();    
    if (numGlyphs == 2) {
        glyph_width /= 2;
    } else if (numGlyphs == 3 || numGlyphs == 4) {
        glyph_width /= 2;
        glyph_height /= 2;
    } else {
        ASSERT(numGlyphs == 1);
    }
   
    for (AIndexType ind = 0; ind < numGlyphs; ind++) {
        LogicalXType glyph_left = interior.LeftX();
        LogicalYType glyph_top = interior.TopY();
        if (numGlyphs == 2) {
            glyph_left += ind*glyph_width;
        } else if (numGlyphs == 3 || numGlyphs == 4) {
            glyph_left += (ind%2)*glyph_width;
            glyph_top += (ind/2)*glyph_height;
        } else {
            ASSERT(numGlyphs == 1);
        }

        Rect const glyphBounds(glyph_top, glyph_left, glyph_width, glyph_height);
        AValueType const glyph = glyphs[ind];
		DrawGlyph(glyphBounds, ind, glyph, tileColor, glyphColor);
    }
    
    return interior;
}

/* static */ void Canvas::InitShapes(void) {
    ASSERT(msShapes.size() == 0);
    
    {
        Poly roundel;
        for (unsigned i = 0; i < 20; i++) {
            double const phi = M_PI/10 * (double)i;
            double const x = 0.5 + 0.44*::cos(phi);
            double const y = 0.5 + 0.44*::sin(phi);
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
        triangle.Add(0.05, 0.2);
        triangle.Add(0.5, 0.95);
        triangle.Add(0.95, 0.2);
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
            double const phi = M_PI/7 * (float)i;
            double x, y;
            if (::is_odd(i)) {
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
        heart.Add(0.1, 0.45);
        heart.Add(0.05, 0.65);
        heart.Add(0.1, 0.8);
        heart.Add(0.2, 0.9);
        heart.Add(0.4, 0.9);
        heart.Add(0.5, 0.75);
        heart.Add(0.6, 0.9);
        heart.Add(0.8, 0.9);
        heart.Add(0.9, 0.8);
        heart.Add(0.95, 0.65);
        heart.Add(0.9, 0.45);
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

    ASSERT(msShapes.size() >= Tile::VALUE_CNT_MAX);
}

/* static */ Rect Canvas::InteriorGridShape(
    Point const &rCenter,
    PCntType width,
	PCntType height,
	bool oddFlag)
{
    Point ulc = rCenter;
	ulc.Offset(-long(width/2), -long(height/2));
	Rect const rectangle = Rect(ulc, width, height);

	Rect interior(0,0,0,0);
	switch (Cell::Grid()) {
	    case GRID_4WAY:
	    case GRID_8WAY: // square
            ASSERT(height == width);
            interior = rectangle;
			break;
	    case GRID_HEX:
            ASSERT(!oddFlag);
            ASSERT(height < width);
			interior = InteriorHexagon(rectangle);
			break;
	    case GRID_TRIANGLE: 
			ASSERT(height < width);
			interior = InteriorEquilateral(rectangle, oddFlag);
			break;
		default:
			FAIL();
	}

	return interior;
}
#endif // defined(_WINDOWS)
