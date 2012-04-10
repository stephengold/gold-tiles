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

std::vector<Poly> Canvas::msShapes;  // polygons for markings
Poly Canvas::msTargetArrow;          // polygon for target arrow


// lifecycle

Canvas::Canvas(Window &rWindow):
    Graphics(rWindow.PaintDevice(), rWindow, false, true)
{}


// misc methods

void Canvas::DrawBlankTile(
    Point const &rCenter,
    PixelCntType width,
	PixelCntType height,
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
            PixelCntType circle_diameter = width/TILE_POINTEDNESS;
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
    PixelCntType width,
	PixelCntType height,
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
    PixelCntType width,
	PixelCntType height,
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

void Canvas::DrawMarking(
    Rect const &rBounds, 
    AttrModeType displayMode,
    AttrType marking,
    ColorType backgroundColor,
    ColorType markingColor)
{
	if (msShapes.size() == 0) {
        InitShapes();
    }
    
	switch (displayMode) {
	    case ATTR_MODE_SHAPE: {
            UseColors(markingColor, markingColor);
        
            ASSERT(marking < msShapes.size());
            Poly const polygon = msShapes[marking];
		    Rect const square = rBounds.CenterSquare();
            DrawPolygon(polygon, square);
			break;
		}
        
		case ATTR_MODE_ABC:
		case ATTR_MODE_RST:
		case ATTR_MODE_123: {
            UseColors(backgroundColor, markingColor);

            String const ch = Tile::AttributeToString(displayMode, marking);
            DrawText(rBounds, ch);
			break;
		}

		default:
			FAIL();
    }
}

void Canvas::DrawTarget(Rect const &rBounds) {
	if (msShapes.size() == 0) {
        InitShapes();
    }

    DrawPolygon(msTargetArrow, rBounds);
}

Rect Canvas::DrawTile(
	Markings const &rMarkings,
	ColorType tileColor,
	Point const &rCenter,
	PixelCntType width,
    PixelCntType height,
	bool borderFlag,
	bool oddFlag)
{
	ASSERT(::is_even(width));

	if (borderFlag) {
		ColorType border_color = COLOR_WHITE;
		if (tileColor == COLOR_DULL_GOLD) {
			border_color = COLOR_GOLD;
		}
        UseColors(border_color, border_color);
	} else {
        UseColors(tileColor, COLOR_DARK_GRAY);
	}

	Rect interior(0,0,0,0);
	switch (Cell::Grid()) {
	    case GRID_4WAY:
	    case GRID_8WAY: {
			ASSERT(width == height);
            PixelCntType const circle_diameter = width/TILE_POINTEDNESS;
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

	if (borderFlag) {
        UseColors(tileColor, tileColor);
		DrawRectangle(interior);
	}

	unsigned const marking_cnt = rMarkings.MarkingCnt();

    PixelCntType marking_width = interior.Width();
    PixelCntType marking_height = interior.Height();    
    if (marking_cnt == 2) {
        marking_width /= 2;
    } else if (marking_cnt == 3 || marking_cnt == 4) {
        marking_width /= 2;
        marking_height /= 2;
    } else {
        ASSERT(marking_cnt == 1);
    }

	ColorType const marking_color = rMarkings.MarkingColor();
    for (AttrIndexType ind = 0; ind < marking_cnt; ind++) {
        LogicalXType marking_left = interior.LeftX();
        LogicalYType marking_top = interior.TopY();
        if (marking_cnt == 2) {
            marking_left += ind*marking_width;
        } else if (marking_cnt == 3 || marking_cnt == 4) {
            marking_left += (ind%2)*marking_width;
            marking_top += (ind/2)*marking_height;
        } else {
            ASSERT(marking_cnt == 1);
        }

        Rect const marking_bounds(marking_top, marking_left, marking_width, marking_height);
		AttrModeType const mode = rMarkings.Mode(ind);
		AttrType const marking = rMarkings.Marking(ind);
		DrawMarking(marking_bounds, mode, marking, tileColor, marking_color); 
    }
    
    return interior;
}

/* static */ void Canvas::InitShapes(void) {
    ASSERT(msShapes.size() == 0);
    
	// arrow for indicating the target cell
	msTargetArrow.Add(0.6, 0.4);
	msTargetArrow.Add(0.5, 0.9);
	msTargetArrow.Add(0.45, 0.65);
	msTargetArrow.Add(0.2, 0.95);
	msTargetArrow.Add(0.05, 0.8);
	msTargetArrow.Add(0.35, 0.55);
	msTargetArrow.Add(0.1, 0.5);
	msTargetArrow.Add(0.6, 0.4);

	// tile markings
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
    PixelCntType width,
	PixelCntType height,
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
