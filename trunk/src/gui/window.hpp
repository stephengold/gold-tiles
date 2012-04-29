#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:     window.hpp
// Location: src/gui
// Purpose:  declare Window class
// Author:   Stephen Gold sgold@sonic.net
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

/*
A Window object represents an overlapped GUI window with the following feaures:
+ a menubar
+ a paintable/resizeable client area
+ access to the mouse pointer (cursor)
+ message boxes, and
+ multiple fibers (optional).

The Window class is implemented by extending the BaseWindow class.
*/

#include "gui/basewindow.hpp" // ISA BaseWindow
#include "gui/rect.hpp"       // HASA PixelCntType


class Window: public BaseWindow {
public:
    // public lifecycle
    Window(void);
    // ~Window(void);  compiler-generated destructor is OK

    // public operators
    operator Rect(void) const;

    // misc public methods
    Point        Brc(void) const;
    PixelCntType ClientAreaHeight(void) const;
    PixelCntType ClientAreaWidth(void) const;
    Win::LRESULT HandleMessage(MessageType, Win::WPARAM, Win::LPARAM); 
    void         InfoBox(TextType message, TextType title);
    int          MessageDispatchLoop(void);
    Win::HDC     PaintDevice(void) const;
    bool         RetryBox(TextType message, TextType title);
    void         Show(int showHow);
    void         Yields(void);

protected:
    // protected lifecycle
    void Initialize(Win::CREATESTRUCT const&);

    // misc protected methods
    void* AddFiber(void (CALLBACK& routine)(void*));
    void  BeginPaint(void);
    void  CaptureMouse(void);
    void  Close(void);
    void  Create(String const& className, Rect const&, 
              BaseWindow* parent, Win::HINSTANCE);
    void  EndPaint(void);
    void  ErrorBox(TextType message, TextType title);
    void  ForceRepaint(void);
    bool  IsMouseCaptured(void) const;
    int   QuestionBox(TextType message, TextType title);
    void  ReleaseMouse(void);
    void  SelfDestruct(void);
    void  SetAcceleratorTable(TextType resourceName);
    void  SetClientArea(PixelCntType width, PixelCntType height);
    void  SetCursorBusy(void);
    void  SetCursorDrag(void);
    void  SetCursorSelect(void);
    void  SetIcons(TextType resourceName);
    void  SetTimer(unsigned msecs, unsigned id);
    void  UpdateMenuBar(void);
    void  UseFibers(void);
    int   WarnBox(TextType message, TextType title);
    void  WarpCursor(Point const&);

private:
    Win::HACCEL      mAcceleratorTable;
    PixelCntType     mClientAreaHeight;
    PixelCntType     mClientAreaWidth;
    void*            mMainFiber;
    Win::HDC         mPaintDevice;
    Win::PAINTSTRUCT mPaintStruct;

    // private lifecycle
    Window(Window const&); // not copyable

    // private operators
    Window& operator=(Window const&); // not assignable

    // misc private methods
    bool GetAMessage(Win::MSG&, int& exitCode);
    void SetCursor(Win::LPSTR);
    void TranslateAndDispatch(Win::MSG&);

    // private inquiry methods
    bool HasAMessage(void) const;
};
#endif // !defined(WINDOW_HPP_INCLUDED)
