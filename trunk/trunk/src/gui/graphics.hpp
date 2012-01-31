#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

// File:    graphics.hpp
// Purpose: Graphics class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

class Rectangle {
    RECT _bounds;
    public:
        Rectangle(void) { assert(false); };
        //Rectangle(Rectangle const &);
        Rectangle(int topY, int leftX, unsigned width, unsigned height);
        //Rectangle &operator=(Rectangle const &);
        //~Rectangle(void);

        unsigned getHeight(void) const;
        unsigned getWidth(void) const;
        operator RECT(void) const;
};

class Graphics {
    HGDIOBJ _bitmapSave, _brushSave, _penSave;
    HDC _device, _draw;
    unsigned _height, _width;
    bool _releaseMe;
    HWND _window;

    public:
        Graphics(void) { assert(false); };
        Graphics(Graphics const &) { assert(false); };
        Graphics(HDC, HWND, bool releaseMe, bool buffer, 
                   unsigned width, unsigned height);
        Graphics &operator=(Graphics const &) { assert(false); };
        ~Graphics(void);

        void close(void);
        
        RECT drawRectangle(int top, int left, unsigned width, unsigned height);
        RECT drawRoundedSquare(int top, int left, unsigned edge, unsigned diam);
        void drawText(int top, int left, unsigned width, unsigned height, 
                  char const *);
        void drawText(int top, int left, unsigned width, unsigned height, 
                  string);

        unsigned getTextHeight(void) const;
        unsigned getTextWidth(char const *) const;
        unsigned getTextWidth(string) const;

        void useColors(COLORREF brushBk, COLORREF penText);
};

#endif
#endif
