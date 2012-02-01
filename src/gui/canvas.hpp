#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

// File:    canvas.hpp
// Purpose: Canvas class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/graphics.hpp"
#include "tile.hpp"

class Canvas: public Graphics {
    static vector<Poly> _glyphs;
      
    void initGlyphs(void);
    void drawGlyph(Rect bounds, AIndex, AValue, 
                   COLORREF background, COLORREF glyph);
              
    public:
        Canvas(void) { assert(false); };
        Canvas(Canvas const &) { assert(false); };
        Canvas(HDC, HWND, bool releaseMe, unsigned width, unsigned height);
        Canvas &operator=(Canvas const &) { assert(false); };
        // ~Canvas(void);  compiler-generated destructor is OK
        
        void drawCell(int top, int left, unsigned width,
                  COLORREF cellColor, COLORREF gridColor);
        Rect drawTile(int top, int left, unsigned width, 
                  ACount numGlyphs, const AValue glyphs[],
                  COLORREF tile, COLORREF glyph);
};

#endif
#endif
