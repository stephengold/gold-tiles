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
#include <vector>
#include <windows.h>
#include "gui/graphics.hpp"
#include "tile.hpp"

class Canvas: public Graphics {
public:
	// lifecycle
    Canvas(void) { ASSERT(false); };
    Canvas(Canvas const &) { ASSERT(false); };
    Canvas(HDC, HWND, bool releaseMe, unsigned width, unsigned height);
    // ~Canvas(void);  compiler-generated destructor is OK
        
	// operators
    Canvas &operator=(Canvas const &) { ASSERT(false); };

	// misc
	void DrawCell(int top, int left, unsigned width,
               ColorType cellColor, ColorType gridColor);
    Rect DrawTile(int top, int left, unsigned width, 
               ACountType numGlyphs, const AValueType glyphs[],
               ColorType tile, ColorType glyph);

private:
    static std::vector<Poly> msGlyphs;
    
	// misc
    void DrawGlyph(Rect bounds, AIndexType, AValueType, 
                   ColorType background, ColorType glyph);
    void InitGlyphs(void);            
};

#endif
#endif
