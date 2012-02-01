#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

// File:    graphics.hpp
// Purpose: Graphics class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

/*

*/

#include "project.hpp"

#ifdef _WINDOWS
#include <vector>
#include <windows.h>

class Rect {
    RECT _bounds;
    public:
        Rect(void) { assert(false); };
        //Rect(Rect const &);
        Rect(int topY, int leftX, unsigned width, unsigned height);
        //Rect &operator=(Rect const &);
        //~Rect(void);

        Rect centerSquare(void) const;
        bool contains(POINT const &) const;

        int getBottomY(void) const;
        unsigned getHeight(void) const;
        int getLeftX(void) const;
        int getRightX(void) const;
        int getTopY(void) const;
        unsigned getWidth(void) const;

        operator RECT(void) const;
};

class Graphics {
    HGDIOBJ _bitmapSave, _brushSave, _penSave;
    COLORREF _brushBkColor, _penTextColor;
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
        
        void drawPolygon(Poly const &, Rect const &);
        Rect drawRectangle(int top, int left, unsigned width, unsigned height);
        Rect drawRoundedSquare(int top, int left, unsigned edge, unsigned diam);
        void drawText(Rect const &, char const *);
        void drawText(Rect const &, string);
        
        void getColors(COLORREF &brushBk, COLORREF &penText) const;
        unsigned getTextHeight(void) const;
        unsigned getTextWidth(char const *) const;
        unsigned getTextWidth(string) const;

        void useColors(COLORREF brushBk, COLORREF penText);
};

#endif
#endif
