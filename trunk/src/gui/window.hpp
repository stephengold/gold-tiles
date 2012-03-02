#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:    window.hpp
// Purpose: Window class
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

/*
A Window object represents a generic Microsoft Windows window.

The Window class encapsulates an HWND (window handle) and implements
a static map for translating handles to Window objects. 
*/

#include <map>            // HASA std::map
#include "gui/rect.hpp"   // HASA PCntType

typedef Win::UINT MessageType;

class Window {
public:
	// public lifecycle
	Window(void);
    // ~WindowClass(void);  compiler-generated destructor is OK

	// public operators
	operator Win::HWND(void) const;
    operator Rect(void) const;

	// misc public methods
	PCntType        ClientAreaHeight(void) const;
	PCntType        ClientAreaWidth(void) const;
	Win::LRESULT    HandleMessage(MessageType, Win::WPARAM, Win::LPARAM); 
    static Window * Lookup(Win::HWND);
	Win::HDC        PaintDevice(void) const;
	void            Show(int showHow);

protected:
	static Window *mspNewlyCreatedWindow;

	// protected lifecycle
    void Initialize(Win::CREATESTRUCT const &);

	// misc protected methods
	void           BeginPaint(void);
	void           CaptureMouse(void);
	void           Center(void);
	void           Close(void);
	Win::HINSTANCE CopyModule(Window const &);
	void           Create(String const &rClassName, Rect const &, 
		                  Window *pParent, Win::HINSTANCE);
	static Rect    DesktopBounds(void);
	void           EndPaint(void);
	void           ErrorBox(char const *message, char const *title);
	void           ForceRepaint(void);
	Win::HACCEL    GetAcceleratorTable(char const *resourceName);
	Win::HMENU     GetMenu(char const *resourceName);
	bool           GetMessage(Win::MSG &, int &rExitCode);
	void           InfoBox(char const *message, char const *title);
	bool           IsMouseCaptured(void) const;
	virtual char const * Name(void) const = 0;
	void           ReleaseMouse(void);
	void           SelfDestruct(void);
	void           SetClientArea(PCntType width, PCntType height);
	void           SetCursorBusy(void);
	void           SetCursorDrag(void);
	void           SetCursorSelect(void);
	void           SetHandle(Win::HWND);
	void           SetIcons(char const *resourceName);
	void           SetTimer(unsigned msecs, unsigned id);
	void           TranslateAndDispatch(Win::MSG &, Win::HACCEL const &);
	void           UpdateMenuBar(void);
	int            WarnBox(char const *message, char const *title);
	void           WarpCursor(Point const &);

private:
    typedef unsigned long             Key;
    typedef std::map<Key, Window*>    Map;
    typedef std::pair<Key, Window*>   Pair;
	typedef Map::const_iterator       ConstIterator;
    typedef Map::iterator             Iterator;
    typedef std::pair<Iterator, bool> InsertResult;

	static Map msMap;
	
    PCntType         mClientAreaWidth, mClientAreaHeight;
    Win::HWND        mHandle;
    Win::HINSTANCE   mModule; // the module/instance which owns this window  TODO static?
	Win::HDC         mPaintDevice;
	Win::PAINTSTRUCT mPaintStruct;

	// private lifecycle
    Window(Window const &); // not copyable

	// private operators
    Window &operator=(Window const &); // not assignable

	// misc private methods
	void SetCursor(Win::LPSTR);
};

#endif
