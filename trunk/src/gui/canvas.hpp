#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

// File:    canvas.hpp
// Purpose: DevContext and Canvas classes for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "tile.hpp"

enum Color {
    BLACK_COLOR       = PALETTERGB(0, 0, 0),
    DARK_GRAY_COLOR   = PALETTERGB(64, 64, 64),
    LIGHT_GRAY_COLOR  = PALETTERGB(192, 192, 192),
    WHITE_COLOR       = PALETTERGB(255, 255, 255),

    RED_COLOR         = PALETTERGB(255, 0, 0),
    PINK_COLOR        = PALETTERGB(255, 128, 128),

    BROWN_COLOR       = PALETTERGB(100, 100, 0),
    YELLOW_COLOR      = PALETTERGB(255, 255, 0),
    
    DARK_GREEN_COLOR  = PALETTERGB(0, 100, 0),
    GREEN_COLOR       = PALETTERGB(0, 255, 0),
    LIGHT_GREEN_COLOR = PALETTERGB(64, 255, 64),

    DARK_BLUE_COLOR  = PALETTERGB(0, 0, 100),
    LIGHT_BLUE_COLOR = PALETTERGB(64, 64, 255),
    
    PURPLE_COLOR = PALETTERGB(128, 128, 0)
};

class DevContext {
    HDC _contextHandle;
    bool _releaseMe;
    HWND _windowHandle;
    HGDIOBJ _brushSave;
    HGDIOBJ _penSave;

    protected:
        DevContext(void) { assert(false); };
        DevContext(DevContext const &) { assert(false); };
        DevContext &operator=(DevContext const &) { assert(false); };
        ~DevContext(void);

        DevContext(HDC, HWND, bool releaseMe);

        operator HDC(void) const;
        void useColors(COLORREF brushBk, COLORREF penText);
};

class Canvas: public DevContext {
    // TODO: implement double buffering
    void drawGlyph(int top, int left, unsigned width, unsigned height,
              AIndex, AValue);
    public:
        Canvas(void) { assert(false); };
        Canvas(Canvas const &) { assert(false); };
        Canvas &operator=(Canvas const &) { assert(false); };
        // ~Canvas(void);  compiler-generated destructor is OK
        
        Canvas(HDC, HWND, bool releaseMe);
        
        void drawBag(int top, int left, unsigned width,
                  COLORREF bagColor, COLORREF textColor, char const *);
        void drawCell(int top, int left, unsigned width,
                  COLORREF cellColor, COLORREF gridColor);
        RECT drawRectangle(int top, int left, unsigned width, unsigned height);
        void drawText(int top, int left, unsigned width, unsigned height, 
                  char const *);
        void drawText(int top, int left, unsigned width, unsigned height, 
                  string);
        RECT drawTile(int top, int left, unsigned width, 
                  ACount numGlyphs, const AValue glyphs[]);
        void useColors(COLORREF brushBk, COLORREF penText);
};

#endif
#endif
