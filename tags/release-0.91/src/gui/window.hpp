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

#ifdef _WINDOWS
# include "gui/basewindow.hpp" // ISA BaseWindow
#elif defined(_QT)
# include <QMainWindow>        // ISA QMainWindow
# define IDYES 1
#endif // defined(_QT)
#include "gui/rect.hpp"        // HASA PixelCntType
#include "string.hpp"          // HASA String

class Window: public
#ifdef _WINDOWS
 BaseWindow
#elif defined(_QT)
 QMainWindow
#endif // defined(_QT)
{
public:
    // public lifecycle
    Window(void);
    // ~Window(void);  implicitly defined destructor

    // public operators
    operator Rect(void) const;

    // misc public methods
    Point         Brc(void) const;
    Area          ClientArea(void) const;
    void          DoneWaiting(void);
    void          ErrorBox(TextType message, TextType title);
#ifdef _WINDOWS
    Win::LRESULT  HandleMessage(MessageType, Win::WPARAM, Win::LPARAM);
#endif // defined(_WINDOWS)
    void          InfoBox(TextType message, TextType title);
    int           MessageDispatchLoop(void);
#ifdef _WINDOWS
    Win::HDC      PaintDevice(void) const;
#endif // defined(_QT)
    int           QuestionBox(TextType message, TextType title);
    bool          RetryBox(TextType message, TextType title);
    void          Show(int showHow);
    void          WaitingFor(String const& event);
    String        WaitMessage(void) const;
    void          Yields(void);

// public inquiry methods
    bool IsWaiting(void) const;

protected:
    // misc protected methods
#ifdef _WINDOWS
    void* AddFiber(void (CALLBACK& routine)(void*));
#endif // defined(_WINDOWS)
    void  BeginPaint(void);
    void  CaptureMouse(void);
    void  Close(void);
#ifdef _WINDOWS
    void  Create(String const& className, Rect const&,
              BaseWindow* parent, Win::HINSTANCE);
#endif // defined(_WINDOWS)
    void  EndPaint(void);
    void  ForceRepaint(void);
#ifdef _WINDOWS
    void  Initialize(Win::CREATESTRUCT const&);
#endif // defined(_WINDOWS)
    void  ReleaseMouse(void);
    void  SetAcceleratorTable(TextType resourceName);
    void  SetClientArea(Area const&);
    void  SetCursorBusy(void);
    void  SetCursorDrag(void);
    void  SetCursorSelect(void);
    void  SetTimer(unsigned msecs, unsigned id);
    void  UpdateMenuBar(void);
    void  UseFibers(void);
    int   WarnBox(TextType message, TextType title);
    void  WarpCursor(Point const&);

    // protected inquiry methods
    bool IsMouseCaptured(void) const;

private:
#ifdef _WINDOWS
    Win::HACCEL      mAcceleratorTable;
    Area             mClientArea;
    bool             mDestroyedFlag;
    void*            mMainFiber;
    Win::HDC         mPaintDevice;
    Win::PAINTSTRUCT mPaintStruct;
#endif // defined(_WINDOWS)
    bool             mWaitingFlag;
    String           mWaitingFor;

    // private lifecycle
    Window(Window const&); // not copyable

    // private operators
    Window& operator=(Window const&); // not assignable

    // misc private methods
#ifdef _WINDOWS
    bool GetAMessage(Win::MSG&, int& exitCode);
#endif // defined(_WINDOWS)
    void SelfDestruct(void);
#ifdef _WINDOWS
    void SetCursor(Win::LPSTR);
#endif // defined(_WINDOWS)
    void SetIcons(TextType resourceName);
#ifdef _WINDOWS
    void TranslateAndDispatch(Win::MSG&);
#endif // defined(_WINDOWS)

    // private inquiry methods
    bool HasAMessage(void) const;
};
#endif // !defined(WINDOW_HPP_INCLUDED)
